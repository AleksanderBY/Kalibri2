#ifndef REPORTCREATOR_H
#define REPORTCREATOR_H

#include "reportcreatorinterface.h"
#include "mainwindow.h"
#include <QCompleter>

class ReportCreatorPlugin : public ReportCreatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "by.alexpozniak.kalibri2.reportcreatorinterface" FILE "reportcreatorplugin.json")
    Q_INTERFACES(ReportCreatorInterface)
public:
    ReportCreatorPlugin();
    void initialization() Q_DECL_OVERRIDE;
    void getReportEditor() Q_DECL_OVERRIDE;
protected:
    MainWindow mw;
private:
    QCompleter * completer;             //объект автоподстановки
};

#endif // REPORTCREATOR_H
