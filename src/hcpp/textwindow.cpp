/**
 * SameGame
 * Copyright 2014 Denis Meyer
 * https://sites.google.com/site/calltopowersoftware/software
 */

#include "textwindow.h"
#include "ui_textwindow.h"

TextWindow::TextWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextWindow)
{
    init();
}

void TextWindow::setTitle(QString title) {
    if(title != 0) {
        this->setWindowTitle(title);
    }
}

void TextWindow::setText(QString textFile) {
    if(textFile != 0) {
        QFile file(textFile);
        if(file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            try
            {
                ui->textEdit->setText(file.readAll());
            } catch(...)
            {
            }
        }
    }
}

void TextWindow::init() {
    ui->setupUi(this);
    this->setWindowTitle("TextWindow");
    ui->textEdit->clear();
    this->setModal(true);
}

// TODO
void TextWindow::changeSize(int width, int height) {
    if((height > 0) && (width > 0)) {
        this->setMinimumSize(width, height);
        this->setMaximumSize(width, height);
        // TODO
        this->setMinimumSize(0, 0);
        this->setMaximumSize(16777215, 16777215);
    }
}

void TextWindow::setDefaultSize() {
    changeSize(590, 450);
}

TextWindow::~TextWindow()
{
    delete ui;
}

void TextWindow::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TextWindow::on_pushButton_clicked()
{
    this->reject();
}
