/**
 * SameGame
 * Copyright 2014 Denis Meyer
 * https://sites.google.com/site/calltopowersoftware/software
 */

#include "modechooser.h"
#include "ui_modechooser.h"

ModeChooser::ModeChooser(SameGame *sg, int nrOfColors, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModeChooser)
{
    ui->setupUi(this);
    m_sg = sg;

    // Modes
    ui->comboBox_Modes->addItem("Normal");
    ui->comboBox_Modes->addItem("Easy");

    // Number of Colors
    setNrOfColors(nrOfColors);
    ui->spinBox_NumberOfColors->setValue(m_nrOfColors);

    // Styles
    ui->comboBox_Styles->addItem("With Images");
    ui->comboBox_Styles->addItem("Without Images");

    connect(ui->pushButton_NewGame_Ok, SIGNAL(clicked()), this, SLOT(buttonOk_clicked()));
    connect(ui->pushButton_NewGame_Cancel, SIGNAL(clicked()), this, SLOT(buttonCancel_clicked()));
    connect(this, SIGNAL(rejected()), this, SLOT(buttonCancel_clicked()));
    connect(ui->spinBox_fieldSize, SIGNAL(valueChanged(int)), this, SLOT(comboBox_FieldSize_valueChanged(int)));

    connect(this, SIGNAL(showWindowSignal()), this, SLOT(showWindow()));
}

ModeChooser::~ModeChooser()
{
    delete ui;
}

void ModeChooser::showWindow()
{
    ui->pushButton_NewGame_Ok->setFocus();
    this->show();
}

void ModeChooser::setNrOfColors(int nrOfColors)
{
    if((nrOfColors >= 3) && (nrOfColors <= 8))
    {
        m_nrOfColors = nrOfColors;
    } else
    {
        m_nrOfColors = 3;
    }
    ui->spinBox_NumberOfColors->setValue(m_nrOfColors);
}

void ModeChooser::setMode(bool normalMode)
{
    if(normalMode)
    {
        ui->comboBox_Modes->setCurrentIndex(0);
    } else
    {
        ui->comboBox_Modes->setCurrentIndex(1);
    }
}

void ModeChooser::setImageMode(bool b)
{
    if(b)
    {
        ui->comboBox_Styles->setCurrentIndex(0);
    } else
    {
        ui->comboBox_Styles->setCurrentIndex(1);
    }
}

void ModeChooser::on_comboBox_Styles_activated(QString str)
{
    if(QString::compare(str, "With Images") == 0)
    {
        m_sg->setStyle(true);
    } else
    {
        m_sg->setStyle(false);
    }
}

void ModeChooser::on_comboBox_Modes_activated(QString str)
{
    if(QString::compare(str, "Normal") == 0)
    {
        m_sg->setMode(true);
    } else
    {
        m_sg->setMode(false);
    }
}

void ModeChooser::on_spinBox_NumberOfColors_valueChanged(int nrOfColors)
{
    setNrOfColors(nrOfColors);
    m_sg->setNumberOfColors(m_nrOfColors);
}

void ModeChooser::comboBox_FieldSize_valueChanged(int size)
{
    if((size >= 10) && (size <= 30))
    {
        ui->label_fieldSize->setText(QString("%1 x %1").arg(size));
        m_sg->setFieldSize(size);
    }
}

void ModeChooser::buttonOk_clicked()
{
    m_sg->setupStart();
    this->hide();
    emit buttonOkClicked();
}

void ModeChooser::buttonCancel_clicked()
{
    m_sg->cancelStart();
    this->hide();
    emit buttonCancelClicked();
}
