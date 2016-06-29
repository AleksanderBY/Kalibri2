#include "reportcreator.h"
#include <QDebug>


ReportCreatorPlugin::ReportCreatorPlugin()
{
//    QStringList tagList;
//    tagList<<"html"<<"body"<<"div"<<"table"<<"th"<<"t+d"<<"qwerty"<<"asdfg"<<"zxcv"<<"vcxz"<<"fdsa"<<"ytrewq";
//    tagList.push_back("<trace>");
//    completer = new QCompleter(tagList);
//    mw.setCompleter(completer);
}

void ReportCreatorPlugin::initialization(QDir *templateDir)
{
    qDebug()<<"report creator";
    this->templateDir = templateDir;
    mw.setTemplateDir(templateDir);
    return;
}


void ReportCreatorPlugin::getReportEditor()
{
    mw.show();
}
QDir *ReportCreatorPlugin::getTemplateDir()
{
    return templateDir;
}

void ReportCreatorPlugin::setTemplateDir(QDir *dir)
{
    templateDir = dir;
    mw.setTemplateDir(dir);
}

