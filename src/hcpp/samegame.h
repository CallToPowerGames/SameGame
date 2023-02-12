/**
 * SameGame
 * Copyright 2014 Denis Meyer
 * https://sites.google.com/site/calltopowersoftware/software
 */

#ifndef SAMEGAME_H
#define SAMEGAME_H

#include <QMainWindow>
#include <QString>
#include <QVector>
#include <QLabel>
#include <QMap>
#include <QTimer>
#include <QMediaPlayer>
#include <QResizeEvent>
#include "clickablelabel.h"
#include "modechooser.h"
#include "textwindow.h"

/**
  * Namespace UI
  */
namespace Ui {
class SameGame;
}

class ModeChooser;

/**
  * Class SameGame <- QMainWindow
  */
class SameGame : public QMainWindow
{
    Q_OBJECT

public:
    /**
      * Constructor
      * @parent Parent Widget
      * @parent fieldSize The Field Size, default = 10, must be >= 10 and <= 30
      */
    explicit SameGame(QWidget *parent = 0, int fieldSize = 10);

    /**
      * Destructor
      */
    ~SameGame();

    /**
      * Sets the Mode
      * @param modeNormal Normal Mode or easy Mode
      */
    void setMode(bool modeNormal);

    /**
      * Sets the Style
      * @param modeImages Images or colored Backgrounds
      */
    void setStyle(bool modeImages);

    /**
      * Sets the Field Size
      * @param fieldSize The Field Size
      */
    void setFieldSize(int fieldSize);

    /**
      * Sets the Number of Colors in [3, 8]
      * @param number Number of Colors to set
      * @return true if set successfully, false else
      */
    bool setNumberOfColors(const int number);

    /**
      * Sets up the Start
      */
    void setupStart();

    /**
      * Cancels the Start
      */
    void cancelStart();

private:
    // The UI
    Ui::SameGame *ui;
    // Initial ClickableLabel
    ClickableLabel *m_initialCl;
    // The ModeChooser
    ModeChooser *m_modeChooser;
    // The Field Size
    int m_fieldSize;
    // The new Field Size
    int m_newFieldSize;
    // Statusbar Label
    QLabel *m_labelInformation;
    // Color Array
    QVector<QString> m_colors;
    //Label Vector
    QVector<ClickableLabel*> m_labels;
    // Label Backup for Undo
    QVector<ClickableLabel*> m_labelsBackup;
    // Backup for deleted Blocks
    int m_blocksDestroyedBackup;
    // Map for the ClickableLabels to delete
    QMap<int, ClickableLabel*> m_map_del;
    // Flag if a Game currently runs
    bool m_gameRunning;
    // Flag whether Game mode is set to normal or not
    bool m_modeNormal;
    // Flag whether to use Image or Label Background Colors
    bool m_modeImages;
    // Number of Colors
    int m_numberOfColors;
    // Seconds
    int m_seconds;
    // Timer
    QTimer *m_timer;
    // Text windows
    TextWindow *textwindow_about;
    TextWindow *textwindow_howto;
    // Counter for destroyed Blocks
    int m_blocksDestroyed;
    QMediaPlayer* player_applause;
    QMediaPlayer* player_dink;
    QMediaPlayer* player_game_over;
    QMediaPlayer* player_gong;
    QMediaPlayer* player_undo;
    bool player_applause_loaded;
    bool player_dink_loaded;
    bool player_game_over_loaded;
    bool player_gong_loaded;
    bool player_undo_loaded;

    /**
      * Returns the current Directory
      */
    QString getCurrentDir();

    /**
      * Plays the 'Game over'-Sound
      */
    void playGameOverSound();

    /**
      * Plays the 'Dink'-Sound
      */
    void playDinkSound();

    /**
      * Plays the 'Applause'-Sound
      */
    void playApplauseSound();

    /**
      * Plays the 'Undo'-Sound
      */
    void playUndoSound();

    /**
      * Calculates the Score
      */
    int getScore(int blocksDeleted);

    /**
      * Initializes the Statusbar
      */
    void setUpStatusbar();

    /**
      * Updates the Statusbar
      * @param statusUpdate Message to update the Statusbar
      */
    void updateStatusBar(const QString statusUpdate);

