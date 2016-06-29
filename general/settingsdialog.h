#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QString>
#include <QSettings>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QSettings *settings, QWidget *parent = 0);
    ~SettingsDialog();
    QString  getData();
    QString getPosition();
    QString getFIO();

    QString getCPADriverDir() const;
    void setCPADriverDir(const QString &value);

public slots:
    void accept();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SettingsDialog *ui;
    QSettings *settings;

    QString position;       //Должность калибровщика
    QString FIO;            //ФИО калибровщика
    QString CPADriverDir;   //Каталог хранения драйверов приборов

    void loadSettings();
    void saveSettings();
};

#endif // SETTINGSDIALOG_H
