#include "setupdialog.h"
#include "ui_setupdialog.h"

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);
}

SetupDialog::~SetupDialog()
{
    delete ui;
}

void SetupDialog::setTypeDevice(QString TypeDevice)
{
    ui->TypeDevice->setText(TypeDevice);
}

QString SetupDialog::getTypeDevice()
{
    return ui->TypeDevice->text();
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
