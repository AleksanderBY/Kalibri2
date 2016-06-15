#include "reportdialog.h"
#include "ui_reportdialog.h"
#include "mustache.h"

reportdialog::reportdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::reportdialog)
{
    ui->setupUi(this);
    QStringList tagList;
    tagList<<"html"<<"body"<<"div"<<"table"<<"th"<<"t+d"<<"qwerty"<<"asdfg"<<"zxcv"<<"vcxz"<<"fdsa"<<"ytrewq";
    tagList.push_back("<trace>");
    completer = new QCompleter(tagList);
    ui->textEdit->setCompleter(completer);
}

reportdialog::~reportdialog()
{
    delete ui;
}

void reportdialog::on_textEdit_textChanged()
{
    ui->textEdit_2->setHtml(ui->textEdit->toPlainText());
}
