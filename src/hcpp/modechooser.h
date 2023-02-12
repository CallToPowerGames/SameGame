/**
 * SameGame
 * Copyright 2014 Denis Meyer
 * https://sites.google.com/site/calltopowersoftware/software
 */

#ifndef MODECHOOSER_H
#define MODECHOOSER_H

#include <QDialog>
#include "samegame.h"

namespace Ui {
class ModeChooser;
}

class SameGame;

class ModeChooser : public QDialog
{
    Q_OBJECT

public:
    explicit ModeChooser(SameGame *sg, int nrOfColors = 3, QWidget *parent = 0);
    ~ModeChooser();

    /**
      * Sets the Number of Colors
      * @param nrOfColors Number of Colors
      */
    void setNrOfColors(int nrOfColors);

    /**
      * Sets Difficulty Mode
      * @param normalMode true if normal Mode, false else
      */
    void setMode(bool normalMode);

    /**
      * Sets Image Mode if b
      * @param b if Image Mode or not
      */
    void setImageMode(bool b);

private:
    Ui::ModeChooser *ui;
    SameGame *m_sg;
    int m_nrOfColors;

signals:
    /**
      * Call if Window should be showed
      */
    void showWindowSignal();

    /**
      * If Button Ok has been clicked
      */
    void buttonOkClicked();

    /**
      * If Button Calcel has been clicked
      */
    void buttonCancelClicked();

private slots:
    /**
      * Displays the Window
      */
    void showWindow();

    /**
      * Combobox 'Styles' changed
      * @param str Current Value
      */
    void on_comboBox_Styles_activated(QString str);

    /**
      * Combobox 'Modes' changed
      * @param str Current Value
      */
    void on_comboBox_Modes_activated(QString str);

    /**
      * Spinbox 'Number of Colors' changed
      * @param nrOfColors Current Value
      */
    void on_spinBox_NumberOfColors_valueChanged(int nrOfColors);

    /**
      * Spinbox 'Field Size' changed
      * @param nrOfColors Current Value
      */
    void comboBox_FieldSize_valueChanged(int size);

    /**
      * Button 'Ok' has been clicked
      */
    void buttonOk_clicked();

    /**
      * Button 'Cancel' has been clicked
      */
    void buttonCancel_clicked();
};

#endif // MODECHOOSER_H
