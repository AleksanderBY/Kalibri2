#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->min, SIGNAL(textChanged(QString)), this, SLOT(on_extChanged(QString)));
    connect(ui->max, SIGNAL(textChanged(QString)), this, SLOT(on_extChanged(QString)));
    dwm = new QDataWidgetMapper();
    dwm->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    connect(dwm, SIGNAL(currentIndexChanged(int)),this, SLOT(start()));
    //Создаем валидаторы для полей
    minValidator = new QDoubleValidator(-10000, 10000, 4);
    ui->min->setValidator(minValidator);
    maxValidator = new QDoubleValidator(-10000, 10000, 4);
    ui->max->setValidator(maxValidator);
    pointsValidator = new QDoubleValidator(-10000, 10000, 4);
    ui->point1->setValidator(pointsValidator);
    ui->point2->setValidator(pointsValidator);
    ui->point3->setValidator(pointsValidator);
    ui->point4->setValidator(pointsValidator);
    QRegExp controllerRegExp("^\\S{1,20}");
    controllerValidator = new QRegExpValidator(controllerRegExp);
    ui->controller->setValidator(controllerValidator);
    QRegExp moduleRegExp("^\\S{1,10}");
    moduleValidator = new QRegExpValidator(moduleRegExp);
    ui->module->setValidator(moduleValidator);
    QRegExp pipeRegExp("^\\S{1,10}");
    pipeValidator = new QRegExpValidator(pipeRegExp);
    ui->pipe->setValidator(pipeValidator);
    QRegExp cipherRegExp("^\\S{3,10}");
    cipherValidator = new QRegExpValidator(cipherRegExp);
    ui->cipher->setValidator(cipherValidator);
    QRegExp nameRegExp("^\\S.{3,59}");
    nameValidator = new QRegExpValidator(nameRegExp);
    ui->name->setValidator(nameValidator);
    QRegExp unitRegExp("^\\S.{,9}");
    unitValidator = new QRegExpValidator(unitRegExp);
    ui->unit->setValidator(unitValidator);
    QRegExp adressRegExp("[A-F0-9]{1,4}");
    adressValidator = new QRegExpValidator(adressRegExp);
    ui->adress->setValidator(adressValidator);
    //hasAcceptableForm();
}

void Dialog::setModel(QAbstractItemModel *model)
{
    dwm->setModel(model);
    QModelIndex index = model->index(0,2);
    qDebug()<<model->data(index).toString();
    dwm->addMapping(ui->controller, 0);
    dwm->addMapping(ui->module,1);
    dwm->addMapping(ui->pipe, 2);
    dwm->addMapping(ui->cipher, 3);
    dwm->addMapping(ui->name, 4);
    dwm->addMapping(ui->type, 5);
    dwm->addMapping(ui->min, 6);
    dwm->addMapping(ui->max, 7);
    dwm->addMapping(ui->unit, 8);
    dwm->addMapping(ui->adress, 9);
    dwm->addMapping(ui->point1, 10);
    dwm->addMapping(ui->point2, 11);
    dwm->addMapping(ui->point3, 12);
    dwm->addMapping(ui->point4, 13);
    dwm->setCurrentIndex(0);
}

void Dialog::setRow(int row)
{
    dwm->setCurrentIndex(row);
}

void Dialog::setSupportType(QStringList typeList)
{
    ui->type->clear();
    ui->type->addItem("");
    ui->type->addItems(typeList);
}

bool Dialog::hasAcceptableForm()
{
    if (ui->name->hasAcceptableInput() && ui->module->hasAcceptableInput() &&
            ui->pipe->hasAcceptableInput() && ui->controller->hasAcceptableInput() &&
            ui->min->hasAcceptableInput() && ui->max->hasAcceptableInput() &&
            ui->unit->hasAcceptableInput() && ui->adress->hasAcceptableInput()) return true;
    else return false;
}


Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    this->reject();
}

void Dialog::on_pushButton_2_clicked()
{
    if (!hasAcceptableForm()) return;
    dwm->submit();
    this->accept();
}

void Dialog::on_controller_textChanged(const QString &arg1)
{
    if (!ui->controller->hasAcceptableInput())
        ui->controller->setStyleSheet("background-color: yellow");
    else ui->controller->setStyleSheet("background-color: white");
}

void Dialog::on_module_textChanged(const QString &arg1)
{
    if (!ui->module->hasAcceptableInput())
        ui->module->setStyleSheet("background-color: yellow");
    else ui->module->setStyleSheet("background-color: white");
}

