#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QUdpSocket1 = new QUdpSocket(this);

    connect(QUdpSocket1,&QUdpSocket::readyRead,this,&MainWindow::onQudpSocket1Rx);
}

MainWindow::~MainWindow()
{
    if(QUdpSocket1->isOpen())
        QUdpSocket1->abort();
    delete QUdpSocket1;

    delete ui;
}

void MainWindow::onQudpSocket1Rx() {

    int count;
    uint8_t *buf;
    QString mensajexxx;

    while (QUdpSocket1->hasPendingDatagrams()) {
        count = QUdpSocket1->pendingDatagramSize();
        buf = new uint8_t[count];
        QUdpSocket1->readDatagram((char *)buf,count,&hostAdress, &hostPort);
        //ui->lineEdit_2->setText((char *)buf);
        Decodificar(count,buf);
        for(uint8_t i=0; i<count; i++){
            buf_UDP[ind_UDP_W]=buf[i];
            ind_UDP_W++;
            //mensajexxx = mensajexxx+QString("%1").arg(buf[i], 2, 16, QChar('0')).toUpper();
        }
        //mensaje+=mensajexxx;
        ui->linea_rec->setText((char *)buf);

        delete[] buf;
    }

    ui->textEdit->setText(hostAdress.toString());
    //QUdpSocket1 = new QUdpSocket(this);

    //connect(QUdpSocket1,&QUdpSocket::readyRead,this,&MainWindow::onQudpSocket1Rx);
}


void MainWindow::on_boton_port_released()
{
    quint16 port;
    bool ok;

    if(QUdpSocket1->isOpen()){
        QUdpSocket1->abort();
        ui->boton_port->setText("OPEN");
    }else{
        port = ui->linea_port->text().toInt(&ok);
        if(!ok)
            return;

        if(!QUdpSocket1->bind(port)){
            QMessageBox::warning(this, "UDP PORT", "CANT BLID PORT");
        }
        QUdpSocket1->open(QUdpSocket::ReadWrite);
        ui->boton_port->setText("CLOSE");
    }
}


void MainWindow::on_boton_env_released()
{
    EnviarComando(0x02,0xF0,payload);
}

void MainWindow::EnviarComando(uint8_t length, uint8_t cmd, uint8_t payload[]){
    uint8_t cks;
    QString strhex;
    //QSerialPort1->read((char *)TX, 6 + TX[4]);

    TX[0] = 'U';
    TX[1] = 'N';
    TX[2] = 'E';
    TX[3] = 'R';
    TX[4] = length;
    TX[5] = ':';
    switch (cmd) {
    case 0xF1: //PWM BASE
        TX[6] = cmd;
        TX[7] = PWM1.u8[0];
        TX[8] = PWM1.u8[1];
        TX[9] = PWM1.u8[2];
        TX[10] = PWM1.u8[3];
        TX[11] = PWM2.u8[0];
        TX[12] = PWM2.u8[1];
        TX[13] = PWM2.u8[2];
        TX[14] = PWM2.u8[3];
        break;
    case 0xF0: //Alive
        TX[6] = cmd;
        break;
    case 0xA5://calibrar ADC
        TX[6] = cmd;
        break;
    case 0xA1://env ADC
        TX[6] = cmd;
        break;
    case 0xB3://STOP
        TX[6] = cmd;
        break;
    case 0xD5://START(ASI NOMAS)
        TX[6] = cmd;
        break;
    case 0xC0: //PID PARAMETROS
        TX[6] = cmd;
        TX[7] = kp.u8[0];
        TX[8] = kp.u8[1];
        TX[9] = kp.u8[2];
        TX[10] = kp.u8[3];
        TX[11] = kd.u8[0];
        TX[12] = kd.u8[1];
        TX[13] = kd.u8[2];
        TX[14] = kd.u8[3];
        TX[15] = ki.u8[0];
        TX[16] = ki.u8[1];
        TX[17] = ki.u8[2];
        TX[18] = ki.u8[3];
        break;
    case 0xD0: //MOVER MOTORES(jobtime)
        /*
        PWM1.i32=200;
        PWM2.i32=200;
        jobTime.i32=5000;//5 seg*/

        TX[6]= cmd;
        TX[7]=PWM1.u8[0];
        TX[8]=PWM1.u8[1];
        TX[9]=PWM1.u8[2];
        TX[10]=PWM1.u8[3];

        TX[11]=PWM2.u8[0];
        TX[12]=PWM2.u8[1];
        TX[13]=PWM2.u8[2];
        TX[14]=PWM2.u8[3];

        TX[15]=jobTime.u8[0];
        TX[16]=jobTime.u8[1];
        TX[17]=jobTime.u8[2];
        TX[18]=jobTime.u8[3];
     break;
    }


    cks=0;

    for(int i=0; i<TX[4]+5; i++){
        cks ^= TX[i];
    }
    TX[TX[4]+5] = cks;
    /*
    if(QSerialPort1->isOpen()){
        QSerialPort1->write((char*)TX, 7 + TX[4]);
    }*/

    if(QUdpSocket1->isOpen()){
        QUdpSocket1->writeDatagram((char*)TX,7 + TX[4],hostAdress,hostPort);//pongo TX[4]+6 porque es "lenght" mas los bits del resto del msj
    }
}

