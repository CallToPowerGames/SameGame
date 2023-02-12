/**
 * SameGame
 * Copyright 2014 Denis Meyer
 * https://sites.google.com/site/calltopowersoftware/software
 */

#include <QtWidgets>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QDir>
#include "samegame.h"
#include "ui_samegame.h"
#include "ui_modechooser.h"
#if defined(Q_OS_MAC)
#else
#include <cstdlib> // for srand, rand
#include <ctime>   // for time
#endif

// Information Strings
const QString VERSION = "1.0.0";
const QString about = "About SameGame";
const QString aboutThisGame = "How to play";

const QString numberOfColors3 = "Set Number of Colors to 3.";
const QString numberOfColors4 = "Set Number of Colors to 4.";
const QString numberOfColors5 = "Set Number of Colors to 5.";
const QString numberOfColors6 = "Set Number of Colors to 6.";
const QString numberOfColors7 = "Set Number of Colors to 7.";
const QString numberOfColors8 = "Set Number of Colors to 8.";

// Color Strings - Do not change the color names!
const QString grey = "grey";
const QString blue = "blue";
const QString red = "red";
const QString green = "green";
const QString orange = "orange";
const QString purple = "purple";
const QString yellow = "yellow";
const QString cyan = "cyan";
const QString magenta = "magenta";

// Strings for InGame
const QString fieldSizeSetTo = "Field Size set to %1 x %1.";
const QString stopGame = "Stop Game";
const QString newGame = "New Game";
const QString switchedToNormalMode = "Switched to normal Mode.";
const QString switchedToEasyMode = "Switched to easy Mode.";
const QString switchedToWithImagesMode = "Switched to 'With Images' Mode.";
const QString switchedToWithoutImagesMode = "Switched to 'Without Images' Mode.";
const QString defaultStatusbar = "To start click at 'Options > New Game' in the Menubar.";
const QString numberOfColorsSetTo = "Number of Colors set to %1.";
const QString stopCurrentGameStartNew = "Stop current Game and start a new one?";
const QString stoppedCurrentGame = "Stopped the current Game.";
const QString statusBarInGame = "Time: %1 Seconds - Blocks destroyed: %2/%3\nScore: %4";
const QString youWonMessage1 = "You won! Time: %1 Seconds\nScore: %2";
const QString youWonMessage2 = "You won!\n\nTime: %1 Seconds\nScore: %2";
const QString youLostMessage1 = "You lost! Time: %1 Seconds, Blocks left: %2\nScore: %3";
const QString youLostMessage2 = "You lost!\n\nTime: %1 Seconds\nBlocks destroyed: %2/%3\nBlocks left: %4\nScore: %5";
const QString gameOverString = "Game over";
const QString startNewGameQuestion = "Start a new Game?";

SameGame::SameGame(QWidget *parent, int fieldSize) :
    QMainWindow(parent),
    ui(new Ui::SameGame)
{
    srand(time(0));

    ui->setupUi(this);

    // Avoid maximizing
    // this->setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);

    // Window Resize Handling
    connect(this, SIGNAL(resized()), this, SLOT(windowResized()));

    // Quit
    connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()));

    // Set Application-wide Logo
    QApplication::setWindowIcon(QIcon(":/resources/img/icon"));

    m_modeChooser = new ModeChooser(this, 3, this);
    m_modeChooser->setModal(true);

    m_seconds = 0;
    m_timer = new QTimer(this);
    m_blocksDestroyed = 0;
    m_blocksDestroyedBackup = m_blocksDestroyed;

    setUpStatusbar();

    player_applause = new QMediaPlayer;
    player_dink = new QMediaPlayer;
    player_game_over = new QMediaPlayer;
    player_gong = new QMediaPlayer;
    player_undo = new QMediaPlayer;

    player_applause_loaded = false;
    player_dink_loaded = false;
    player_game_over_loaded = false;
    player_gong_loaded = false;
    player_undo_loaded = false;

    textwindow_about = NULL;
    textwindow_howto = NULL;

    // Start in Normal Mode
    m_modeNormal = false;
    switchMode();

    // Start in Image Mode
    m_modeImages = false;
    switchImageMode();

    if(fieldSize >= 10)
    {
        m_fieldSize = fieldSize;
        m_newFieldSize = fieldSize;
    } else
    {
        m_fieldSize = 10;
    }

    // disable Undo-Action in the Menubar
    ui->actionUndo->setEnabled(false);

    m_colors.clear();
    m_colors.append(grey);
    m_colors.append(blue);
    m_colors.append(red);
    m_colors.append(green);
    m_colors.append(orange);
    m_colors.append(purple);
    m_colors.append(yellow);
    m_colors.append(cyan);
    m_colors.append(magenta);

    setNumberOfColors(3);

    m_gameRunning = false;
    enableLabels();
    updateStatusBar(defaultStatusbar);

    m_initialCl = new ClickableLabel(0, 999);
    m_initialCl->setAlignment(Qt::AlignCenter);
    m_initialCl->setPixmap(QPixmap(":/resources/img/logo"));
    m_initialCl->setFrameStyle(QFrame::Panel | QFrame::Raised);
    connect(m_initialCl, SIGNAL(clicked(int)), this, SLOT(initialClickableLabelClicked()));
    connect(m_modeChooser, SIGNAL(buttonOkClicked()), this, SLOT(playGongSound()));
    connect(m_modeChooser, SIGNAL(buttonOkClicked()), this, SLOT(initialClickableLabelRemove()));
    connect(m_modeChooser, SIGNAL(buttonCancelClicked()), this, SLOT(initialClickableLabelReset()));
    ui->gridLayout->addWidget(m_initialCl);

    connect(player_applause, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(play_applause(QMediaPlayer::MediaStatus)));
    connect(player_dink, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(play_dink(QMediaPlayer::MediaStatus)));
    connect(player_game_over, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(play_game_over(QMediaPlayer::MediaStatus)));
    connect(player_gong, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(play_gong(QMediaPlayer::MediaStatus)));
    connect(player_undo, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(play_undo(QMediaPlayer::MediaStatus)));

    // Display after 2 Seconds a 'New Game' Dialog
    // QTimer::singleShot(2000, m_modeChooser, SIGNAL(showWindowSignal()));
}

