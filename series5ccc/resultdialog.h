#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>
#include <QQueue>
#include <QAbstractListModel>

namespace Ui {
class ResultDialog;
}

class SListModel : public QAbstractListModel
{
    Q_OBJECT

    // QAbstractItemModel interface
public:
    SListModel(QQueue<double> * list);
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void dataChange();
private:
    QQueue<double> * list;
};

class ResultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResultDialog(QWidget *parent = 0);
    ~ResultDialog();
   double getResult() { return list->dequeue(); }
   void clear() { list->clear();}
   int count() { list->count();}

private slots:
   void on_pushButton_clicked();

private:
    Ui::ResultDialog *ui;
    QQueue<double> * list;
    SListModel * SLM;
};

#endif // RESULTDIALOG_H
