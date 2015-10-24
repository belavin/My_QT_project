#include "mainwindow.h"

#include "categories.h"

namespace STORE {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    Categories::View *CW = new Categories::View(this) ;
    setCentralWidget( CW ) ;

}

MainWindow::~MainWindow() {
}

} // namespace STORE
