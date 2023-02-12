/**
 * SameGame
 * Copyright 2014 Denis Meyer
 * https://sites.google.com/site/calltopowersoftware/software
 */

#ifndef TEXTWINDOW_H
#define TEXTWINDOW_H

#include <QFile>
#include <QtWidgets>

namespace Ui {
class TextWindow;
}

class TextWindow : public QDialog {
    Q_OBJECT
public:
    TextWindow(QWidget *parent = 0);
    ~TextWindow();

    /**
     * Sets the Window Title
     * @param title Window Title
     */
    void setTitle(QString title);

    /**
     * Sets the Text
     * @param textFile File with the Text
     */
    void setText(QString textFile);

    /**
     * Changes the Window Size
     * @param width Width
     * @param height Height
     */
    void changeSize(int width, int height);

    /**
     * Sets the default Window Size
     */
    void setDefaultSize();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TextWindow *ui;

    /**
     * Initializes the UI
     */
    void init();

private slots:
    /**
     * Push Button Action
     */
    void on_pushButton_clicked();
};

#endif // TEXTWINDOW_H
