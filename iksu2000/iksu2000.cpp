#include "iksu2000.h"
#include <QtWidgets>


Iksu2000Plugin::Iksu2000Plugin()
{
    serial = new  QSerialPort();
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(on_serrial_error(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(aboutToClose()), this, SLOT(on_close()));
}

Iksu2000Plugin::~Iksu2000Plugin()
{
    if (serial->isOpen()) serial->close();
    delete serial;
}

void Iksu2000Plugin::initialization(QSettings *settings)
{
    this->settings = settings;
    prev_command=-1;
    last_mode = -1;
    //Открываем и настраиваем СОМ-порт
    portName = settings->value("iksu-2000/port").toString();
    ZN = settings->value("iksu-2000/zn").toString();
    SKN = settings->value("iksu-2000/skn").toString();
    SROK = settings->value("iksu-2000/srok").toDate();
    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setFlowControl(QSerialPort:: SoftwareControl);
    if (serial->open(QIODevice::ReadWrite)) {
        emit this->log("Порт "+portName+" успешно открыт!");
    }
    else {
        emit this->log("Не удалось открыть порт "+portName+"!", Qt::red);
    }
}

QString Iksu2000Plugin::getName()
{
    QString str = "ИКСУ-2000";
    return str;
}

int Iksu2000Plugin::showInfo()
{
    Dialog d;
    d.setWindowTitle("ИКСУ-2000");
    d.setModal(true);
    return d.exec();
}

void Iksu2000Plugin::test()
{
    iksu2000Dialog.exec();
}

bool Iksu2000Plugin::setValue(float value, measurement type_value)
{
    //преобразуем значение в строку
    QString str_val = QString::number(value, 'g', 6);
    //Определяем есть ли в начале минус
    bool minus=false;
    if (str_val[0]=='-') {
        minus=true;
        str_val.remove(0,1);
    }

    //Делим строку на целую и дробную части
    QStringList str_list = str_val.split(".");
    //готовим переменную для записи обработанного числа
    str_val="";
    //длина целой и дробной части
    int l1,l2;
    //Ошибочное значение
    int error_val = 0;
    //Добавляем дробную часть
    if (str_list.count()<2) { str_list.append("0"); }
    //Проверяем тип измерения
    switch (type_value) {
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //Если необхобимо задать милиамперы:
    case mA: qDebug()<<value<<" mA";
        if (value<0||value>20) { error_val=1; break; }
        l1=2;l2=3;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //Если необхобимо задать миливольты:
    case mV: qDebug()<<value<<" mV";
        if (value>100||value<-11) { error_val=1; break; }
        l1=3;l2=2;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(13);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //Если необхобимо задать вольты:
    case V: qDebug()<<value<<" V";
        if (value>12||value<0) { error_val=1; break; }
        l1=2;l2=3;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(13);sendCOM(13);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //Если необхобимо задать омы:
    case Om:
        if (value>300||value<-50) { error_val=1; break; }
        l1=3;l2=2;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(14);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термосопротивление 100М(1,426):
    case C100M1_426:
        if (value>200||value<-50) { error_val=1; break; }
        l1=3;l2=2;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(14);sendCOM(14);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термосопротивление 100М(1,428):
    case C100M1_428:
        if (value>200||value<-50) { error_val=1; break; }
        l1=3;l2=2;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(14);sendCOM(14);sendCOM(14);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термосопротивление 50М(1,426):
    case C50M1_426:
        if (value>200||value<-50) { error_val=1; break; }
        l1=3;l2=2;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(14);sendCOM(14);sendCOM(14);sendCOM(14);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термосопротивление 50М(1,428):
    case C50M1_428:
        if (value>200||value<-50) { error_val=1; break; }
        l1=3;l2=2;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(14);sendCOM(14);sendCOM(14);sendCOM(14);sendCOM(14);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термосопротивление 50P(RUS):
    case C50P:
        if (value>600||value<-200) { error_val=1; break; }
        l1=3;l2=2;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(14);sendCOM(14);sendCOM(14);sendCOM(14);sendCOM(14);sendCOM(14);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термосопротивление 100P(RUS):
    case C100P:
        if (value>600||value<-200) { error_val=1; break; }
        l1=3;l2=2;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(13);sendCOM(13);sendCOM(13);sendCOM(13);sendCOM(13);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термосопротивление CPt100 IEC(385):
    case CPt100IEC385:
        if (value>600||value<-200) { error_val=1; break;}
        l1=3;l2=2;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(13);sendCOM(13);sendCOM(13);sendCOM(13);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термопара  тип J(IEC):
    case CTypeJ:
        if (value>600||value<-200) { error_val=1; break; }
        l1=4;l2=1;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(13);sendCOM(13);sendCOM(13);sendCOM(12);
        while (Ctype!=0){
            sendCOM(13);
            Ctype+=1;
            if (Ctype>5) Ctype=0;
        }
        sendCOM(12);
        //термокомпенсация ручная 20 С
        sendCOM(14);sendCOM(2);sendCOM(0);sendCOM(0);sendCOM(12);
        break;

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термопара  тип K(IEC):
    case CTypeK:
        if (value>1300||value<-200) { error_val=1; break; }
        l1=4;l2=1;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(13);;sendCOM(13);sendCOM(13);sendCOM(12);
        while (Ctype!=1){
            sendCOM(13);
            Ctype+=1;
            if (Ctype>5) Ctype=0;
        }
        sendCOM(12);
        //термокомпенсация ручная 20 С
        sendCOM(14);sendCOM(2);sendCOM(0);sendCOM(0);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термопара  тип B(IEC):
    case CTypeB:
        if (value>1800||value<300) { error_val=1; break; }
        l1=4;l2=1;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(13);;sendCOM(13);sendCOM(13);sendCOM(12);
        while (Ctype!=2){
            sendCOM(13);
            Ctype+=1;
            if (Ctype>5) Ctype=0;
        }
        sendCOM(12);
        //термокомпенсация ручная 20 С
        sendCOM(14);sendCOM(2);sendCOM(0);sendCOM(0);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термопара  тип A1(IEC):
    case CTypeA1:
        if (value>1800||value<0) { error_val=1; break; }
        l1=4;l2=1;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(13);;sendCOM(13);sendCOM(13);sendCOM(12);
        while (Ctype!=3){
            sendCOM(13);
            Ctype+=1;
            if (Ctype>5) Ctype=0;
        }
        sendCOM(12);
        //термокомпенсация ручная 20 С
        sendCOM(14);sendCOM(2);sendCOM(0);sendCOM(0);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термопара  тип S(IEC):
    case CTypeS:
        if (value>1300||value<300) { error_val=1; break; }
        l1=4;l2=1;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(13);;sendCOM(13);sendCOM(13);sendCOM(12);
        while (Ctype!=4){
            sendCOM(13);
            Ctype+=1;
            if (Ctype>5) Ctype=0;
        }
        sendCOM(12);
        //термокомпенсация ручная 20 С
        sendCOM(14);sendCOM(2);sendCOM(0);sendCOM(0);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термопара  тип ХК (Рус):
    case CTypeXK:
        if (value>600||value<-200) { error_val=1; break; }
        l1=4;l2=1;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(13);;sendCOM(13);sendCOM(13);sendCOM(12);
        while (Ctype!=5){
            sendCOM(13);
            Ctype+=1;
            if (Ctype>5) Ctype=0;
        }
        sendCOM(12);
        //термокомпенсация ручная 20 С
        sendCOM(14);sendCOM(2);sendCOM(0);sendCOM(0);sendCOM(12);
        break;
    default:
        error_val = 2;
        break;
    }

    if (error_val) {emit this->set_value_error();return false;}

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    qDebug()<<"Унифицированный вывод:";
    str_val = str_val=str_list.at(0).rightJustified(l1,'0') + str_list.at(1).leftJustified(l2, '0').left(l2);
    qDebug()<<str_val;
    //Если число отрицательное
    if (minus) sendCOM(15);
    //Задаем значение на ИКСУ-2000 по каждому из полученных символов
    foreach (QChar s, str_val) {
        qDebug()<<s;
        sendCOM(QString(s).toInt());
    }
    //Включаем эмуляцию и посылаем сигнал об успешном завершении
    sendCOM(12);
    emit this->set_value_ok();
    last_mode = type_value;
}

bool Iksu2000Plugin::setup()
{
    emit this->log("Изменение настроек");
    SetupDialog sd;
    sd.setModal(true);
    sd.setPort(portName);
    sd.setZN(ZN);
    sd.setSKN(SKN);
    sd.setSROK(SROK);
    if  (sd.exec() == QDialog::Accepted) {
        if (portName!=sd.getPort())
        {
            portName = sd.getPort();
            serial->close();
            serial->setPortName(portName);
            serial->open(QIODevice::ReadWrite);
            settings->setValue("iksu-2000/port", portName);  
        }
        ZN = sd.getZN();
        SKN = sd.getSKN();
        SROK = sd.getSROK();
        settings->setValue("iksu-2000/zn", ZN);
        settings->setValue("iksu-2000/skn", SKN);
        settings->setValue("iksu-2000/srok", SROK);
    }
}




void Iksu2000Plugin::sendCOM(int command)
{
    switch (command) {
    case 0:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(but0, 23);
                if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);
            if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 1:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(but1, 23);
               if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 2:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(but2, 23);
               if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 3:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(but3, 23);
             if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 4:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(but4, 23);
                if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 5:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(but5, 23);
                if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 6:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(but6, 23);
               if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 7:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(but7, 23);
                if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 8:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(but8, 23);
                if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 9:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(but9, 23);
                if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 11:
        if (serial->isOpen()) {
                serial->write(reset, 23);
                if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 12:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(enter, 23);
                if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);
            if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 13:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(down, 23);
                if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 14:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(up, 23);
                if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    case 15:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                serial->write(minus, 23);
                if (serial->waitForBytesWritten(1500)) Sleep(800);
            }
            serial->write(exec1, 22);if (serial->waitForBytesWritten(1500)) Sleep(800);
        }
        break;
    default:
        break;
    }
    prev_command=command;
}

void Iksu2000Plugin::on_serrial_error(QSerialPort::SerialPortError error)
{
    if (error) emit this->log(serial->errorString(), Qt::red);
    qDebug()<<"IKSU2000: "+QString::number(error);

}

void Iksu2000Plugin::on_close()
{
    qDebug()<<"Закрытие порта";
    emit this->log("Закрытие порта");
}

QString Iksu2000Plugin::getType()
{
    return "ИКСУ-2000";
}

QString Iksu2000Plugin::getSN()
{
    return ZN;
}

QString Iksu2000Plugin::getSKN()
{
    return SKN;
}

QString Iksu2000Plugin::getSROK()
{
    return SROK.toString("dd.MM.yyyy");
}

