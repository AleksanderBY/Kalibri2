#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <QDate>

namespace Ui {
class SetupDialog;
}

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog(QWidget *parent = 0);
    ~SetupDialog();
    void setPort(QString port);     //установка текущего порта
    QString getPort();  //отправка выбранного порта
    void setZN(QString ZN);
    QString getZN();
    void setSKN(QString SKN);
    QString getSKN();
    void setSROK(QDate SROK);
    QDate getSROK();
    void setAutoMode(bool autoMode);
    bool getAutoMode();


private:
    Ui::SetupDialog *ui;
};

#endif // SETUPDIALOG_H
