#include "aboutwindow.h"
#include "ui_aboutwindow.h"

AboutWindow::AboutWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);
    //set logo in about window
    //QPixmap pix("halide_logo.png");
    //ui->aboutLogo->setPixmap(pix);
}

AboutWindow::~AboutWindow()
{
    delete ui;
}
