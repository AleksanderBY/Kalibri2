#ifndef ALOGGERMODEL_H
#define ALOGGERMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QTime>
#include <QString>



class ALog{
public:
    QTime time;
    QString message;
    Qt::GlobalColor category;
};

class ALoggerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ALoggerModel(QObject *parent = 0);
    ~ALoggerModel();
    void log(QString message, Qt::GlobalColor category = Qt::white);
    void clear();



signals:

public slots:
    void on_log(QString message, Qt::GlobalColor category = Qt::white);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    QList<ALog> * list;
};

#endif // ALOGGERMODEL_H
