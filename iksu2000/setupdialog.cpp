#include "setupdialog.h"
#include "ui_setupdialog.h"
#include <QSerialPortInfo>
#include <QDebug>

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);
    QList<QSerialPortInfo> listSPI = QSerialPortInfo::availablePorts();
    ui->comboBox->clear();
    foreach (QSerialPortInfo info, listSPI) {
        ui->comboBox->addItem(info.portName());
    }

    //QSerialPortInfo spi;

}

SetupDialog::~SetupDialog()
{
    delete ui;
}

void SetupDialog::setPort(QString port)
{
    ui->comboBox->setCurrentIndex(ui->comboBox->findText(port));
}

QString SetupDialog::getPort()
{
    return ui->comboBox->currentText();
}



void SetupDialog::setZN(QString ZN)
{
    ui->ZN->setText(ZN);
}

QString SetupDialog::getZN()
{
    return ui->ZN->text();
}

void SetupDialog::setSKN(QString SKN)
{
    ui->SKN->setText(SKN);
}

QString SetupDialog::getSKN()
{
    return ui->SKN->text();
}

void SetupDialog::setSROK(QDate SROK)
{
    ui->SROK->setDate(SROK);
}

QDate SetupDialog::getSROK()
{
    return ui->SROK->date();
}
