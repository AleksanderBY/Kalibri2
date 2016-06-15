#ifndef QTEXTEDITCOMPLETER_H
#define QTEXTEDITCOMPLETER_H

#include <QTextEdit>
#include <QCompleter>
#include <QStringListModel>
#include <QVariantHash>

class QTextEditCompleter : public QTextEdit
{
    Q_OBJECT
public:
    QTextEditCompleter(QWidget *parent = 0);
    ~QTextEditCompleter();
    void setCompleter(QCompleter * completer);
    QCompleter * completer() const;

protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private slots:
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;
    int insertMode;

private:
    QStringListModel * model;
    QVariantHash autoCompleteList;
    QString key;
    QStringList tagList;
    QStringList cssList;
    QCompleter * c;
    void setStringList(QString k);
};

#endif // QTEXTEDITCOMPLETER_H
