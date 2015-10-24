#include "categories.h"

#include <QtDebug>

#include <QAction>
#include <QMessageBox>
#include <QSqlQuery>
#include <QFile>
#include <QSqlError>

namespace STORE {
namespace Categories {

namespace {

    Data *ptr( const QModelIndex & I ) {
        if ( ! I.isValid() ) throw (int)1 ;
        Data *D = (Data*)I.internalPointer() ;
        if ( ! D ) throw (int)2 ;
        return D ;
    }

    QString query_text( QString path ) {
        QFile SRC( path ) ;
        if ( ! SRC.open(QIODevice::ReadOnly|QIODevice::Text) ) {
            return QString() ;
        }
        return SRC.readAll() ;
    }

} // namespace

/******************************************************/

Frame::Frame( QWidget *parent )
  : QFrame(parent),
    pData(0) {
    setupUi(this) ;
}

void Frame::attach( void *ptr ) {
    if ( ptr ) {
        pData = (Data*)ptr ;
        load() ;
    } else {
        pData = 0 ;
    }
}

void Frame::load() {
    if ( ! pData ) return ;
    edtStatCode->setText( pData->StatCode.toString() ) ;
    edtTitle   ->setText( pData->Title   .toString() ) ;
    edtRegisteredAt->setDateTime( pData->RegisteredAt.toDateTime() );
    if ( pData->LockedAt.isValid() ) {
        cbxLockedAt->setChecked( true ) ;
        edtLockedAt->setDateTime( pData->LockedAt.toDateTime() ) ;
    } else {
        cbxLockedAt->setChecked( false ) ;
        edtLockedAt->setDateTime( QDateTime::currentDateTime() ) ;
    }
    if ( pData->Fast.isValid() ) {
        Qt::CheckState C = pData->Fast.toBool() ? Qt::Checked
                                                : Qt::Unchecked ;
        cbxFast->setCheckState( C );
    } else {
        cbxFast->setCheckState( Qt::PartiallyChecked );
    }
    edtComment->setPlainText( pData->Comment.toString() );
}

void Frame::save() {
    if ( ! pData ) return ;

    QString T ;

    T = edtStatCode->text().simplified() ;
    pData->StatCode = T.isEmpty() ? QVariant() : QVariant(T) ;

    T = edtTitle->text().simplified() ;
    pData->Title = T.isEmpty() ? QVariant() : QVariant(T) ;

    pData->RegisteredAt = edtRegisteredAt->dateTime() ;

    if ( cbxLockedAt->isChecked() ) {
        pData->LockedAt = edtLockedAt->dateTime() ;
        // TODO Сбросить время в ноль
    } else {
        pData->LockedAt = QVariant() ;
    }

    switch ( cbxFast->checkState() ) {
    case Qt::Checked : pData->Fast = true ; break ;
    case Qt::Unchecked : pData->Fast = false ; break ;
    default: pData->Fast = QVariant() ; break ;
    }

    T = edtComment->toPlainText().trimmed() ;
    pData->Comment = T.isEmpty() ? QVariant() : QVariant(T) ;

}

void Frame::canClose( bool *pOK ) {
}

/******************************************************/

Dialog::Dialog( QWidget *parent )
  : STORE::Dialog(parent) {
    setFrame( new Frame(this) ) ;
}

/******************************************************/

QStringList Model::Headers ;

Model::Model( QObject *parent )
  : QAbstractTableModel(parent) {

    {
     QFile SRC( ":sql/all_categories" ) ;
     SRC.open( QIODevice::ReadOnly | QIODevice::Text ) ;
     QString QueryText = SRC.readAll() + " ;" ;
     QSqlQuery QRY ;
     QRY.setForwardOnly( true ) ;
     QRY.prepare( QueryText ) ;
     bool OK = QRY.exec() ;
     if ( OK ) {
         while( QRY.next() ) {
             Data *D = new Data ;
             D->Id           = QRY.value( "iid"           ) ;
             D->IdParent     = QRY.value( "rid_parent"    ) ;
             //D->Icon         = QRY.value( "icon"       ) ;
             D->Title        = QRY.value( "title"         ) ;
             D->RegisteredAt = QRY.value( "registered_at" ) ;
             D->Fast         = QRY.value( "fast"          ) ;
             D->StatCode     = QRY.value( "stat_code"     ) ;
             D->LockedAt     = QRY.value( "locked_at"     ) ;
             D->Comment      = QRY.value( "acomment"      ) ;
             Categories.append( D ) ;
         }
     } else {
         qCritical() << "=================================" ;
         qCritical() << "Cannot execute query" ;
         qCritical() << QueryText ;
         qCritical() << "---------------------------------" ;
         qCritical() << QRY.lastError().nativeErrorCode() ;
         qCritical() << "---------------------------------" ;
         qCritical() << QRY.lastError().driverText() ;
         qCritical() << "---------------------------------" ;
         qCritical() << QRY.lastError().databaseText() ;
         qCritical() << "=================================" ;
     }
    }

    if ( Headers.size() <= 0 ) {
        Headers << tr("Icon")
                << tr("Title")
                << tr("Registered")
                << tr("Fast")
                << tr("Stat Code")
                << tr("Count")
                << tr("Valid through")
                << tr("Comment") ;
    }

}

Model::~Model() {
    QListIterator<Data*> it(Categories) ;
    while( it.hasNext() ) {
        Data *item = it.next() ;
        if ( item ) delete item ;
    }
}

QVariant Model::headerData(
                    int section,
                    Qt::Orientation orientation,
                    int role ) const {
    if ( orientation == Qt::Horizontal ) {
        switch ( role ) {
        case Qt::DisplayRole :
            return Headers.value( section, QString() ) ;
        default :
            break ;
        }
    }
    return QAbstractTableModel::headerData(section,orientation,role) ;
}

int Model::rowCount( const QModelIndex &parent ) const {
    if ( parent.isValid() ) return 0 ;
    return Categories.size() ;
}

int Model::columnCount( const QModelIndex &parent ) const {
    if ( parent.isValid() ) return 0 ;
    return 8 ;
}

QVariant Model::data( const QModelIndex &I, int role ) const {
    if ( ! I.isValid() ) return QVariant() ;
    try {
        switch ( role ) {
        case Qt::DisplayRole       : return dataDisplay  (I) ;
        case Qt::TextAlignmentRole : return dataAlignment(I) ;
        case Qt::FontRole          : {
            if ( ! ptr(I)->changed ) return QVariant() ;
            QFont F ;
            F.setBold( true ) ;
            return F ;
        }
        case Qt::BackgroundRole    : {
            if ( ptr(I)->Id.isValid() ) return QVariant() ;
            return QColor("lightgray") ;
        }
        case Qt::ForegroundRole    : {
            Data *D = ptr(I) ;
            if ( ! D->Fast.isValid() ) return QVariant() ;
            if ( D->Fast.toBool() ) return QColor("red") ;
            return QColor("green") ;
        }
        case Qt::UserRole          : return dataUser( I ) ;
        case Qt::UserRole+1        : return ptr(I)->Title ;
        default:
            return QVariant() ;
        }
    } catch ( int ) { return QVariant() ; }
}

QVariant Model::dataAlignment( const QModelIndex &I ) const {
    int Result = Qt::AlignVCenter ;
    switch( I.column() ) {
    case 2 :
    case 4 :
    case 6 : return Result | Qt::AlignHCenter ;
    default: return Result | Qt::AlignLeft ;
    }
}

QVariant Model::dataUser( const QModelIndex &I ) const {
    return I.row() ;
}

QVariant Model::dataDisplay( const QModelIndex &I ) const {
    bool OK ;
    int idx = data(I,Qt::UserRole).toInt(&OK) ;
    if ( ! OK ) return QVariant() ;
    Data *D = Categories[idx] ;
    switch ( I.column() ) {
        // QVariant Icon ;
    case 1 : return D->Title ;
    case 2 : return D->RegisteredAt.toDateTime().date() ;
    case 3 :
        if ( ! D->Fast.isValid() ) return QVariant() ;
        return D->Fast.toBool() ? tr("YES") : tr("no") ;
    case 4 : return D->StatCode ;
    case 5 : return D->Count ;
    case 6 : return D->LockedAt.toDateTime().date() ;
    case 7 : return D->Comment.isValid() ? "*" : "" ;
    default:
        return QString("(%1,%2)").arg(I.row()).arg(I.column()) ;
    }
}

QModelIndex Model::index( int row, int column,
                          const QModelIndex &parent ) const {
    if ( parent.isValid() ) return QModelIndex() ;
    Data *D = Categories[row] ;
    return createIndex( row, column, (void*)D ) ;
}

void Model::append( Data *new_item ) {
    // TODO тут вычислить QModelIndex
    beginResetModel();
    Categories.append( new_item ) ;
    endResetModel() ;
}

void Model::remove( const QModelIndex &I ) {
    bool OK ;
    int idx = data(I,Qt::UserRole).toInt(&OK) ;
    if ( ! OK ) return ;
    beginResetModel();
    Data *D = Categories.takeAt( idx ) ;
    if ( D ) delete D ;
    endResetModel();
}

void Model::edited( const QModelIndex &I ) {
    beginResetModel();
    try { ptr(I)->changed = true ; } catch ( int ) {}
    endResetModel();
}

void Model::save() {
    { // Сохраняем новые категории
     QSqlQuery QRY ;
     QRY.prepare( query_text(":sql/new_category") + " ;" ) ;
     beginResetModel();
     QListIterator<Data*> LI( Categories ) ;
     while ( LI.hasNext() ) {
         Data *D = LI.next() ;
         if ( D->Id.isValid() ) continue ;
         QRY.bindValue( ":rid_parent"    , D->IdParent     ) ;
         QRY.bindValue( ":icon"          , D->Icon         ) ;
         QRY.bindValue( ":title"         , D->Title        ) ;
         QRY.bindValue( ":registered_at" , D->RegisteredAt ) ;
         QRY.bindValue( ":fast"          , D->Fast         ) ;
         QRY.bindValue( ":stat_code"     , D->StatCode     ) ;
         QRY.bindValue( ":locked_at"     , D->LockedAt     ) ;
         QRY.bindValue( ":acomment"      , D->Comment      ) ;
         QRY.exec() ;
         if ( QRY.next() ) {
             D->Id      = QRY.value("iid") ;
             D->changed = false            ;
         }
     }
     endResetModel();
    }{ // Сохраняем измененные категории

    }
}

/******************************************************/

View::View( QWidget *parent )
  : QTableView( parent ) {

    Model *M = new Model( this ) ;
    setModel( M ) ;

    setContextMenuPolicy( Qt::ActionsContextMenu ) ;
    setSelectionBehavior( SelectRows ) ;
    setSelectionMode( SingleSelection );

    {
     QHeaderView *H = verticalHeader() ;
     H->setSectionResizeMode( QHeaderView::ResizeToContents ) ;
    }{
     QHeaderView *H = horizontalHeader() ;
     H->setSectionResizeMode( QHeaderView::ResizeToContents ) ;
     H->setSectionResizeMode( 1, QHeaderView::Stretch ) ;
    }

    {
     QAction *A = new QAction( this ) ;
     A->setText( tr("Add category") ) ;
     connect( A, SIGNAL(triggered()), this, SLOT(on_add_cat()) ) ;
     addAction(A) ;
    }{
     QAction *A = new QAction( this ) ;
     A->setText( tr("Edit category") ) ;
     connect( A, SIGNAL(triggered()), this, SLOT(on_edit_cat()) ) ;
     addAction(A) ;
    }{
     QAction *A = new QAction( this ) ;
     A->setText( tr("Delete category") ) ;
     connect( A, SIGNAL(triggered()), this, SLOT(on_del_cat()) ) ;
     addAction(A) ;
    }{
     QAction *A = new QAction( this ) ;
     A->setText( tr("Save all") ) ;
     connect( A, SIGNAL(triggered()), M, SLOT(save())) ;
     addAction( A ) ;
    }

    connect( this, SIGNAL(cat_appended(Data*)), M, SLOT(append(Data*)) ) ;
    connect( this, SIGNAL(cat_deleted(const QModelIndex &)),
             M, SLOT(remove(const QModelIndex &)) ) ;
    connect( this, SIGNAL(cat_edited(const QModelIndex &)),
             M, SLOT(edited(const QModelIndex &)) ) ;

}

void View::on_add_cat() {

    Data *D = new Data ;
    D->Title = "Какая-то фигня" ;

    Dialog Dia(this) ;
    Dia.attach( D ) ;
    if ( Dia.exec() == QDialog::Accepted )
        emit cat_appended(D);
    else
        delete D ;
}

void View::on_del_cat() {

    QModelIndex I = currentIndex() ;
    if ( !I.isValid() ) return ;

    QString T = tr("Are you sure to delete category?") ;
    T += "\n" ;
    T += I.data( Qt::UserRole+1 ).toString() ;

    int Answer = QMessageBox::question( this,
                           tr("Confirm"),
                           T ) ;

    if ( Answer == QMessageBox::Yes )
        emit cat_deleted( I ) ;

}

void View::on_edit_cat() {

    QModelIndex I = currentIndex() ;
    if ( !I.isValid() ) return ;

    void *D = I.internalPointer() ;
    if ( ! D ) return ;

    Dialog Dia(this) ;
    Dia.attach( D ) ;
    if ( Dia.exec() == QDialog::Accepted )
        emit cat_edited(I);

}

/******************************************************/

} // namespace Categories
} // namespace STORE

