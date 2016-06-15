#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>
#include <QCompleter>

namespace Ui {
class reportdialog;
}

class reportdialog : public QDialog
{
    Q_OBJECT

public:
    explicit reportdialog(QWidget *parent = 0);
    ~reportdialog();

private slots:
    void on_textEdit_textChanged();

private:
    Ui::reportdialog *ui;
    QCompleter *completer;
};

#endif // REPORTDIALOG_H
