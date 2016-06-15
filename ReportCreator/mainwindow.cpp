#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>
#include <QTextCursor>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::setCompleter(QCompleter *completer)
{
    ui->textEdit->setCompleter(completer);
}

void MainWindow::on_textEdit_textChanged()
{
    ui->textEdit_2->setHtml(ui->textEdit->toPlainText());
}

//сохраняем шаблон
void MainWindow::on_save_triggered()
{
    QFile file;
    if (fileName=="") fileName = QFileDialog::getSaveFileName(0, "Сохранить файл", "", "*.html");
    qDebug()<<fileName;
    file.setFileName(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    qDebug()<<"Файл открыт";
    QTextStream out(&file);
    out<<ui->textEdit->toPlainText();
    file.close();
}

void MainWindow::on_open_triggered()
{
    QFile file;
    QString fileNameTemp = QFileDialog::getOpenFileName(0, "Открытие файла", "", "*html");
    file.setFileName(fileNameTemp);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        return;
    }
    ui->textEdit->clear();
    fileName = fileNameTemp;
    QTextCursor tc = ui->textEdit->textCursor();

    QTextStream in(&file);
    tc.insertText(in.readAll());
}

void MainWindow::on_pushButton_clicked()
{
    ui->textEdit->setVisible(!ui->textEdit->isVisible());
}
