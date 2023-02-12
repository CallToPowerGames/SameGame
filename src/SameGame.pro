QT += core gui widgets multimedia

TARGET = SameGame
TEMPLATE = app


SOURCES += hcpp/main.cpp\
        hcpp/samegame.cpp\
        hcpp/clickablelabel.cpp \
    hcpp/modechooser.cpp \
    hcpp/textwindow.cpp

HEADERS  += hcpp/samegame.h\
        hcpp/clickablelabel.h \
    hcpp/modechooser.h \
    hcpp/textwindow.h

FORMS    += ui/samegame.ui \
    ui/modechooser.ui \
    ui/textwindow.ui

RESOURCES += \
    resources/resources.qrc

win32 {
	RC_ICONS = samegame.ico
} else:macx {
    ICON = samegame.icns
    FILETYPES.files = samegame.icns
    FILETYPES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += FILETYPES
} else {
}
