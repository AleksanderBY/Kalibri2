#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>

namespace Ui {
class setupDialog;
}

class setupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit setupDialog(QWidget *parent = 0);
    ~setupDialog();
    void setZN(QString ZN);
    QString getZN();
    void setSKN(QString SKN);
    QString getSKN();
    void setSROK(QDate SROK);
    QDate getSROK();

private:
    Ui::setupDialog *ui;
};

#endif // SETUPDIALOG_H
