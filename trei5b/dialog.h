#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDataWidgetMapper>
#include <QSqlTableModel>
#include <QDoubleValidator>
#include <QRegExpValidator>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    void setModel(QAbstractItemModel * model);
    void setRow(int row);
    void setSupportType(QStringList typeList);
    bool hasAcceptableForm();
    ~Dialog();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_extChanged (const QString &arg1);

    void on_controller_textChanged(const QString &arg1);

    void on_module_textChanged(const QString &arg1);

    void on_pipe_textChanged(const QString &arg1);

    void on_cipher_textChanged(const QString &arg1);

    void on_name_textChanged(const QString &arg1);

    void on_unit_textChanged(const QString &arg1);

    void on_adress_textChanged(const QString &arg1);

    void on_point1_textChanged(const QString &arg1);

    void start();

    void on_point2_textChanged(const QString &arg1);

    void on_point3_textEdited(const QString &arg1);

    void on_type_editTextChanged(const QString &arg1);

private:
    Ui::Dialog *ui;
    QDataWidgetMapper * dwm;
    QDoubleValidator * minValidator, *maxValidator, *pointsValidator;
    QRegExpValidator * controllerValidator,
                        *moduleValidator,
                        *pipeValidator,
                        *nameValidator,
                        *cipherValidator,
                        *unitValidator,
                        *adressValidator;
    QString oldPoint1,oldPoint2,oldPoint3, oldPoint4, oldPoint5;
};

#endif // DIALOG_H
