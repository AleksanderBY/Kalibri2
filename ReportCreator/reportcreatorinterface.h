#ifndef REPORTCREATORINTERFACE
#define REPORTCREATORINTERFACE

#include <QObject>
#include <QDir>


class ReportCreatorInterface: public QObject
{
public:
    virtual void initialization(QDir * templateDir) = 0;      //инициализация построителя отчетов
    virtual void getReportEditor() = 0;     //Вызов редактора шаблоов
    virtual void setTemplateDir(QDir *dir) = 0; //Установка каталога шаблонов
    virtual QDir * getTemplateDir() = 0;        //Получение каталога шаблонов
    //virtual bool setContext(QVariantHash context) = 0;

};

QT_BEGIN_NAMESPACE

#define ReportCreatorInterface_iid "by.alexpozniak.kalibri2.reportcreatorinterface"

Q_DECLARE_INTERFACE(ReportCreatorInterface, ReportCreatorInterface_iid)
QT_END_NAMESPACE



#endif // REPORTCREATORINTERFACE

