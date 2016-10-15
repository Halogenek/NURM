#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFileDialog>

//CREATE POINTER FOR SERIAL PORT
QSerialPort *serial_port;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //GET LIST OF COM PORTS
    for(QSerialPortInfo port : QSerialPortInfo::availablePorts())
        ui->ComPortsList->addItem(port.portName());
    //CONFIGURE GRAPS
    ui->UIplot->addGraph();
    ui->UIplot->plotLayout()->insertRow(0);
    ui->UIplot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->UIplot, "Current To Voltage Relation", QFont("sans", 9, QFont::Bold)));
    ui->UIplot->xAxis->setLabel("Voltage [V]");
    ui->UIplot->yAxis->setLabel("Current [A]");
    ui->UIplot->xAxis->setRange(0, 5);
    ui->UIplot->yAxis->setRange(0, 0.1);
    ui->URplot->addGraph();
    ui->URplot->plotLayout()->insertRow(0);
    ui->URplot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->URplot, "Dynamic Resistance", QFont("sans", 9, QFont::Bold)));
    ui->URplot->xAxis->setLabel("Voltage [V]");
    ui->URplot->yAxis->setLabel("Resistance [Ω]");
    ui->URplot->xAxis->setRange(0, 5);
    ui->URplot->yAxis->setRange(0, 50);
    //CONFIGURE SERIAL PORT
    serial_port = new QSerialPort(this);
    serial_port->setBaudRate(QSerialPort::Baud9600);
    serial_port->setDataBits(QSerialPort::Data8);
    serial_port->setParity(QSerialPort::NoParity);
    serial_port->setStopBits(QSerialPort::OneStop);
    serial_port->setFlowControl(QSerialPort::NoFlowControl);
    //BLOCK THE START BUTTON
    ui->StartButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    serial_port->close();
}

void MainWindow::on_StartButton_clicked()
{
    //start the measurment
    //get the max values
    double maxu, maxi;
    maxu = ui->MaxU->value();
    maxi = ui->MaxI->value();
    //for each minimal step send voltage as string to microcontroler and receive string with amps then plot that
    for(double step = 0; step < maxu; step += 0.005){
        char voltage_string[6];
        snprintf(voltage_string, sizeof(voltage_string), "%g", step);
        for(int character_number = 0; character_number < sizeof(voltage_string); character_number++){
            serial_port->write(&voltage_string[character_number], 1);
        }
        char amp_string[6];
        serial_port->read(amp_string, 6);
        //if the computer wont receive data for more then 200ms stop measurment
        if(serial_port->waitForReadyRead(200) != true){
            QMessageBox msgBox;
            msgBox.setText("Device offline!");
            msgBox.exec();
            ui->StartButton->setEnabled(false);
            return;
        }
    }
}

void MainWindow::on_ConnectButton_clicked()
{
    serial_port->setPortName(ui->ComPortsList->currentText());
    serial_port->open(QIODevice::ReadWrite);
    //test the connection
    serial_port->write("1", 1);
    char test;
    serial_port->read(&test, 1);
    if (test == '1'){
        QMessageBox msgBox;
        msgBox.setText("Device connected!");
        msgBox.exec();
        //allow to start the measurment
        ui->StartButton->setEnabled(true);
    }
}

void MainWindow::on_actionSave_UI_plot_to_PNG_triggered()
{
    //bring up a window to ask where save the plot and save it
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Plot"), "UIplot.png", tr("Images (*.png)"));
    ui->UIplot->savePng(filename, 850, 400, 1.0, 10);
}

void MainWindow::on_actionSave_UR_plot_to_PNG_triggered()
{
    //bring up a window to ask where save the plot and save it
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Plot"), "URplot.png", tr("Images (*.png)"));
    ui->URplot->savePng(filename, 850, 400, 1.0, 10);
}

void MainWindow::on_actionAbout_triggered()
{
    aboutwindow = new AboutWindow(this);
    aboutwindow->show();
}