SameGame::~SameGame()
{
    if(labelsSet())
    {
        int counter = 0;
        for(int i = 0; i < m_fieldSize; ++i)
        {
            for(int j = 0; j < m_fieldSize; ++j)
            {
                ui->gridLayout->removeWidget(m_labels[counter]);
                delete m_labels[counter];
                ++counter;
            }
        }
        m_labels.clear();
    }
    for(int i = 0; i < m_labelsBackup.size(); ++i)
    {
        delete m_labelsBackup[i];
    }
    m_labelsBackup.clear();
    m_map_del.clear();
    if(m_timer)
    {
        delete m_timer;
    }
    if(m_modeChooser)
    {
        delete m_modeChooser;
    }
    if(m_initialCl)
    {
        delete m_initialCl;
    }
    if(textwindow_about)
    {
        delete textwindow_about;
    }
    if(textwindow_howto)
    {
        delete textwindow_howto;
    }
    delete ui;
}

QString SameGame::getCurrentDir() {
    QDir dir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_MAC)
    if (dir.dirName() == "MacOS")
    {
        dir.cdUp();
    }
#endif

    QString dir_str = dir.absolutePath();
#if defined(Q_OS_MAC)
    dir_str = dir_str.endsWith(QDir::separator()) ? dir_str : QString("%1%2%3").arg(dir_str).arg(QDir::separator()).arg("Resources");
#endif
    return dir_str;
}

void SameGame::play_applause(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::LoadedMedia)
    {
        player_applause_loaded = true;
        player_applause->play();
    }
}

void SameGame::play_dink(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::LoadedMedia)
    {
        player_dink_loaded = true;
        player_dink->play();
    }
}

void SameGame::play_game_over(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::LoadedMedia)
    {
        player_game_over_loaded = true;
        player_game_over->play();
    }
}

void SameGame::play_gong(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::LoadedMedia)
    {
        player_gong_loaded = true;
        player_gong->play();
    }
}

void SameGame::play_undo(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::LoadedMedia)
    {
        player_undo_loaded = true;
        player_undo->play();
    }
}

void SameGame::playDinkSound()
{
    QString sound(getCurrentDir() + "/audio/dink.wav");
    if(QFile(sound).exists())
    {
        if(player_dink_loaded)
        {
            player_dink->stop();
            player_dink->play();
        } else
        {
            player_dink->setMedia(QUrl::fromLocalFile(sound));
            player_dink->setVolume(100);
        }
    }
}

void SameGame::playApplauseSound()
{
    QString sound(getCurrentDir() + "/audio/applause.wav");
    if(QFile(sound).exists())
    {
        if(player_applause_loaded)
        {
            player_applause->play();
        } else
        {
            player_applause->setMedia(QUrl::fromLocalFile(sound));
            player_applause->setVolume(100);
        }
    }
}

void SameGame::playGameOverSound()
{
    QString sound(getCurrentDir() + "/audio/game_over.wav");
    if(QFile(sound).exists())
    {
        if(player_game_over_loaded)
        {
            player_game_over->play();
        } else
        {
            player_game_over->setMedia(QUrl::fromLocalFile(sound));
            player_game_over->setVolume(100);
        }
    }
}

