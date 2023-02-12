/**
 * SameGame
 * Copyright 2014 Denis Meyer
 * https://sites.google.com/site/calltopowersoftware/software
 */

#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QString>

/**
  * Class ClickableLabel <- QLabel
  */
class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    /**
      * Constructor
      * @param parent Parent Widget
      * @param number Number
      * @param color Color
      */
    ClickableLabel(QWidget *parent = 0, int number = 0, QString color = "grey");

    /**
      * Destructor
      */
    ~ClickableLabel();

    /**
      * Returns the Number
      * @return the Number
      */
    int getNr() const;

    /**
      * Returns the Color
      * @return the Color
      */
    QString getColor() const;

    /**
      * Sets the Number
      * @param number The Number to set
      * @return true if Number has been successfully, false else
      */
    bool setNr(const int number);

    /**
      * Sets the Color
      * @param number The Color to set
      * @return true if Color has been successfully, false else
      */
    bool setColor(const QString c);

private:
    // The Number
    int m_nr;
    // The Color
    QString m_color;

    /**
      * Event when Mouse released
      * @event Event when Mouse released
      */
    void mouseReleaseEvent(QMouseEvent *event);

    /*****************************************************************************
     * Signals
     ***************************************************************************/

signals:
    /**
      * Signal emitted when Label has been clicked
      * @param number The Number
      */
    void clicked(int number);
};

#endif // CLICKABLELABEL_H
