#include "setupdialog.h"
#include "ui_setupdialog.h"

setupDialog::setupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setupDialog)
{
    ui->setupUi(this);
}

setupDialog::~setupDialog()
{
    delete ui;
}

void setupDialog::setZN(QString ZN)
{
    ui->ZN->setText(ZN);
}

QString setupDialog::getZN()
{
    return ui->ZN->text();
}

void setupDialog::setSKN(QString SKN)
{
    ui->SKN->setText(SKN);
}

QString setupDialog::getSKN()
{
    return ui->SKN->text();
}

void setupDialog::setSROK(QDate SROK)
{
    ui->SROK->setDate(SROK);
}

QDate setupDialog::getSROK()
{
    return ui->SROK->date();
}
