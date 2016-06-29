#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>

SettingsDialog::SettingsDialog(QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->settings = settings;
    this->loadSettings();

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

QString SettingsDialog::getData()
{

}

QString SettingsDialog::getPosition()
{
    return this->position;
}

QString SettingsDialog::getFIO()
{
    return this->FIO;
}

void SettingsDialog::accept()
{
    this->saveSettings();
    this->close();
}
QString SettingsDialog::getCPADriverDir() const
{
    return CPADriverDir;
}

void SettingsDialog::setCPADriverDir(const QString &value)
{
    CPADriverDir = value;
}


void SettingsDialog::loadSettings()
{
    position = settings->value("calibrator/position").toString();
    ui->position->setText(position);
    FIO = settings->value("calibrator/FIO").toString();
    ui->FIO->setText(FIO);
    CPADriverDir = settings->value("kalibri2/CPADriverDir").toString();
    ui->CPADriverDir->setText(CPADriverDir);
}

void SettingsDialog::saveSettings()
{
    this->position = ui->position->text();
    settings->setValue("calibrator/position", this->position);
    this->FIO = ui->FIO->text();
    settings->setValue("calibrator/FIO", this->FIO);
    this->CPADriverDir = ui->CPADriverDir->text();
    settings->setValue("kalibri2/CPADriverDir", CPADriverDir);
}

void SettingsDialog::on_pushButton_clicked()
{
    QString str = QFileDialog::getExistingDirectory();
    if (str!="") ui->CPADriverDir->setText(str);
}
