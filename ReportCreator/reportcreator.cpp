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

void ReportCreatorPlugin::initialization()
{
    qDebug()<<"report creator";
    return;
}


void ReportCreatorPlugin::getReportEditor()
{
    mw.show();
}