void MainWindow::RecibirComando(){

}

void MainWindow::on_boton_mot_start_released()
{
    EnviarComando(0x02,0xD5,payload);
}


void MainWindow::on_boton_stop_mot_released()
{
    EnviarComando(0x02,0xB3,payload);
}


void MainWindow::on_bot_jobtime_released()
{
    PWM1.u32=ui->lineaPWM1->text().toUInt();
    PWM2.u32=ui->lineaPWM2->text().toUInt();
    jobTime.u32=ui->lineaJobtime->text().toUInt()*1000;
    EnviarComando(14,0xD0,payload);
}


void MainWindow::on_bot_PID_released()
{
    kp.i32 = ui->lineaKp->text().toInt();
    kd.i32 = ui->lineaKd->text().toInt();
    ki.i32 = ui->lineaKi->text().toInt();
    EnviarComando(14,0xC0,payload);
}


void MainWindow::on_bot_calib_released()
{
    EnviarComando(0x02,0xA5,payload);
}

void MainWindow::Decodificar(int ind, quint8 *buf)
{
    static int caracter=0;
    static uint8_t nBytes=0, cks=0, indLectura=0;
    int indRecepcion=0;

    while (indRecepcion!=ind) {
        switch(caracter){
        case 0:
            if(buf[indRecepcion]=='U'){
                caracter=1;
            }
            break;
        case 1:
            if(buf[indRecepcion]=='N')
                caracter=2;
            else {
                indRecepcion--;
                caracter=0;
            }
            break;
        case 2:
            if(buf[indRecepcion]=='E')
                caracter=3;
            else {
                indRecepcion--;
                caracter=0;
            }
            break;
        case 3:
            if(buf[indRecepcion]=='R')
                caracter=4;
            else {
                indRecepcion--;
                caracter=0;
            }
            break;
        case 4:
            nBytes=buf[indRecepcion];
            indiceRX=nBytes;
            caracter=5;
            break;
        case 5:
            if(buf[indRecepcion]==':'){
                cks= 'U'^'N'^'E'^'R'^nBytes^':';
                caracter=7;
            }
            else {
                indRecepcion--;
                caracter=0;
            }
            break;
        case 7:
            if(nBytes>1){
                cks^=buf[indRecepcion];
                RX[indLectura++]=buf[indRecepcion]; //guardo datos del buffer en RX para luego ver que llego
            }

            nBytes--;
            if(nBytes==0){
                caracter=0;
                if(cks==buf[indRecepcion]){
                    RecibirComando(indLectura+1-indiceRX);
                }
            }
            break;
        }
        indRecepcion++;
    }
}


void MainWindow::RecibirComando(uint8_t ind){
    static uint8_t asdasd = 0;
    QString ss;
    switch(RX[ind++]){
    case 0xF0:
        asdasd++;
        //ui->lineEdit_3->setText("Recibido ALIVE");
        ui->label_vivo->setText("Recibido ALIVE");
        break;
    case 0xD0:
        asdasd++;
        //ui->lineEdit_3->setText("tiempo configurado");
        ui->label->setText("tiempo configurado");
        break;
    case 0xA1:

        for (uint8_t i = 0; i<9 ;i++) {
            for (uint8_t j = 0; j<4 ; j++) {
                bufADC[i].i8[j] = RX[ind++];
            }
        }
        //asdasd++;
        ui->line_sens->setText("Recibido sensores");
        //ui->label->setNum(asdasd);
        ui->sens1->setNum(bufADC[0].i32);
        ui->sens2->setNum(bufADC[1].i32);
        ui->sens3->setNum(bufADC[2].i32);
        ui->sens4->setNum(bufADC[3].i32);
        ui->sens5->setNum(bufADC[4].i32);
        ui->sens6->setNum(bufADC[5].i32);
        ui->sens7->setNum(bufADC[6].i32);
        ui->sens8->setNum(bufADC[7].i32);
        //ss = QString::number(bufADC[8].f,'f',3);
        //ui->lineEdit_7->setText(ss);
        //  ui->label_12->setn
        //       ui->label_4->setNum();
        break;
        //        case 0xD2:
        //            ui->lineEdit_3->setText("Motores 0xD0");
        //        break;
        //        case 0xF1:
        //            asdasd++;
        //            ui->lineEdit_3->setText("0XF1");
        //            ui->label->setNum(asdasd);
        //        break;
    }
}