    /**
      * Calculates the Position of (x, y) in the Field
      * @param x x-Coordinate
      * @param y y-Coordinate
      * @return the Position of (x, y) in the Field
      */
    int to1Dimension(const int x, const int y) const;

    /**
      * Calculates the Position of a Field in (x, y)
      * @param n Number in 1D
      * @param x Storage for x-Coordinate
      * @param y Storage for y-Coordinate
      */
    void to2Dimension(const int n, int &x, int &y) const;

    /**
      * Initializes the Labels
      */
    void initLabels();

    /**
      * Initializes the Labels
      */
    void delLabels();

    /**
      * Enables/Disables the Labels and Disables/Enables the Options
      */
    void enableLabels();

    /**
      * Connects the Labels to the "clicked"-Slot
      */
    void connectLabels();

    /**
      * Returns whether Labels have been set
      * @return true if Labels have been set, false else
      */
    bool labelsSet() const;

    /**
      * Quits the Application
      */
    void quitApplication();

    /**
      * Checks of 2 given Colors are equal - Case insensitively!
      * @param First Color
      * @param Second Color
      * @return true if Colors are equal, false else
      */
    bool colorEqual(const QString &s1, const QString &s2) const;

    /**
      * Style the Label label with its own Color
      * @param label Label to style
      * @return true if styled, false else
      */
    bool styleLabel(ClickableLabel *label) const;

    /**
      * Style the Label label with a given Color
      * @param label Label to style
      * @param color The Color to style the Label
      * @return true if styled, false else
      */
    bool styleLabel(ClickableLabel *label, const QString color) const;

    /**
      * Checks the North Side
      * @param fieldNumber Number of the Field to check
      * @param insert true if Field should be inserted into the Map, false else
      * @return true if a Field of the own Color has been detected
      */
    bool checkNorth(const int fieldNumber, const bool insert);

    /**
      * Checks the North East Side
      * @param fieldNumber Number of the Field to check
      * @param insert true if Field should be inserted into the Map, false else
      * @return true if a Field of the own Color has been detected
      */
    bool checkNorthEast(const int fieldNumber, const bool insert);

    /**
      * Checks the East Side
      * @param fieldNumber Number of the Field to check
      * @param insert true if Field should be inserted into the Map, false else
      * @return true if a Field of the own Color has been detected
      */
    bool checkEast(const int fieldNumber, const bool insert);

    /**
      * Checks the South East Side
      * @param fieldNumber Number of the Field to check
      * @param insert true if Field should be inserted into the Map, false else
      * @return true if a Field of the own Color has been detected
      */
    bool checkSouthEast(const int fieldNumber, const bool insert);

    /**
      * Checks the South Side
      * @param fieldNumber Number of the Field to check
      * @param insert true if Field should be inserted into the Map, false else
      * @return true if a Field of the own Color has been detected
      */
    bool checkSouth(const int fieldNumber, const bool insert);

    /**
      * Checks the South West Side
      * @param fieldNumber Number of the Field to check
      * @param insert true if Field should be inserted into the Map, false else
      * @return true if a Field of the own Color has been detected
      */
    bool checkSouthWest(const int fieldNumber, const bool insert);

    /**
      * Checks the West Side
      * @param fieldNumber Number of the Field to check
      * @param insert true if Field should be inserted into the Map, false else
      * @return true if a Field of the own Color has been detected
      */
    bool checkWest(const int fieldNumber, const bool insert);

    /**
      * Checks the North West Side
      * @param fieldNumber Number of the Field to check
      * @param insert true if Field should be inserted into the Map, false else
      * @return true if a Field of the own Color has been detected
      */
    bool checkNorthWest(const int fieldNumber, const bool insert);

    /**
      * Checks all Fields around the Field with the Number number
      * and inserts, if insert is true, these Fields into the Map (to delete)
      * @param number Field with the Number number check
      * @param insert true if Fields should be inserted into the Map, false else
      * @return true if any of the Checks has been true, false else
      */
    bool checkAll(const int number, const bool insert);

    /**
      * Inserts the Label with the Number number into the Map (to delete)
      * @param number Number of the Field to insert into the Map
      * @return true if inserted (uniquely), false else
      */
    bool insertIntoMap(int number);

