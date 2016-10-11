#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

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
    ui->UIplot->xAxis->setLabel("Voltage [V]");
    ui->UIplot->yAxis->setLabel("Current [A]");
    ui->UIplot->xAxis->setRange(0, 5);
    ui->UIplot->yAxis->setRange(0, 0.1);
    ui->URplot->addGraph();
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

}

MainWindow::~MainWindow()
{
    delete ui;
    serial_port->close();
}

void MainWindow::on_StartButton_clicked()
{
    ;
}

void MainWindow::on_ConnectButton_clicked()
{
    serial_port->setPortName(ui->ComPortsList->currentText());
    serial_port->open(QIODevice::ReadWrite);
    serial_port->write("Hello World!");
}
