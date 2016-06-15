#ifndef REPORTCREATORINTERFACE
#define REPORTCREATORINTERFACE

#include <QObject>


class ReportCreatorInterface: public QObject
{
public:
    virtual void initialization() = 0;      //инициализация построителя отчетов
    virtual void getReportEditor() = 0;     //Вызов редактора шаблоов
    //virtual bool setContext(QVariantHash context) = 0;

};

QT_BEGIN_NAMESPACE

#define ReportCreatorInterface_iid "by.alexpozniak.kalibri2.reportcreatorinterface"

Q_DECLARE_INTERFACE(ReportCreatorInterface, ReportCreatorInterface_iid)
QT_END_NAMESPACE



#endif // REPORTCREATORINTERFACE

