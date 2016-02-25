#include "createdialog.h"
#include "ui_createdialog.h"

createDialog::createDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::createDialog)
{
    ui->setupUi(this);
}

createDialog::~createDialog()
{
    delete ui;
}

void createDialog::setDefault(QStringList driversList)
{
    ui->lineEdit->clear();
    ui->comboBox->clear();
    ui->comboBox->addItems(driversList);
}

QString createDialog::getNameDB()
{
    return ui->lineEdit->text();
}

int createDialog::getIndexDriver()
{
    ui->comboBox->currentText();
    return ui->comboBox->currentIndex();
}

void createDialog::on_toolButton_clicked()
{
    QString str = QFileDialog::getExistingDirectory();
    if (str!="") ui->lineEdit_2->setText(str);
}


void createDialog::on_buttonBox_accepted()
{
    if (ui->lineEdit->text()=="") ui->lineEdit->setStyleSheet("background-color: yellow");
}
