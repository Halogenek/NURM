#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <aboutwindow.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_StartButton_clicked();

    void on_ConnectButton_clicked();

    void on_actionSave_UI_plot_to_PNG_triggered();

    void on_actionSave_UR_plot_to_PNG_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    AboutWindow *aboutwindow;

    int serial_write_uint16_t(uint16_t);
    uint16_t serial_read_uint16_t(void);
    uint16_t convert_double_to_voltage(double number);
    double convert_current_to_double(uint16_t number);
};

#endif // MAINWINDOW_H
