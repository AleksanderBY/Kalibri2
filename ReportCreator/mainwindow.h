#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QDir *getTemplateDir() const;
    void setTemplateDir(QDir *value);

private slots:
    void on_textEdit_textChanged();

    void on_save_triggered();

    void on_open_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QString fileName;
    QDir * templateDir;
};

#endif // MAINWINDOW_H
