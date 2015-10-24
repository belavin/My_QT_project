#include "dialog.h"

#include <QTimer>
#include <QLayout>

namespace STORE {

/***************************************************************/

Buttons::Buttons( QWidget *parent )
  : QFrame(parent) {

    setupUi(this) ;

    connect( btnCancel, SIGNAL(clicked()), this, SIGNAL(rejected()) ) ;
}

void Buttons::errorMessage( QString msg ) {
    lblErrorMessage->setText( msg ) ;
    if ( ! msg.isEmpty() )
        QTimer::singleShot( 30000, this, SLOT(errorMessage()) ) ;
}

void Buttons::on_btnOK_clicked() {
    bool OK = true ;
    emit goodRequest(&OK);
    if ( OK ) {
        emit mustSave() ;
        emit accepted() ;
    }
}

/***************************************************************/

Dialog::Dialog( QWidget *parent )
  : QDialog( parent ) {

    QVBoxLayout *L = pLayout = new QVBoxLayout(this) ;

    Buttons *B = pButtons = new Buttons(this) ;
    L->addWidget(B);

    connect( B, SIGNAL(accepted()), this, SLOT(accept()) ) ;
    connect( B, SIGNAL(rejected()), this, SLOT(reject()) ) ;

    // connect( B, &Buttons::rejected, this, &Dialog::reject ) ;

}

void Dialog::setFrame( QFrame *frm ) {

    frm->setParent( this ) ;
    pLayout->insertWidget( 0, frm ) ;
    frm->setTabOrder( frm, pButtons ) ;
    pFrame = frm ;

    connect( pFrame, SIGNAL(errorMessage(QString)),
             pButtons, SLOT(errorMessage(QString)) ) ;

    connect( pButtons, SIGNAL(goodRequest(bool*)),
             pFrame, SLOT(canClose(bool*)) ) ;

    connect( pButtons, SIGNAL(mustSave()),
             pFrame, SLOT(save()) ) ;

    connect( this, SIGNAL(attached(void*)),
             pFrame, SLOT(attach(void*)) ) ;

}

void Dialog::attach( void *pData ) {
    emit attached( pData ) ;
}

/***************************************************************/

} // namespace STORE

