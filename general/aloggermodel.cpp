#include "aloggermodel.h"
#include <QColor>

ALoggerModel::ALoggerModel(QObject *parent) : QAbstractListModel(parent)
{
    list = new QList<ALog>;
}

ALoggerModel::~ALoggerModel()
{

}

void ALoggerModel::log(QString message, Qt::GlobalColor category)
{
    ALog log;
    log.time = QTime::currentTime();
    log.message = message;
    log.category = category;
    list->push_front(log);
//    QModelIndex index;
//    index.=list->count()-1;
    emit dataChanged(index(list->count()-1), index(list->count()-1));
}

void ALoggerModel::clear()
{
    list->clear();
    emit dataChanged(index(-1), index(-1));
}

void ALoggerModel::on_log(QString message, Qt::GlobalColor category)
{
    log(message, category);
}


int ALoggerModel::rowCount(const QModelIndex &parent) const
{
    return list->count();
}

QVariant ALoggerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    switch (role) {
    case Qt::DisplayRole: return list->at(index.row()).time.toString()+" - "+list->at(index.row()).message; break;
    case Qt::BackgroundColorRole: return QColor(list->at(index.row()).category); break;
    default: return QVariant();
    }


}
