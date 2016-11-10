#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFileDialog>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define TIMER_STEPS   1024
#define ADC_RES       1024
#define ADC_VOL_REF   1.1
#define PWM_VOL_MAX   5

//CREATE POINTER FOR SERIAL PORT
QSerialPort *serial_port;

uint16_t MainWindow::serial_read_uint16_t(void) {
    char string[5];
    serial_port->read(string, sizeof(string));
    //if the computer wont receive data for more then 200ms stop measurment
    if(serial_port->waitForReadyRead(200) != true) {
        QMessageBox msgBox;
        msgBox.setText("Device offline!");
        msgBox.exec();
        //ui->StartButton->setEnabled(false);
        return -1;
    }
    uint16_t number = 0;
    unsigned char counter;
    for (counter = 0; counter < sizeof(string) &&
        string[counter] >= '0' && string[counter] <= '9'; counter++) {
      number = number * 10 + string[counter] - '0';
    }
    return number;
    //return atoi(string);
}

int MainWindow::serial_write_uint16_t(uint16_t number) {
    char string[5];
    snprintf(string, sizeof(string), "%04u", number);
    serial_port->write(string, sizeof(string));
    if(serial_port->waitForBytesWritten(50) == false) {
        QMessageBox msgBox;
        msgBox.setText("Device offline!");
        msgBox.exec();
        //ui->StartButton->setEnabled(false);
        return -1;
    }
    return 0;
}

uint16_t MainWindow::convert_double_to_voltage(double number) {
    return (uint16_t)(number * ((TIMER_STEPS - 1.0) / 5));
}

double MainWindow::convert_current_to_double(uint16_t number) {
    return (double)(number * (ADC_VOL_REF / (ADC_RES - 1.0)));
}

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
    ui->UIplot->xAxis->setRange(0, 2.6);
    ui->UIplot->yAxis->setRange(0, 0.1);
    ui->URplot->addGraph();
    ui->URplot->plotLayout()->insertRow(0);
    ui->URplot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->URplot, "Dynamic Resistance", QFont("sans", 9, QFont::Bold)));
    ui->URplot->xAxis->setLabel("Voltage [V]");
    ui->URplot->yAxis->setLabel("Resistance [Ω]");
    ui->URplot->xAxis->setRange(0, 2.6);
    ui->URplot->yAxis->setRange(0, 50);
    //CONFIGURE SERIAL PORT
    serial_port = new QSerialPort(this);
    serial_port->setBaudRate(QSerialPort::Baud9600);
    serial_port->setDataBits(QSerialPort::Data8);
    serial_port->setParity(QSerialPort::NoParity);
    serial_port->setStopBits(QSerialPort::OneStop);
    serial_port->setFlowControl(QSerialPort::NoFlowControl);
    //BLOCK THE START BUTTON
    ui->StartButton->setEnabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    serial_port->close();
}

void MainWindow::on_StartButton_clicked()
{
    //clear the graphs
    ui->UIplot->graph(0)->setData((QVector<double>)0, (QVector<double>)0);
    ui->URplot->graph(0)->setData((QVector<double>)0, (QVector<double>)0);
    //start the measurment
    //get the max values
    double max_u, max_i, actual_current, maximal_actual_current, maximal_actual_resistance;
    actual_current = maximal_actual_current = maximal_actual_resistance = 0;
    max_u = ui->MaxU->value();
    max_i = ui->MaxI->value();
    //serial_write_uint16_t(convert_double_to_voltage(1.1)); //////////////////////////////////////////////////////////////////////////////////////////
    //for each minimal step send voltage as string to microcontroler and receive string with amps then plot that
    for(double actual_voltage = 0; actual_voltage < max_u; actual_voltage += 0.005){
        actual_current = actual_voltage/100;
        if(serial_write_uint16_t(convert_double_to_voltage(actual_voltage)) == -1) {
            return;
        }
        actual_current = convert_current_to_double(serial_read_uint16_t());
        if(actual_current == -1) {
            return;
        }
        ui->UIplot->graph(0)->addData(actual_voltage, actual_current);
        ui->UIplot->xAxis->setRange(0, actual_voltage);
        if(actual_current > maximal_actual_current){
            maximal_actual_current = actual_current;
            ui->UIplot->yAxis->setRange(0, maximal_actual_current);
        }
        ui->UIplot->replot();
        //if the maximum current is reached
        //if(actual_current >= max_i) {
        //    serial_write_uint16_t(0);
        //    return;
        //}
        if(actual_current != 0){
            ui->URplot->graph(0)->addData(actual_voltage, actual_voltage/actual_current);
            ui->URplot->xAxis->setRange(0, actual_voltage);
            if(actual_voltage/actual_current > maximal_actual_resistance) {
                maximal_actual_resistance = actual_voltage/actual_current;
                ui->URplot->yAxis->setRange(0, maximal_actual_resistance);
            }
            ui->URplot->replot();
        }
    }
}

void MainWindow::on_ConnectButton_clicked()
{
    serial_port->setPortName(ui->ComPortsList->currentText());
    serial_port->open(QIODevice::ReadWrite);
    //test the connection
    //char test;
    //serial_port->write("1", 1);
    //serial_port->waitForBytesWritten(100);
    //serial_port->read(&test, 1);
    //serial_port->waitForReadyRead(200);
    //if (test == '1'){
    //    QMessageBox msgBox;
    //    msgBox.setText("Device connected!");
    //    msgBox.exec();
        //allow to start the measurment
     //   ui->StartButton->setEnabled(true);
     //   ui->ComPortsList->setEnabled(false);
     //   ui->ConnectButton->setEnabled(false);
    //}
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
