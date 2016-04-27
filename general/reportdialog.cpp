#include "reportdialog.h"
#include "ui_reportdialog.h"

reportdialog::reportdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::reportdialog)
{
    ui->setupUi(this);
}

reportdialog::~reportdialog()
{
    delete ui;
}
