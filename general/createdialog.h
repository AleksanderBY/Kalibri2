#ifndef CREATEDIALOG_H
#define CREATEDIALOG_H

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class createDialog;
}

class createDialog : public QDialog
{
    Q_OBJECT

public:
    explicit createDialog(QWidget *parent = 0);
    ~createDialog();
    void setDefault(QStringList driversList);
    QString getNameDB();
    int getIndexDriver();

private slots:
    void on_toolButton_clicked();


    void on_buttonBox_accepted();

private:
    Ui::createDialog *ui;
};

#endif // CREATEDIALOG_H
