#ifndef DIALOG_H
#define DIALOG_H

#include <QFrame>
#include <QDialog>

#include "ui_buttons.h"

class QVBoxLayout ;

namespace STORE {

/***************************************************************/

class Buttons : public QFrame, private Ui::Buttons {

    Q_OBJECT

  public:
    Buttons( QWidget *parent=0 ) ;
    virtual ~Buttons() {}

  public slots:
    void errorMessage( QString msg = QString() ) ;
    void on_btnOK_clicked() ;

  signals:
    void accepted() ;
    void rejected() ;
    void goodRequest(bool*) ;
    void mustSave() ;

};

/***************************************************************/

class Dialog : public QDialog {

    Q_OBJECT

  private:
    QVBoxLayout *pLayout ;
    QFrame      *pFrame  ;
    Buttons     *pButtons ;

  public:
    Dialog( QWidget *parent = 0 ) ;
    virtual ~Dialog() {}

    QFrame *frame() const { return pFrame ; }
    void setFrame( QFrame *frm ) ;

    void attach( void *pData ) ;

  signals:
    void attached(void*) ;

};

/***************************************************************/

} // namespace STORE

#endif // DIALOG_H

