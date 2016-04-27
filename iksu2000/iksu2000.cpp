#include "iksu2000.h"
#include <QtWidgets>


Iksu2000Plugin::Iksu2000Plugin()
{
    serial = new  QSerialPort();
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(on_serrial_error(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(aboutToClose()), this, SLOT(on_close()));
    connect(serial, SIGNAL(readyRead()),this, SLOT(sl_readyRead()));
    connect(this, SIGNAL(send_next_commond()), this, SLOT(sl_send_next_command()));
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

    if (serial->open(QIODevice::ReadWrite)) {
        emit this->log("Порт "+portName+" успешно открыт!");serial->setBaudRate(QSerialPort::Baud9600);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setFlowControl(QSerialPort::NoFlowControl);
//        serial->setDataTerminalReady(true);
//        serial->setRequestToSend(false);
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
    serial->write(test1, 12);serial->flush();
    if (serial->waitForBytesWritten(1500)) Sleep(800);
}

bool Iksu2000Plugin::setValue(float value, measurement type_value)
{
    //Очищаем буфер команд
    sendArray.clear();
    current_send=0;
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
    //задаем паузу
    _delay=150;
    //Проверяем тип измерения
    switch (type_value) {
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //Если необхобимо задать милиамперы:
    case mA: //qDebug()<<value<<" mA";
        if (value<0||value>20) { error_val=1; break; }
        l1=2;l2=3;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //Если необхобимо задать миливольты:
    case mV: //qDebug()<<value<<" mV";
        if (value>100||value<-11) { error_val=1; break; }
        l1=3;l2=2;
        if (last_mode == type_value) break;
        sendCOM(11);sendCOM(12);sendCOM(13);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //Если необхобимо задать вольты:
    case V: //qDebug()<<value<<" V";
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
        _delay=300;
        sendCOM(11);sendCOM(12);sendCOM(13);sendCOM(13);sendCOM(13);sendCOM(12);
        while (Ctype!=0){
            sendCOM(13);
            Ctype+=1;
            if (Ctype>5) Ctype=0;
        }
        sendCOM(12);
        //термокомпенсация ручная 20 С
        sendCOM(14);sendCOM(2);sendCOM(1);sendCOM(0);sendCOM(12);
        break;

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термопара  тип K(IEC):
    case CTypeK:
        if (value>1300||value<-200) { error_val=1; break; }
        l1=4;l2=1;
        if (last_mode == type_value) break;
        _delay=300;
        sendCOM(11);sendCOM(12);sendCOM(13);;sendCOM(13);sendCOM(13);sendCOM(12);
        while (Ctype!=1){
            sendCOM(13);
            Ctype+=1;
            if (Ctype>5) Ctype=0;
        }
        sendCOM(12);
        //термокомпенсация ручная 20 С
        sendCOM(14);sendCOM(2);sendCOM(1);sendCOM(0);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термопара  тип B(IEC):
    case CTypeB:
        if (value>1800||value<300) { error_val=1; break; }
        l1=4;l2=1;   
        if (last_mode == type_value) break;
        _delay=300;
        sendCOM(11);sendCOM(12);sendCOM(13);;sendCOM(13);sendCOM(13);sendCOM(12);
        while (Ctype!=2){
            sendCOM(13);
            Ctype+=1;
            if (Ctype>5) Ctype=0;
        }
        sendCOM(12);
        //термокомпенсация ручная 20 С
        sendCOM(14);sendCOM(2);sendCOM(1);sendCOM(0);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термопара  тип A1(IEC):
    case CTypeA1:
        if (value>1800||value<0) { error_val=1; break; }
        l1=4;l2=1;
        if (last_mode == type_value) break;
        _delay=300;
        sendCOM(11);sendCOM(12);sendCOM(13);;sendCOM(13);sendCOM(13);sendCOM(12);
        while (Ctype!=3){
            sendCOM(13);
            Ctype+=1;
            if (Ctype>5) Ctype=0;
        }
        sendCOM(12);
        //термокомпенсация ручная 20 С
        sendCOM(14);sendCOM(2);sendCOM(1);sendCOM(0);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термопара  тип S(IEC):
    case CTypeS:
        if (value>1300||value<300) { error_val=1; break; }
        l1=4;l2=1;      
        if (last_mode == type_value) break;
        _delay=300;
        sendCOM(11);sendCOM(12);sendCOM(13);sendCOM(13);sendCOM(13);sendCOM(12);
        while (Ctype!=4){
            sendCOM(13);
            Ctype+=1;
            if (Ctype>5) Ctype=0;
        }
        sendCOM(12);
        //термокомпенсация ручная 20 С
        sendCOM(14);sendCOM(2);sendCOM(1);sendCOM(0);sendCOM(12);
        break;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //   Термопара  тип ХК (Рус):
    case CTypeXK:
        if (value>600||value<-200) { error_val=1; break; }
        l1=4;l2=1;
        if (last_mode == type_value) break;
        _delay=300;
        sendCOM(11);sendCOM(12);sendCOM(13);;sendCOM(13);sendCOM(13);sendCOM(12);
        while (Ctype!=5){
            sendCOM(13);
            Ctype+=1;
            if (Ctype>5) Ctype=0;
        }
        sendCOM(12);
        //термокомпенсация ручная 20 С
        sendCOM(14);sendCOM(2);sendCOM(1);sendCOM(0);sendCOM(12);
        break;
    default:
        error_val = 2;
        break;
    }

    if (error_val) {emit this->set_value_error();return false;}

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //qDebug()<<"Унифицированный вывод:";
    str_val = str_val=str_list.at(0).rightJustified(l1,'0') + str_list.at(1).leftJustified(l2, '0').left(l2);
    //qDebug()<<str_val;
    //Если число отрицательное
    if (minus) sendCOM(15);
    //Задаем значение на ИКСУ-2000 по каждому из полученных символов
    foreach (QChar s, str_val) {
        qDebug()<<s;
        sendCOM(QString(s).toInt());
    }
    //Включаем эмуляцию и посылаем сигнал об успешном завершении
    sendCOM(12);

    last_mode = type_value;
    //qDebug()<<sendArray.count();
    //начинаем отправку команд
    emit this->send_next_commond();
    return true;
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
    return true;
}




void Iksu2000Plugin::sendCOM(int command)
{
    switch (command) {
    case 0:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                 sendArray.push_back(10);
            }
             sendArray.push_back(17);
        }
        break;
    case 1:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                 sendArray.push_back(1);
            }
            sendArray.push_back(17);
        }
        break;
    case 2:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                 sendArray.push_back(2);
            }
             sendArray.push_back(17);
        }
        break;
    case 3:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                sendArray.push_back(3);
            }
             sendArray.push_back(17);
        }
        break;
    case 4:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                 sendArray.push_back(4);
            }
             sendArray.push_back(17);
        }
        break;
    case 5:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                sendArray.push_back(5);
            }
             sendArray.push_back(17);
        }
        break;
    case 6:
        if (serial->isOpen()) {
            if (prev_command!=command) {
               sendArray.push_back(6);
            }
            sendArray.push_back(17);
        }
        break;
    case 7:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                sendArray.push_back(7);
            }
            sendArray.push_back(17);
        }
        break;
    case 8:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                sendArray.push_back(8);
            }
            sendArray.push_back(17);
        }
        break;
    case 9:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                sendArray.push_back(9);
            }
            sendArray.push_back(17);
        }
        break;
    case 11:
        if (serial->isOpen()) {
               sendArray.push_back(11);
        }
        break;
    case 12:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                sendArray.push_back(12);
            }
            sendArray.push_back(17);
        }
        break;
    case 13:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                sendArray.push_back(13);
            }
            sendArray.push_back(17);
        }
        break;
    case 14:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                sendArray.push_back(14);
            }
            sendArray.push_back(17);
        }
        break;
    case 15:
        if (serial->isOpen()) {
            if (prev_command!=command) {
                sendArray.push_back(16);
            }
            sendArray.push_back(17);
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

void Iksu2000Plugin::sl_readyRead()
{
    QByteArray response;
    response = serial->readAll();
    responseArray.push_back(response);
    if (response.at(response.count()-1)==0x0D) {
        response.clear();
        Sleep(_delay);
        emit this->send_next_commond();
    }

}

//Отправка очередной команды на прибор
void Iksu2000Plugin::sl_send_next_command()
{
    if (!serial->isOpen()) {
        return;
    }
    if (current_send>sendArray.count()-1) {
        emit this->set_value_ok();
        return;
    }
    byte command = sendArray.at(current_send);
    switch (command) {
        case 10:serial->write(but0, 23);serial->waitForBytesWritten(1500);break;
        case 1: serial->write(but1, 23);serial->waitForBytesWritten(1500);break;
        case 2: serial->write(but2, 23);serial->waitForBytesWritten(1500);break;
        case 3: serial->write(but3, 23);serial->waitForBytesWritten(1500);break;
        case 4: serial->write(but4, 23);serial->waitForBytesWritten(1500);break;
        case 5: serial->write(but5, 23);serial->waitForBytesWritten(1500);break;
        case 6: serial->write(but6, 23);serial->waitForBytesWritten(1500);break;
        case 7: serial->write(but7, 23);serial->waitForBytesWritten(1500);break;
        case 8: serial->write(but8, 23);serial->waitForBytesWritten(1500);break;
        case 9: serial->write(but9, 23);serial->waitForBytesWritten(1500);break;
        case 11:serial->write(reset, 23);serial->waitForBytesWritten(1500);break;
        case 12:serial->write(enter, 23);serial->waitForBytesWritten(1500);break;
        case 13:serial->write(down, 23);serial->waitForBytesWritten(1500);break;
        case 14:serial->write(up, 23);serial->waitForBytesWritten(1500);break;
        case 15:serial->write(minus, 23);serial->waitForBytesWritten(1500);break;
        case 17:serial->write(exec1, 22);serial->waitForBytesWritten(1500);break;
        default:
            break;
        }
    current_send++;
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

//Возвращаем основную погрешность эталона для заданого режима измерения
double Iksu2000Plugin::getIndeterminacyGeneral(float value, measurement type_value)
{
    switch (type_value) {
    case mA:
        return 0.001/sqrt(3);
        break;
    case mV:
        return 0.003/sqrt(3);
        break;
    case V:
        return 0.003/sqrt(3);
        break;
    case Om: if (value>=0&&value<180) return 0.015/sqrt(3);
        if (value>=180&&value<=320) return 0.025/sqrt(3);
        break;
    case C100M1_426:
    case C100M1_428:
        return 0.05/sqrt(3);
        break;
    case C50M1_426:
    case C50M1_428:
    case C50P:
        return 0.08/sqrt(3);
        break;
    case CPt100IEC385:
    case C100P: if (value>=-200&&value<200) return 0.03/sqrt(3);
        if (value>=200&&value<=600) return 0.05/sqrt(3);
        break;
    case CTypeJ:
    case CTypeK:
    case CTypeXK:
        return 0.3/sqrt(3);
        break;
    case CTypeB:
        return 2/sqrt(3);
        break;
    case CTypeS:
        return 1/sqrt(3);
        break;
    case CTypeA1: if (value>=0&&value<1200) return 2/sqrt(3);
        if (value>=1200&&value<=2500) return 2.5/sqrt(3);
        break;
    default:
        break;
    }
    return 0;
}

double Iksu2000Plugin::getIndeterminacySecondary(float value, measurement type_value, QHash<QString, QString> conditions)
{
    float temperature;
    //проверяем наличие температуры в списке
    if (conditions.contains("temperature")) {
        bool ok;
        temperature = conditions.value("temperature").toFloat(&ok);
        if (ok) {
            if (temperature>=15&&temperature<=25) return 0;
            else return this->getIndeterminacyGeneral(value, type_value);
        }
    }
    return 0;
}
//Возвращаем подходящий тип задатчика
measurement Iksu2000Plugin::getMeasurenentType(QList<measurement> list)
{
    for (int i=0; i<list.count();i++) {
        if (list.at(i)==mA||list.at(i)==mV||list.at(i)==V||list.at(i)==Om||list.at(i)==C100M1_426||list.at(i)==C100M1_428||list.at(i)==C50M1_426
                    ||list.at(i)==C50M1_428||list.at(i)==C50P||list.at(i)==C100P||list.at(i)==CPt100IEC385||list.at(i)==CTypeJ
                    ||list.at(i)==CTypeK||list.at(i)==CTypeB||list.at(i)==CTypeA1||list.at(i)==CTypeS||list.at(i)==CTypeXK) return list.at(i);
    }
    return notSupport;
}

bool Iksu2000Plugin::checkConditions(QHash<QString, QString> conditions)
{
    //Проверяем условия окружаующей среды на соответствие условиям эксплуатации прибора
    //температура от 15 до 30
    bool ok;
    //Температура
    if (conditions.contains("temperature")) {
        float temperature = conditions.value("temperature").toFloat(&ok);
        if (!ok) {
            emit this->log("ИКСУ-2000: Неверное содержание поля \"температура\"", Qt::yellow);
            return false;
        }
        if (temperature>40||temperature<5) {
            emit this->log("ИКСУ-2000: Температура выходит за пределы допустимой", Qt::yellow);
            return false;
        }
    }
    return true;
}

double Iksu2000Plugin::getErrorGeneral(float value, measurement type_value)
{
    return 0;
}

double Iksu2000Plugin::getErrorSecondary(float value, measurement type_value, QHash<QString, QString> conditions)
{
    return 0;
}

