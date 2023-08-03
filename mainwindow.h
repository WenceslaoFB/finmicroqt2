#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QUdpSocket>
#include <QtSerialPort/QSerialPort>
#include <QtNetwork/QHostAddress>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>

typedef union {
    uint8_t u8[4];
    int8_t i8[4];
    uint16_t u16[2];
    int16_t i16[2];
    int32_t i32;
    uint32_t u32;
    float f;
} _sWork;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onQudpSocket1Rx();

    void EnviarComando(uint8_t length, uint8_t cmd, uint8_t payload[]);

    void RecibirComando();

    void on_boton_port_released();

    void on_boton_env_released();

    void on_boton_mot_start_released();

    void on_boton_stop_mot_released();

    void on_bot_jobtime_released();

    void on_bot_PID_released();

    void on_bot_calib_released();

    void Decodificar(int ind, quint8 *buf);

    void RecibirComando(uint8_t ind);

private:
    Ui::MainWindow *ui;

    QUdpSocket *QUdpSocket1;
    quint16 hostPort; //guarda puerto que transmite algo
    QHostAddress hostAdress; //guarda ip que transmite algo

    uint8_t TX[256], payload[8],RX[256],indiceRX=0, buf_UDP[256];
    uint8_t ind_UDP_R=0, ind_UDP_W=0;
    QString mensaje="--> 0x";
    _sWork PWM1, PWM2, jobTime, bufADC[9], kp,kd,ki;
};
#endif // MAINWINDOW_H