void SameGame::playUndoSound()
{
    QString sound(getCurrentDir() + "/audio/undo.wav");
    if(QFile(sound).exists())
    {
        if(player_undo_loaded)
        {
            player_undo->play();
        } else
        {
            player_undo->setMedia(QUrl::fromLocalFile(sound));
            player_undo->setVolume(100);
        }
    }
}

void SameGame::playGongSound()
{
    QString sound(getCurrentDir() + "/audio/gong.wav");
    if(QFile(sound).exists())
    {
        if(player_gong_loaded)
        {
            player_gong->play();
        } else
        {
            player_gong->setMedia(QUrl::fromLocalFile(sound));
            player_gong->setVolume(100);
        }
    }
}

int SameGame::getScore(int blocksDeleted)
{
    return (blocksDeleted / 2) * m_numberOfColors;
}

void SameGame::initialClickableLabelClicked()
{
    m_initialCl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    on_actionNew_Game_triggered();
}

void SameGame::initialClickableLabelReset()
{
    m_initialCl->setFrameStyle(QFrame::Panel | QFrame::Raised);
    m_initialCl->setEnabled(true);
    m_initialCl->setVisible(true);
}

void SameGame::initialClickableLabelRemove()
{
    m_initialCl->setEnabled(false);
    m_initialCl->setVisible(false);
}

void SameGame::setMode(bool modeNormal)
{
    m_modeNormal = !modeNormal;
    switchMode();
}

void SameGame::setStyle(bool modeImages)
{
    m_modeImages = !modeImages;
    switchImageMode();
}

void SameGame::setFieldSize(int fieldSize)
{
    if((fieldSize >= 10) && (fieldSize <= 30))
    {
        m_newFieldSize = fieldSize;
        updateStatusBar(QString(fieldSizeSetTo).arg(m_newFieldSize));
    }
}

void SameGame::setUpStatusbar()
{
    m_labelInformation = new QLabel;
    m_labelInformation->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_labelInformation->setLineWidth(2);
    ui->statusBar->addPermanentWidget(m_labelInformation);
}

void SameGame::updateStatusBar(const QString statusUpdate)
{
    if(statusUpdate != 0) {
        m_labelInformation->setText(statusUpdate);
    }
}

int SameGame::to1Dimension(const int x, const int y) const
{
    return y + (x * m_fieldSize);
}

void SameGame::to2Dimension(const int n, int &x, int &y) const
{
    x = (n - (n % m_fieldSize)) / m_fieldSize;
    y = n % m_fieldSize;
}

void SameGame::initLabels()
{
    m_fieldSize = m_newFieldSize;
    ClickableLabel *l;
    m_labels = QVector<ClickableLabel*>(m_fieldSize * m_fieldSize);
    int counter = 0;
    for(int i = 0; i < m_fieldSize; ++i)
    {
        for(int j = 0; j < m_fieldSize; ++j)
        {
            QString color = m_gameRunning ? m_colors[(rand() % m_numberOfColors) + 1] : m_colors[0];
            l = new ClickableLabel(0, counter, color);
            l->setBaseSize(5, 5);
            l->setColor(color);
            l->setScaledContents(true);
            styleLabel(l, color);
            ui->gridLayout->addWidget(l, i, j);
            m_labels[counter] = l;
            ++counter;
        }
        // Don't stretch
        ui->gridLayout->setColumnStretch(i, 0);
        ui->gridLayout->setRowStretch(i, 0);
    }
    if(m_gameRunning)
    {
        connectLabels();
    }
}

void SameGame::delLabels()
{
    // Delete all Labels inside m_labels and remove them from the GridLayout
    if(labelsSet())
    {
        for(int i = 0; i < m_labels.size(); ++i)
        {
            disconnect(m_labels[i]);
            ui->gridLayout->removeWidget(m_labels[i]);
            delete m_labels[i];
        }
        m_labels.clear();
    }
}

void SameGame::enableLabels()
{
    if(labelsSet())
    {
        for(int i = 0; i < m_labels.size(); ++i)
        {
            m_labels[i]->setEnabled(m_gameRunning);
        }
    }
    if(m_gameRunning)
    {
        ui->actionNew_Game->setText(stopGame);
        ui->menuMode->setEnabled(false);
        ui->menuStyle->setEnabled(false);
        ui->menuColors->setEnabled(false);
    } else
    {
        ui->actionNew_Game->setText(newGame);
        ui->menuMode->setEnabled(true);
        ui->menuStyle->setEnabled(true);
        ui->menuColors->setEnabled(true);
    }
}

void SameGame::connectLabels()
{
    if(labelsSet())
    {
        for(int i = 0; i < m_labels.size(); ++i)
        {
            if(!(m_labels[i] == 0))
            {
                connect(m_labels[i], SIGNAL(clicked(int)), this, SLOT(checkField(int)));
            } else
            {
                quitApplication();
            }
        }
    }
}

