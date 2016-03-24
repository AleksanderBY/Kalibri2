#include "settingsdialog.h"
#include "ui_settingsdialog.h"

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

void SettingsDialog::loadSettings()
{
    position = settings->value("calibrator/position").toString();
    ui->position->setText(position);
    FIO = settings->value("calibrator/FIO").toString();
    ui->FIO->setText(FIO);
}

void SettingsDialog::saveSettings()
{
    this->position = ui->position->text();
    settings->setValue("calibrator/position", this->position);
    this->FIO = ui->FIO->text();
    settings->setValue("calibrator/FIO", this->FIO);
}