    /**
      * Loop after a click, loops until nothing changes
      * @param number Field Number to start with
      */
    void loop(const int number);

    /**
      * Checks if all Fields are grey (-> Player wins the Game)
      * @return true if all Fields are grey, false else
      */
    bool allGrey() const;

    /**
      * Checks if the Game is over
      * @return true if the Game is over, false else
      */
    bool gameOver();

    /**
      * Calculates the destroyed Blocks
      */
    void setDestroyedBlocks();

    /**
      * Returns if Label label is greyed out
      * @param label Label to check
      * @return true if Label is greyed out, false else
      */
    bool isGrey(const ClickableLabel* label) const;

    /**
      * Switches 2 Labels (not the Number!)
      * @param number1 Number of Label 1
      * @param number2 Number of Label 2
      */
    void switchLabels(int number1, int number2);

    /**
      * Filles up grey free Room
      * 1. From the top to the Bottom
      * 2. From the right Side to the left Side
      */
    void fillupLines();

    /**
      * Switches the Mode from Normal to easy or vice versa
      */
    void switchMode();

    /**
      * Switches the Image Mode from using Image to using Background Colors and vice versa
      */
    void switchImageMode();

    /**
      * Starts and/or stops a new Game
      * @param Ask for a new Game or not
      */
    void startStopGame(bool ask);

    /**
      * Event if Window resized
      * @param e QResizeEvent
      */
    void resizeEvent(QResizeEvent* e)
    {
        emit resized();
    }

    /*****************************************************************************
      * Signals
      ***************************************************************************/

signals:
    /**
      * Signal if Window has been resized
      */
    void resized();

    /*****************************************************************************
      * Slots
      ***************************************************************************/

private slots:
    /**
     * When media has been loaded
     * @param status
     */
    void play_applause(QMediaPlayer::MediaStatus status);
    void play_dink(QMediaPlayer::MediaStatus status);
    void play_game_over(QMediaPlayer::MediaStatus status);
    void play_gong(QMediaPlayer::MediaStatus status);
    void play_undo(QMediaPlayer::MediaStatus status);

    /**
      * Plays the 'Gong'-Sund
      */
    void playGongSound();

    /**
      * Slot for Initial ClickableLabel clicked
      */
    void initialClickableLabelClicked();

    /**
      * Slot for Initial ClickableLabel reset
      */
    void initialClickableLabelReset();

    /**
      * Slot for Initial ClickableLabel remove
      */
    void initialClickableLabelRemove();

    /**
      * Slot for Window resized
      */
    void windowResized();

    /**
      * Updates the Statusbar with Time Time
      */
    void updateTime();

    /**
      * Updates the Statusbar without the Time
      */
    void updateWithoutTime();

    /**
      * Checks the Field
      * @param number Number of the clicked Field
      */
    void checkField(const int number);

    /**
      * "New Game"-Action
      */
    void on_actionNew_Game_triggered();

    /**
      * "Undo"-Action
      */
    void on_actionUndo_triggered();

    /**
      * "Mode Normal"-Action
      */
    void on_action_ModeNormal_triggered();

    /**
      * "Mode Easy"-Action
      */
    void on_action_ModeEasy_triggered();

    /**
      * "Mode One Color"-Action
      */
    void on_action_One_Color_triggered();

    /**
      * "Mode Images"-Action
      */
    void on_action_Images_triggered();

    /**
      * "Mode 3 Colors"-Action
      */
    void on_action3_Colors_triggered();

    /**
      * "Mode 4 Colors"-Action
      */
    void on_action4_Colors_triggered();

    /**
      * "Mode 5 Colors"-Action
      */
    void on_action5_Colors_triggered();

    /**
      * "Mode 6 Colors"-Action
      */
    void on_action6_Colors_triggered();

    /**
      * "Mode 7 Colors"-Action
      */
    void on_action7_Colors_triggered();

    /**
      * "Mode 8 Colors"-Action
      */
    void on_action8_Colors_triggered();

    /**
      * "About"-Action
      */
    void on_action_About_triggered();

    /**
      * "About this Game"-Action
      */
    void on_actionAbout_this_Game_triggered();
};

#endif // SAMEGAME_H
