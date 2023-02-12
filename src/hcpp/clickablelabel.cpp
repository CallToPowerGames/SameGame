/**
 * SameGame
 * Copyright 2014 Denis Meyer
 * https://sites.google.com/site/calltopowersoftware/software
 */

#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent, int number, QString color) :
    QLabel(parent)
{
    if(!setNr(number))
    {
        number = 0;
    }
    if(!setColor(color))
    {
        this->m_color = "grey";
    }
}

ClickableLabel::~ClickableLabel()
{
    // Do nothing in here
}

int ClickableLabel::getNr() const
{
    return m_nr;
}

QString ClickableLabel::getColor() const
{
    return m_color;
}

bool ClickableLabel::setNr(const int number)
{
    if(number >= 0)
    {
        m_nr = number;
        return true;
    }
    return false;
}

bool ClickableLabel::setColor(const QString c)
{
    if(!(c == ""))
    {
        m_color = c;
        return true;
    }
    return false;
}

/*****************************************************************************
  * Slots
  ***************************************************************************/

void ClickableLabel::mouseReleaseEvent(QMouseEvent *event)
{
    emit clicked(m_nr);
}
