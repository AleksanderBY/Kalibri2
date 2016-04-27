#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>

namespace Ui {
class reportdialog;
}

class reportdialog : public QDialog
{
    Q_OBJECT

public:
    explicit reportdialog(QWidget *parent = 0);
    ~reportdialog();

private:
    Ui::reportdialog *ui;
};

#endif // REPORTDIALOG_H
