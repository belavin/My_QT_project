#include "application.h"

#include <QtDebug>

#include <QSqlDatabase>

namespace STORE {

Application::Application( int argc, char *argv[] )
  : QApplication( argc, argv ) {

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL") ;
    db.setHostName    ( "localhost" );
    db.setPort        ( 5432        );
    db.setDatabaseName( "store"     );
    // TODO Это спрашивать у пользователя
    db.setUserName( "dbowner" );
    db.setPassword( "1"       );

    bool OK = db.open() ;
    if ( ! OK )
        qCritical() << "Cannot connect ro database" ;
    else
        qDebug() << "Database connected" ;

}

} // namespace STORE

