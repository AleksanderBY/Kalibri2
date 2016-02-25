#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

//    serial = new  QSerialPort();
//    serial->setPortName("COM3");
//    serial->setBaudRate(QSerialPort::Baud9600);
//    serial->setStopBits(QSerialPort::OneStop);
//    serial->setDataBits(QSerialPort::Data8);
//    serial->setParity(QSerialPort::NoParity);
//    serial->setFlowControl(QSerialPort:: SoftwareControl);
//    connect(this->serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(on_error(QSerialPort::SerialPortError)));

//    serial_open = serial->open(QIODevice::WriteOnly);

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_but_enter_clicked()
{
    if (serial->isOpen()) {
        serial->write(enter,23); Sleep(800); serial->flush();  serial->write(exec1, 22);Sleep(800);
    }
}

void Dialog::on_but_reset_clicked()
{
    if (serial->isOpen()) {
        serial->write(reset, 23);
        Sleep(800);
    }
}

void Dialog::on_error(QSerialPort::SerialPortError error)
{
    qDebug()<<"error:" <<error;
}

void Dialog::on_but_up_clicked()
{
    if (serial->isOpen()) {
        serial->write(up, 23);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}

void Dialog::on_but_down_clicked()
{
    if (serial->isOpen()) {
        serial->write(down, 23);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}

void Dialog::on_but_ce_clicked()
{
    serial->write(exec1, 22);Sleep(800);
}

void Dialog::on_but_0_clicked()
{
    if (serial->isOpen()) {
        serial->write(but0, 23);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}

void Dialog::on_but_1_clicked()
{
    if (serial->isOpen()) {
        serial->write(but1, 23);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}

void Dialog::on_but_2_clicked()
{
    if (serial->isOpen()) {
        serial->write(but2, 23);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}

void Dialog::on_but_3_clicked()
{
    if (serial->isOpen()) {
        serial->write(but3, 23);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}

void Dialog::on_but_4_clicked()
{
    if (serial->isOpen()) {
        serial->write(but4, 23);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}

void Dialog::on_but_5_clicked()
{
    if (serial->isOpen()) {
        serial->write(but5, 22);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}

void Dialog::on_but_6_clicked()
{
    if (serial->isOpen()) {
        serial->write(but6, 23);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}

void Dialog::on_but_7_clicked()
{
    if (serial->isOpen()) {
        serial->write(but7, 23);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}

void Dialog::on_but_8_clicked()
{
    if (serial->isOpen()) {
        serial->write(but8, 23);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}

void Dialog::on_but_9_clicked()
{
    if (serial->isOpen()) {
        serial->write(but9, 23);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}

void Dialog::on_but_minus_clicked()
{
    if (serial->isOpen()) {
        serial->write(minus, 23);
        serial->flush();
        Sleep(800);
        serial->write(exec1, 22);serial->flush();Sleep(800);
    }
}
