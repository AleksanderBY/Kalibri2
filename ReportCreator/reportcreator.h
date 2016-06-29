#ifndef REPORTCREATOR_H
#define REPORTCREATOR_H

#include "reportcreatorinterface.h"
#include "mainwindow.h"
#include <QCompleter>
#include <QDir>

class ReportCreatorPlugin : public ReportCreatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "by.alexpozniak.kalibri2.reportcreatorinterface" FILE "reportcreatorplugin.json")
    Q_INTERFACES(ReportCreatorInterface)
public:
    ReportCreatorPlugin();
    void initialization(QDir * templateDir) Q_DECL_OVERRIDE;
    void getReportEditor() Q_DECL_OVERRIDE;
    void setTemplateDir(QDir *dir) Q_DECL_OVERRIDE; //Установка каталога шаблонов
    QDir * getTemplateDir() Q_DECL_OVERRIDE;        //Получение каталога шаблонов

protected:
    MainWindow mw;
private:
    QDir * templateDir;     //Каталог хранения шаблонов
};

#endif // REPORTCREATOR_H