bool SameGame::labelsSet() const
{
    return (!m_labels.empty() && (m_labels.size() == (m_fieldSize * m_fieldSize)));
}

void SameGame::quitApplication()
{
    this->close();
}

bool SameGame::colorEqual(const QString &s1, const QString &s2) const
{
    if((s1 != 0) && (s2 != 0))
    {
        return (QString::compare(s1, s2, Qt::CaseInsensitive) == 0);
    }
    return false;
}

bool SameGame::styleLabel(ClickableLabel *label) const
{
    return styleLabel(label, label->getColor());
}

bool SameGame::styleLabel(ClickableLabel *label, const QString color) const
{
    if((label != 0) && (color != 0))
    {
        if(m_modeImages)
        {
            label->setStyleSheet(QString("background-color: %1").arg("none"));
            if(!colorEqual(color, m_colors[0]))
            {
                label->clear();
                label->setPixmap(QPixmap(QString(":/resources/img/%1").arg(color)));
            } else
            {
                label->clear();
                label->setPixmap(QPixmap(QString(":/resources/img/%1").arg("spacer")));
            }
        } else
        {
            label->clear();
            if(!colorEqual(color, m_colors[0]))
            {
                label->setStyleSheet(QString("background-color: %1").arg(color));
            } else
            {
                label->setStyleSheet(QString("background-color: %1").arg("none"));
            }
        }
        return true;
    }
    return false;
}

bool SameGame::checkNorth(const int fieldNumber, const bool insert)
{
    QString fieldColor = m_labels[fieldNumber]->getColor();
    int x, y;
    to2Dimension(fieldNumber, x, y);

    bool changedOnce = false;
    if(x > 0)
    {
        if(colorEqual(m_labels[to1Dimension(x - 1, y)]->getColor(), fieldColor))
        {
            if(insert && insertIntoMap(to1Dimension(x - 1, y)))
            {
                changedOnce = true;
            } else if(!insert)
            {
                changedOnce = true;
            }
        }
    }
    return changedOnce;
}

bool SameGame::checkNorthEast(const int fieldNumber, const bool insert)
{
    QString fieldColor = m_labels[fieldNumber]->getColor();
    int x, y;
    to2Dimension(fieldNumber, x, y);

    bool changedOnce = false;
    if((x > 0) && (y < m_fieldSize - 1))
    {
        if(colorEqual(m_labels[to1Dimension(x - 1, y + 1)]->getColor(), fieldColor))
        {
            if(insert && insertIntoMap(to1Dimension(x - 1, y + 1)))
            {
                changedOnce = true;
            } else if(!insert)
            {
                changedOnce = true;
            }
        }
    }
    return changedOnce;
}

bool SameGame::checkEast(const int fieldNumber, const bool insert)
{
    QString fieldColor = m_labels[fieldNumber]->getColor();
    int x, y;
    to2Dimension(fieldNumber, x, y);

    bool changedOnce = false;
    if(y < m_fieldSize - 1)
    {
        if(colorEqual(m_labels[to1Dimension(x, y + 1)]->getColor(), fieldColor))
        {
            if(insert && insertIntoMap(to1Dimension(x, y + 1)))
            {
                changedOnce = true;
            } else if(!insert)
            {
                changedOnce = true;
            }
        }
    }
    return changedOnce;
}

bool SameGame::checkSouthEast(const int fieldNumber, const bool insert)
{
    QString fieldColor = m_labels[fieldNumber]->getColor();
    int x, y;
    to2Dimension(fieldNumber, x, y);

    bool changedOnce = false;
    if((x < m_fieldSize - 1) && (y < m_fieldSize - 1))
    {
        if(colorEqual(m_labels[to1Dimension(x + 1, y + 1)]->getColor(), fieldColor))
        {
            if(insert && insertIntoMap(to1Dimension(x + 1, y + 1)))
            {
                changedOnce = true;
            } else if(!insert)
            {
                changedOnce = true;
            }
        }
    }
    return changedOnce;
}

bool SameGame::checkSouth(const int fieldNumber, const bool insert)
{
    QString fieldColor = m_labels[fieldNumber]->getColor();
    int x, y;
    to2Dimension(fieldNumber, x, y);

    bool changedOnce = false;
    if(x < m_fieldSize - 1)
    {
        if(colorEqual(m_labels[to1Dimension(x + 1, y)]->getColor(), fieldColor))
        {
            if(insert && insertIntoMap(to1Dimension(x + 1, y)))
            {
                changedOnce = true;
            } else if(!insert)
            {
                changedOnce = true;
            }
        }
    }
    return changedOnce;
}

