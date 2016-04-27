#ifndef AXMLCALIBRATIONMODEL_H
#define AXMLCALIBRATIONMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "calibrationdata.h"


//---------------------------------------------------------------------------------------------------------------
//
//Класс предназначен для:
//              -парсинга XML-файла хранящего информацию о каналах контроллеров и результатам калибровки
//              -создания структуры для доступа к данным из модели
//              -сохранения результатов
//
//----------------------------------------------------------------------------------------------------------------
class ADomCalibration : public QObject {
    Q_OBJECT
public:
    explicit ADomCalibration(QObject *parent = 0);
    ADomCalibration(QString fileName, QObject *parent = 0);
    ~ADomCalibration();
    void setFileName(QString fileName);
    bool open();
    bool open(QString fileName);
    bool isOpen() {return attrOpen;}
    void clear();                                                                   //очистка данных
    bool save(QString fileName);

    //----------------------------------------------------------------
    QHash<QString, QString> getSettings() {return settings;}
    QString getDriverFile() {return settings.value("driver");}
    int columnCount() {return column;}                                              //Возвращает число колонок в таблице
    QString getSetting(QString name) {return settings.value(name);}
    void addSetting(QString key, QString value) {settings.insert(key, value);}
    void setCurrentChannel(QModelIndex index) {currentChannel=index.row();}
    int getCurrentChannel() {return currentChannel;}
    //----------------------------------------------------------------
    bool isDomValid() {return attrOpen;}

    //----------------------------------------------------------------
    QHash<QString, QString> settings;                                //Установки базы
   //----------------------------------------------------------------

    //------------------------------------------------------------------
    //          тесты по структуре данных
    //------------------------------------------------------------------
    QVector<TChannelCalibration> tChannelList;

signals:
    reset();
    log(QString message, Qt::GlobalColor category = Qt::white);
private:
    QFile * device;                                                //Файл данных
    QXmlStreamReader * reader;
    QXmlStreamWriter * writer;
    int column;
    bool attrOpen;
    bool attrParser;
    //QStringList *fields;                                          //Названия полей в базе
    QStringRef version, encoding;                                   //Версия и кодировка файла
    int currentChannel;                                             //Текущий канал калибровки
    bool parsing();                                                 //Функция парсинга файла


};


//-------------------------------------------------------------------------------------------------------------------
//
//                 Модель даступа к данным в файле XML через класс ADomCalibration(каналы калибровки)
//
//-------------------------------------------------------------------------------------------------------------------
class AXMLCalibrationModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    AXMLCalibrationModel(ADomCalibration * dom, int level = 0, QObject *parent = 0);
    ADomCalibration * getDom() {return dom;}
    void setLevel(int level) {modelLevel=level;}
    //void setCurrentChannel(int a) {currentChannel=a;}
    //int getCurrentChannel() {return currentChannel;}
private:
    ADomCalibration *dom;
    int modelLevel;
    int currentChannel;
signals:
    log(QString message, Qt::GlobalColor category = Qt::white);

public slots:
    void change_current_channel(QModelIndex index);
    void model_reset();




    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool removeRows(int row, int count, const QModelIndex &parent);
};

//-------------------------------------------------------------------------------------------------------------------
//
//                 Модель даступа к данным в файле XML через класс ADomCalibration(результаты калибровки)
//
//-------------------------------------------------------------------------------------------------------------------
class AXMLCalibrationResultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    AXMLCalibrationResultModel(ADomCalibration * dom, QObject *parent = 0);
    ADomCalibration * getDom() {return dom;}
private:
    ADomCalibration *dom;
signals:
    log(QString message, Qt::GlobalColor category = Qt::white);

public slots:
    void change_current_channel(QModelIndex index, QModelIndex prev);
    void model_reset();



    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    //bool insertRows(int row, int count, const QModelIndex &parent);
    //bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    //bool removeRows(int row, int count, const QModelIndex &parent);

    // QAbstractItemModel interface
public:
    bool insertRows(int row, int count, const QModelIndex &parent);
};

class AXMLCalibrationListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    AXMLCalibrationListModel(ADomCalibration * dom, QObject *parent = 0);
    ADomCalibration * getDom() {return dom;}
private:
    ADomCalibration *dom;

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // QAbstractItemModel interface
public:
    Qt::ItemFlags flags(const QModelIndex &index) const;

    // QAbstractItemModel interface
public:
    int columnCount(const QModelIndex &parent) const;
};

#endif // AXMLCALIBRATIONMODEL_H
