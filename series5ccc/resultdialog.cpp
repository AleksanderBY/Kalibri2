#include "resultdialog.h"
#include "ui_resultdialog.h"

ResultDialog::ResultDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultDialog)
{
    ui->setupUi(this);
    this->list = new QQueue<double>();
    this->SLM = new SListModel(list);
    ui->listView->setModel(SLM);
}

ResultDialog::~ResultDialog()
{
    delete ui;
}


SListModel::SListModel(QQueue<double> *list)
{
    this->list = list;
}

int SListModel::rowCount(const QModelIndex &parent) const
{
    this->list->count();
}

QVariant SListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    switch (role) {
    case Qt::DisplayRole: return list->at(index.row());
        break;
    default:
        return QVariant();
        break;
    }
}

void SListModel::dataChange()
{
    emit dataChanged(index(list->count()-1), index(list->count()-1));
}

//проверяем правильность поля
void ResultDialog::on_pushButton_clicked()
{
    bool ok;
    double val = ui->lineEdit->text().toDouble(&ok);
    if (ok) list->enqueue(val);
    SLM->dataChange();
}