bool SameGame::checkSouthWest(const int fieldNumber, const bool insert)
{
    QString fieldColor = m_labels[fieldNumber]->getColor();
    int x, y;
    to2Dimension(fieldNumber, x, y);

    bool changedOnce = false;
    if((x < m_fieldSize - 1) && (y > 0))
    {
        if(colorEqual(m_labels[to1Dimension(x + 1, y - 1)]->getColor(), fieldColor))
        {
            if(insert && insertIntoMap(to1Dimension(x + 1, y - 1)))
            {
                changedOnce = true;
            } else if(!insert)
            {
                changedOnce = true;
            }
        }
    }
    return changedOnce;
}

bool SameGame::checkWest(const int fieldNumber, const bool insert)
{
    QString fieldColor = m_labels[fieldNumber]->getColor();
    int x, y;
    to2Dimension(fieldNumber, x, y);

    bool changedOnce = false;
    if(y > 0)
    {
        if(colorEqual(m_labels[to1Dimension(x, y - 1)]->getColor(), fieldColor))
        {
            if(insert && insertIntoMap(to1Dimension(x, y - 1)))
            {
                changedOnce = true;
            } else if(!insert)
            {
                changedOnce = true;
            }
        }
    }
    return changedOnce;
}

bool SameGame::checkNorthWest(const int fieldNumber, const bool insert)
{
    QString fieldColor = m_labels[fieldNumber]->getColor();
    int x, y;
    to2Dimension(fieldNumber, x, y);

    bool changedOnce = false;
    if((x > 0) && (y > 0))
    {
        if(colorEqual(m_labels[to1Dimension(x - 1, y - 1)]->getColor(), fieldColor))
        {
            if(insert && insertIntoMap(to1Dimension(x - 1, y - 1)))
            {
                changedOnce = true;
            } else if(!insert)
            {
                changedOnce = true;
            }
        }
    }
    return changedOnce;
}

bool SameGame::insertIntoMap(int number)
{
    if((number >= 0) && (number <= ((m_fieldSize * m_fieldSize) - 1)))
    {
        m_map_del.insert(m_labels[number]->getNr(), m_labels[number]);
        return true;
    }
    return false;
}

bool SameGame::checkAll(const int number, const bool insert)
{
    if((number >= 0) &&
            (number <= (m_fieldSize * m_fieldSize)) &&
            !isGrey(m_labels[number]) && m_gameRunning)
    {
        if(m_modeNormal)
        {
            bool r1 = checkNorth(number, insert);
            bool r3 = checkEast(number, insert);
            bool r5 = checkSouth(number, insert);
            bool r7 = checkWest(number, insert);
            return (r1 || r3 || r5 || r7);
        } else
        {
            bool r1 = checkNorth(number, insert);
            bool r2 = checkNorthEast(number, insert);
            bool r3 = checkEast(number, insert);
            bool r4 = checkSouthEast(number, insert);
            bool r5 = checkSouth(number, insert);
            bool r6 = checkSouthWest(number, insert);
            bool r7 = checkWest(number, insert);
            bool r8 = checkNorthWest(number, insert);
            return (r1 || r2 || r3 || r4 || r5 || r6 || r7 || r8);
        }
    }
    return false;
}

void SameGame::loop(const int number)
{
    if(checkAll(number, true))
    {
        insertIntoMap(number);
        m_labels[number]->setColor(m_colors[0]);
        styleLabel(m_labels[number]);
        QMap<int, ClickableLabel*>::const_iterator i = m_map_del.begin();
        while (i != m_map_del.end()) {
            loop((i.value())->getNr());
            ++i;
        }
    }
}

