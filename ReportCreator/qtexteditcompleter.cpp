#include "qtexteditcompleter.h"
#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QtDebug>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QTextDocumentFragment>

QTextEditCompleter::QTextEditCompleter(QWidget *parent)
: QTextEdit(parent)
{
   insertMode = 0;
   key = "default";
   tagList<<"address"<<"b"<<"big"<<"blockquote"<<"body"<<"br"<<"center"<<"cite"<<"co-de"<<"dd"<<"dfn"
          <<"div"<<"dl"<<"dt"<<"em"<<"font"<<"h1"<<"h2"<<"h3"<<"h4"<<"h5"<<"h6"<<"head"<<"hr"<<"html"<<"i"
          <<"img"<<"kbd"<<"meta"<<"li"<<"nobr"<<"ol"<<"p"<<"pre"<<"qt"<<"s"<<"samp"<<"small"<<"span"<<"strong"
          <<"sub"<<"sup"<<"table"<<"tbody"<<"td"<<"tfoot"<<"th"<<"thead"<<"title"<<"tr"<<"tt"<<"u"<<"ul"<<"var";
   cssList<<"background-color"<<"background-image"<<"color"<<"font-size";
   autoCompleteList.insert("tag", tagList);
   autoCompleteList.insert("default", QStringList());
   autoCompleteList.insert("css", cssList);
   model = new QStringListModel(autoCompleteList.value(key).toStringList());
   c = new QCompleter(model);
   c->setWidget(this);
   c->setCompletionMode(QCompleter::PopupCompletion);
   c->setCaseSensitivity(Qt::CaseInsensitive);
   QObject::connect(c, SIGNAL(activated(QString)),
                    this, SLOT(insertCompletion(QString)));
}

QTextEditCompleter::~QTextEditCompleter()
{

}

//Устанавливаем автозаполнитель в редакторе
void QTextEditCompleter::setCompleter(QCompleter *completer)
{
    //Если подстановщик уже есть, то разрываем старые сигналы и слоты
    if (c)
        qDebug()<<"123";
        QObject::disconnect(c, 0, this, 0);

    c = completer;

    //если подстановщик недействителен выходим
    if (!c)
        return;

    //настраиваем подстановщик
    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(c, SIGNAL(activated(QString)),
                     this, SLOT(insertCompletion(QString)));
}

QCompleter *QTextEditCompleter::completer() const
{
    return c;
}

void QTextEditCompleter::keyPressEvent(QKeyEvent *e)
{
    if (c && !c->popup()->isVisible()) {
        switch (e->key()) {
        case Qt::Key_Less:
            insertMode=1;
            this->setStringList("tag");
            break;
        default:
            break;
        }
    }

    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) // do not process the shortcut when we have a completer
        QTextEdit::keyPressEvent(e);
//! [7]

//! [8]
    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 1
                      || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!
}

void QTextEditCompleter::focusInEvent(QFocusEvent *e)
{
    if (c)
        c->setWidget(this);
    QTextEdit::focusInEvent(e);
}

void QTextEditCompleter::insertCompletion(const QString &completion)
{
    if (c->widget() != this)
        return;
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::StartOfWord);
    tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);

    if (insertMode==1) {
        insertMode=2;
        tc.insertText(completion+" ");
        this->setTextCursor(tc);
        qDebug()<<"css";
        this->setStringList("css");
        return;
    }

    if (insertMode==2) {
        tc.insertText(completion+"=\"\"");
    }

//    int extra = completion.length() - c->completionPrefix().length();
//    tc.movePosition(QTextCursor::Left);
//    tc.movePosition(QTextCursor::EndOfWord);
//    tc.insertText(completion.right(extra));
//    tc.insertText(">");
//    tc.insertText("</"+completion+">");
//    tc.movePosition(QTextCursor::StartOfLine);
//    tc.movePosition(QTextCursor::PreviousCharacter);
//    setTextCursor(tc);
}

QString QTextEditCompleter::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    return tc.selectedText();
}

void QTextEditCompleter::setStringList(QString k)
{
    model->setStringList(autoCompleteList.value(k).toStringList());
}



void QTextEditCompleter::mousePressEvent(QMouseEvent *e)
{
    QTextEdit::mousePressEvent(e);
}
