#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>

namespace Ui {
class SetupDialog;
}

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog(QWidget *parent = 0);
    ~SetupDialog();
    void setTypeDevice(QString TypeDevice);
    QString getTypeDevice();
    void setZN(QString ZN);
    QString getZN();
    void setSKN(QString SKN);
    QString getSKN();
    void setSROK(QDate SROK);
    QDate getSROK();

private:
    Ui::SetupDialog *ui;
};

#endif // SETUPDIALOG_H