void Dialog::on_pipe_textChanged(const QString &arg1)
{
    if (!ui->pipe->hasAcceptableInput())
        ui->pipe->setStyleSheet("background-color: yellow");
    else ui->pipe->setStyleSheet("background-color: white");

}

void Dialog::on_cipher_textChanged(const QString &arg1)
{
    if (!ui->cipher->hasAcceptableInput())
        ui->cipher->setStyleSheet("background-color: yellow");
    else ui->cipher->setStyleSheet("background-color: white");
}

void Dialog::on_name_textChanged(const QString &arg1)
{
    if (!ui->name->hasAcceptableInput())
        ui->name->setStyleSheet("background-color: yellow");
    else ui->name->setStyleSheet("background-color: white");
}

void Dialog::on_unit_textChanged(const QString &arg1)
{
    if (!ui->unit->hasAcceptableInput())
        ui->unit->setStyleSheet("background-color: yellow");
    else ui->unit->setStyleSheet("background-color: white");
}

void Dialog::on_adress_textChanged(const QString &arg1)
{
    if (!ui->adress->hasAcceptableInput())
            ui->adress->setStyleSheet("background-color: yellow");
    else ui->adress->setStyleSheet("background-color: white");
}

void Dialog::on_extChanged(const QString &arg1)
{
    bool minIsValid = ui->min->hasAcceptableInput();
    bool maxIsValid = ui->max->hasAcceptableInput();
    bool priznak=false;
    if (minIsValid && maxIsValid){
        priznak = ui->max->text().toDouble()>=ui->min->text().toDouble();
    }

    if (priznak) {
        ui->min->setStyleSheet("background-color: white");
        ui->max->setStyleSheet("background-color: white");
        pointsValidator->setRange(ui->min->text().toDouble(), ui->max->text().toDouble(),4);
        ui->point1->setEnabled(true);
        ui->point1->setText(oldPoint1);
    }
    else
    {
        ui->min->setStyleSheet("background-color: yellow");
        ui->max->setStyleSheet("background-color: yellow");
        pointsValidator->setRange(-10000, 10000, 4);
        ui->point1->setEnabled(false);
        oldPoint1=ui->point1->text();
        ui->point1->clear();
    }
    //hasAcceptableForm();
}


void Dialog::on_point1_textChanged(const QString &arg1)
{
    if (!ui->point1->hasAcceptableInput())
    {
        ui->point1->setStyleSheet("background-color: yellow");
        oldPoint2=ui->point2->text();
        ui->point2->setText("");
        ui->point2->setEnabled(false);
    }
    else {
        ui->point1->setStyleSheet("background-color: white");
        //ui->point2->setText(oldPoint2);
        ui->point2->setEnabled(true);
    }
}


void Dialog::start()
{
    emit ui->controller->textChanged("");
    emit ui->module->textChanged("");
    emit ui->pipe->textChanged("");
    emit ui->cipher->textChanged("");
    emit ui->name->textChanged("");
    emit ui->unit->textChanged("");
    emit ui->adress->textChanged("");
    emit ui->min->textChanged("");
    emit ui->max->textChanged("");
    emit ui->point1->textChanged("");
    emit ui->point2->textChanged("");
    emit ui->point3->textChanged("");
    emit ui->point4->textChanged("");
    oldPoint1=ui->point1->text().toDouble();
    oldPoint2=ui->point2->text().toDouble();
    oldPoint3=ui->point3->text().toDouble();
    oldPoint4=ui->point4->text().toDouble();



}

void Dialog::on_point2_textChanged(const QString &arg1)
{
    if (!ui->point2->hasAcceptableInput())
    {
        ui->point2->setStyleSheet("background-color: yellow");
        oldPoint3=ui->point3->text();
        ui->point3->setText("");
        ui->point3->setEnabled(false);
    }
    else {
        ui->point2->setStyleSheet("background-color: white");
        //ui->point2->setText(oldPoint2);
        ui->point3->setEnabled(true);
    }
}

void Dialog::on_point3_textEdited(const QString &arg1)
{
    if (!ui->point3->hasAcceptableInput())
    {
        ui->point3->setStyleSheet("background-color: yellow");
        oldPoint4=ui->point4->text();
        ui->point4->setText("");
        ui->point4->setEnabled(false);
    }
    else {
        ui->point3->setStyleSheet("background-color: white");
        //ui->point2->setText(oldPoint2);
        ui->point4->setEnabled(true);
    }
}

void Dialog::on_type_editTextChanged(const QString &arg1)
{

}
