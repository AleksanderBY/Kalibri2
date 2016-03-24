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

public slots:
    void accept();

private:
    Ui::SettingsDialog *ui;
    QSettings *settings;

    QString position;
    QString FIO;

    void loadSettings();
    void saveSettings();
};

#endif // SETTINGSDIALOG_H
