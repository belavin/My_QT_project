#ifndef CATEGORIES_H
#define CATEGORIES_H

#include <QAbstractTableModel>
#include <QList>
#include <QTableView>

#include "dialog.h"

#include "ui_category.h"

class QAction ;

namespace STORE {
namespace Categories {

/******************************************************/

struct Data {
    QVariant Id ;
    QVariant IdParent ;
    QVariant Icon ;
    QVariant Title ;         //
    QVariant RegisteredAt ;
    QVariant Fast ;
    QVariant StatCode ;      //
    QVariant Count ;
    QVariant LockedAt ;
    QVariant Comment ;
    bool changed ;
    Data() : changed(false) {}
};

/******************************************************/

class Frame : public QFrame, Ui::Category {

    Q_OBJECT

  private:
    Data *pData ;

  public:
    Frame( QWidget *parent = 0 ) ;
    virtual ~Frame() {}

  public slots:
    void load() ;
    void save() ;
    void canClose(bool*) ;
    void attach(void*) ;

  signals:
    void errorMessage(QString) ;

};

/******************************************************/

class Dialog : public STORE::Dialog {
    Q_OBJECT
  public:
    Dialog( QWidget *parent=0 ) ;
    virtual ~Dialog(){}
};

/******************************************************/

class Model : public QAbstractTableModel {

    Q_OBJECT

  private:
    QList<Data*> Categories ;
    static QStringList Headers ;

  public:
    Model( QObject *parent=0 ) ;
    virtual ~Model() ;

    int rowCount( const QModelIndex &parent ) const ;
    int columnCount( const QModelIndex &parent ) const ;

    QVariant headerData( int section,
                         Qt::Orientation orientation,
                         int role = Qt::DisplayRole ) const ;

    QVariant data( const QModelIndex &I,
                   int role = Qt::DisplayRole ) const ;

    QModelIndex index( int row, int column,
                       const QModelIndex &parent = QModelIndex() ) const ;

  protected:
    virtual QVariant dataDisplay  ( const QModelIndex &I ) const ;
    virtual QVariant dataUser     ( const QModelIndex &I ) const ;
    virtual QVariant dataAlignment( const QModelIndex &I ) const ;

  public slots:
    void append( Data *new_item ) ;
    void remove( const QModelIndex &I ) ;
    void edited( const QModelIndex &I ) ;

    void save() ;
    //void load() ;

};

/******************************************************/

class View : public QTableView {

    Q_OBJECT

  private:
    QAction *pNewCatAction ;

  public:
    View( QWidget *parent=0 ) ;
    virtual ~View() {}

  private slots:
    void on_add_cat() ;
    void on_edit_cat() ;
    void on_del_cat() ;

  signals:
    void cat_appended( Data *new_item ) ;
    void cat_deleted( const QModelIndex &I ) ;
    void cat_edited( const QModelIndex &I ) ;

};

/******************************************************/

} // namespace Categories
} // namespace STORE


#endif // CATEGORIES_H