bool SameGame::allGrey() const
{
    if(labelsSet())
    {
        for(int i = 0; i < m_labels.size(); ++i) {
            for(int co = 1; co < m_colors.size(); ++co)
            {
                if(colorEqual(m_colors[co], m_labels[i]->getColor()))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool SameGame::gameOver()
{
    if(labelsSet())
    {
        for(int i = 0; i < m_labels.size(); ++i) {
            if(checkAll(m_labels[i]->getNr(), false))
            {
                return false;
            }
        }
    }
    return true;
}

void SameGame::setDestroyedBlocks()
{
    if(labelsSet())
    {
        int db = 0;
        for(int i = 0; i < m_labels.size(); ++i) {
            if(isGrey(m_labels[i]))
            {
                ++db;
            }
        }
        m_blocksDestroyed = db;
        if((m_blocksDestroyed - m_blocksDestroyedBackup) > 0)
        {
            playDinkSound();
        }
    }
}

bool SameGame::isGrey(const ClickableLabel *label) const
{
    return (colorEqual(m_colors[0], label->getColor()));
}

void SameGame::switchLabels(int number1, int number2)
{
    ClickableLabel *tmpStorage = new ClickableLabel();

    // Only switch the Color of the Fields, not the Number!
    tmpStorage->setColor(m_labels[number1]->getColor());
    styleLabel(tmpStorage, m_labels[number1]->getColor());

    m_labels[number1]->setColor(m_labels[number2]->getColor());
    styleLabel(m_labels[number1], m_labels[number2]->getColor());

    m_labels[number2]->setColor(tmpStorage->getColor());
    styleLabel(m_labels[number2], tmpStorage->getColor());
}

void SameGame::fillupLines()
{
    int x, y, currField, belowField, leftField;
    to2Dimension((m_fieldSize * m_fieldSize) - 1, x, y);
    bool ready = false, switched = false;

    while(!ready)
    {
        switched = false;
        for(int ix = 0; ix < x; ++ix)
        {
            for(int iy = 0; iy <= y; ++iy)
            {
                currField = to1Dimension(ix, iy);
                belowField = to1Dimension(ix + 1, iy);
                if(!isGrey(m_labels[currField]) && isGrey(m_labels[belowField]))
                {
                    switchLabels(currField, belowField);
                    switched = true;
                }
            }
        }

        if(!switched)
        {
            for(int ix = 0; ix <= x; ++ix)
            {
                for(int iy = y; iy > 0; --iy)
                {
                    currField = to1Dimension(ix, iy);
                    leftField = to1Dimension(ix, iy - 1);
                    if(!isGrey(m_labels[currField]) && isGrey(m_labels[leftField]))
                    {
                        switchLabels(currField, leftField);
                        switched = true;
                    }
                }
            }
        }

        if(!switched)
        {
            ready = true;
        }
    }
}

void SameGame::switchMode()
{
    m_modeNormal = !m_modeNormal;
    ui->action_ModeEasy->setEnabled(m_modeNormal);
    ui->action_ModeNormal->setEnabled(!m_modeNormal);
    if(m_modeNormal)
    {
        m_modeChooser->setMode(true);
        updateStatusBar(switchedToNormalMode);
    } else
    {
        m_modeChooser->setMode(false);
        updateStatusBar(switchedToEasyMode);
    }
}

void SameGame::switchImageMode()
{
    m_modeImages = !m_modeImages;
    ui->action_One_Color->setEnabled(m_modeImages);
    ui->action_Images->setEnabled(!m_modeImages);
    if(m_modeImages)
    {
        m_modeChooser->setImageMode(true);
        ui->gridLayout->setSpacing(0);
        updateStatusBar(switchedToWithImagesMode);
    } else
    {
        m_modeChooser->setImageMode(false);
        ui->gridLayout->setSpacing(1);
        updateStatusBar(switchedToWithoutImagesMode);
    }
}

void SameGame::setupStart()
{
    m_gameRunning = true;
    initLabels();
    m_blocksDestroyed = 0;
    m_seconds = 0;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    m_timer->start(1000);
    enableLabels();
    updateWithoutTime();
}

void SameGame::cancelStart()
{
    m_gameRunning = false;
    enableLabels();
    updateStatusBar(defaultStatusbar);
}

bool SameGame::setNumberOfColors(const int number)
{
    if((number >= 3) && (number <= (m_colors.size() - 1)))
    {
        // Set other Actions to enabled
        switch(number)
        {
        case 3:
            ui->action3_Colors->setEnabled(false);
            ui->action4_Colors->setEnabled(true);
            ui->action5_Colors->setEnabled(true);
            ui->action6_Colors->setEnabled(true);
            ui->action7_Colors->setEnabled(true);
            ui->action8_Colors->setEnabled(true);
            break;
        case 4:
            ui->action3_Colors->setEnabled(true);
            ui->action4_Colors->setEnabled(false);
            ui->action5_Colors->setEnabled(true);
            ui->action6_Colors->setEnabled(true);
            ui->action7_Colors->setEnabled(true);
            ui->action8_Colors->setEnabled(true);
            break;
        case 5:
            ui->action3_Colors->setEnabled(true);
            ui->action4_Colors->setEnabled(true);
            ui->action5_Colors->setEnabled(false);
            ui->action6_Colors->setEnabled(true);
            ui->action7_Colors->setEnabled(true);
            ui->action8_Colors->setEnabled(true);
            break;
        case 6:
            ui->action3_Colors->setEnabled(true);
            ui->action4_Colors->setEnabled(true);
            ui->action5_Colors->setEnabled(true);
            ui->action6_Colors->setEnabled(false);
            ui->action7_Colors->setEnabled(true);
            ui->action8_Colors->setEnabled(true);
            break;
        case 7:
            ui->action3_Colors->setEnabled(true);
            ui->action4_Colors->setEnabled(true);
            ui->action5_Colors->setEnabled(true);
            ui->action6_Colors->setEnabled(true);
            ui->action7_Colors->setEnabled(false);
            ui->action8_Colors->setEnabled(true);
            break;
        case 8:
            ui->action3_Colors->setEnabled(true);
            ui->action4_Colors->setEnabled(true);
            ui->action5_Colors->setEnabled(true);
            ui->action6_Colors->setEnabled(true);
            ui->action7_Colors->setEnabled(true);
            ui->action8_Colors->setEnabled(false);
            break;
        }

        m_numberOfColors = number;
        updateStatusBar(QString(numberOfColorsSetTo).arg(m_numberOfColors));
        return true;
    }
    return false;
}

void SameGame::startStopGame(bool ask)
{
    if(m_gameRunning && ask)
    {
        int n = QMessageBox::warning(this,
                                     newGame,
                                     stopCurrentGameStartNew,
                                     QMessageBox::No | QMessageBox::Yes);
        if(n == QMessageBox::Yes)
        {
            updateStatusBar(stoppedCurrentGame);
            ui->actionUndo->setEnabled(false);
            delLabels();
            if(m_timer->isActive())
            {
                m_timer->stop();
                m_timer->disconnect(this);
            }
            m_modeChooser->show();
            m_modeChooser->exec();
        }
    } else
    {
        ui->actionUndo->setEnabled(false);
        delLabels();
        for(int i = 0; i < m_labelsBackup.size(); ++i)
        {
            delete m_labelsBackup[i];
        }
        m_labelsBackup.clear();
        if(m_timer->isActive())
        {
            m_timer->stop();
            m_timer->disconnect(this);
        }
        m_modeChooser->show();
        m_modeChooser->exec();
    }
}

/*****************************************************************************
  * Slots
  ***************************************************************************/

void SameGame::windowResized()
{
    QDesktopWidget *d = QApplication::desktop();
    int dx = d->width();    // Screen width
    int dy = d->height();   // Screen height

    int w = this->width();  // Application Width
    int h = this->height(); // Application Height
    int x = this->x() + w;  // Lower x-Cordinate of the Application
    int y = this->y() + h;  // Lower y-Cordinate of the Application

    // If not bigger than the Screen
    if((x < (dx - 50)) && (y < (dy - 50)))
    {
        if(w >= h)
        {
            this->resize(QSize(w, w));
        } else
        {
            this->resize(QSize(h, h));
        }
    } else
    {
        if(dx >= dy)
        {
            this->resize(QSize(dy - 70, dy - 70));
        } else
        {
            this->resize(QSize(dx - 70, dx - 70));
        }
    }
}

void SameGame::updateTime()
{
    updateStatusBar(QString(statusBarInGame).arg(++m_seconds).arg(m_blocksDestroyed).arg(m_labels.size()).arg(getScore(m_blocksDestroyed)));
}

void SameGame::updateWithoutTime()
{
    updateStatusBar(QString(statusBarInGame).arg(m_seconds).arg(m_blocksDestroyed).arg(m_labels.size()).arg(getScore(m_blocksDestroyed)));
}

void SameGame::checkField(const int number)
{
    if(!isGrey(m_labels[number]))
    {
        m_map_del.clear();

        m_blocksDestroyedBackup = m_blocksDestroyed;
        for(int i = 0; i < m_labelsBackup.size(); ++i)
        {
            delete m_labelsBackup[i];
        }
        m_labelsBackup.clear();

        m_labelsBackup = QVector<ClickableLabel*>(m_fieldSize * m_fieldSize);
        ClickableLabel *l;
        int counter = 0;
        for(int i = 0; i < m_fieldSize; ++i)
        {
            for(int j = 0; j < m_fieldSize; ++j)
            {
                QString color = m_labels[counter]->getColor();
                l = new ClickableLabel(0, m_labels[counter]->getNr(), color);
                l->setBaseSize(m_labels[counter]->baseSize());
                l->setColor(color);
                l->setScaledContents(true);
                styleLabel(l, color);
                m_labelsBackup[counter] = l;
                ++counter;
            }
        }

        loop(number);
        QMap<int, ClickableLabel*>::const_iterator i = m_map_del.begin();
        while (i != m_map_del.end()) {
            i.value()->setColor(m_colors[0]);
            styleLabel(i.value(), m_colors[0]);
            ++i;
        }
        fillupLines();
        setDestroyedBlocks();
        updateWithoutTime();
        if(allGrey())
        {
            playApplauseSound();
            ui->actionUndo->setEnabled(false);
            m_gameRunning = false;
            enableLabels();
            if(m_timer->isActive())
            {
                m_timer->stop();
                m_timer->disconnect(this);
            }
            updateStatusBar(QString(youWonMessage1).arg(m_seconds).arg(getScore(m_blocksDestroyed)));
            QMessageBox::information(this,
                                     gameOverString,
                                     QString(youWonMessage2).arg(m_seconds).arg(getScore(m_blocksDestroyed)),
                                     QMessageBox::Ok);
            int n = QMessageBox::warning(this,
                                         newGame,
                                         startNewGameQuestion,
                                         QMessageBox::No | QMessageBox::Yes);
            if(n == QMessageBox::Yes)
            {
                startStopGame(false);
            }
        } else if(gameOver())
        {
            playGameOverSound();
            ui->actionUndo->setEnabled(false);
            m_gameRunning = false;
            enableLabels();
            if(m_timer->isActive())
            {
                m_timer->stop();
                m_timer->disconnect(this);
            }
            updateStatusBar(QString(youLostMessage1).arg(m_seconds).arg(m_labels.size() - m_blocksDestroyed).arg(getScore(m_blocksDestroyed)));
            QMessageBox::information(this,
                                     gameOverString,
                                     QString(youLostMessage2).arg(m_seconds).arg(m_blocksDestroyed).arg(m_labels.size()).arg(m_labels.size() - m_blocksDestroyed).arg(getScore(m_blocksDestroyed)),
                                     QMessageBox::Ok);
            int n = QMessageBox::warning(this,
                                         newGame,
                                         startNewGameQuestion,
                                         QMessageBox::No | QMessageBox::Yes);
            if(n == QMessageBox::Yes)
            {
                startStopGame(false);
            }
        } else
        {
            ui->actionUndo->setEnabled(true);
        }
    }
}

void SameGame::on_actionUndo_triggered()
{
    ui->actionUndo->setEnabled(false);
    playUndoSound();

    m_blocksDestroyed = m_blocksDestroyedBackup;
    delLabels();

    ClickableLabel *l;
    m_labels = QVector<ClickableLabel*>(m_fieldSize * m_fieldSize);
    int counter = 0;
    for(int i = 0; i < m_fieldSize; ++i)
    {
        for(int j = 0; j < m_fieldSize; ++j)
        {
            QString color = m_labelsBackup[counter]->getColor();
            l = new ClickableLabel(0, m_labelsBackup[counter]->getNr(), color);
            l->setBaseSize(m_labelsBackup[counter]->baseSize());
            l->setColor(color);
            l->setScaledContents(true);
            styleLabel(l, color);
            ui->gridLayout->addWidget(l, i, j);
            m_labels[counter] = l;
            ++counter;
        }
        // Don't stretch
        ui->gridLayout->setColumnStretch(i, 0);
        ui->gridLayout->setRowStretch(i, 0);
    }
    connectLabels();
}

void SameGame::on_actionNew_Game_triggered()
{
    startStopGame(true);
}

void SameGame::on_action_ModeEasy_triggered()
{
    switchMode();
}

void SameGame::on_action_ModeNormal_triggered()
{
    switchMode();
}

void SameGame::on_action_One_Color_triggered()
{
    switchImageMode();
}

void SameGame::on_action_Images_triggered()
{
    switchImageMode();
}

void SameGame::on_action3_Colors_triggered()
{
    m_modeChooser->setNrOfColors(3);
    setNumberOfColors(3);
    updateStatusBar(numberOfColors3);
}

void SameGame::on_action4_Colors_triggered()
{
    m_modeChooser->setNrOfColors(4);
    setNumberOfColors(4);
    updateStatusBar(numberOfColors4);
}

void SameGame::on_action5_Colors_triggered()
{
    m_modeChooser->setNrOfColors(5);
    setNumberOfColors(5);
    updateStatusBar(numberOfColors5);
}

void SameGame::on_action6_Colors_triggered()
{
    m_modeChooser->setNrOfColors(6);
    setNumberOfColors(6);
    updateStatusBar(numberOfColors6);
}

void SameGame::on_action7_Colors_triggered()
{
    m_modeChooser->setNrOfColors(7);
    setNumberOfColors(7);
    updateStatusBar(numberOfColors7);
}

void SameGame::on_action8_Colors_triggered()
{
    m_modeChooser->setNrOfColors(8);
    setNumberOfColors(8);
    updateStatusBar(numberOfColors8);
}

void SameGame::on_action_About_triggered()
{
    if(textwindow_about == NULL)
    {
        textwindow_about = new TextWindow(this);
        textwindow_about->setTitle("About");
        textwindow_about->setText(":/resources/text/about");
    }
    textwindow_about->show();
}

void SameGame::on_actionAbout_this_Game_triggered()
{
    if(textwindow_howto == NULL)
    {
        textwindow_howto = new TextWindow(this);
        textwindow_howto->setTitle("How to play");
        textwindow_howto->setText(":/resources/text/howto");
    }
    textwindow_howto->show();
}
