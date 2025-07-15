QT += core gui widgets network

CONFIG += c++17 warn_on release link_pkgconfig

TEMPLATE = app
TARGET = SeaBattle

linux {
    QMAKE_CXXFLAGS += -O3 -march=native -pipe -fPIC
    QMAKE_LFLAGS += -Wl,--as-needed
    LIBS += -lpthread
    PKGCONFIG += libnotify

    # Только если действительно используешь X11 напрямую:
    # LIBS += -lX11 -lXext
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    secondwindow.cpp \
    shipplacementwindow.cpp \
    battlewindow.cpp \
    multiplayermodewindow.cpp \
    hostsetupwindow.cpp \
    hostlobbywindow.cpp \
    connect_window.cpp \
    gameserver.cpp \
    gameclient.cpp

HEADERS += \
    mainwindow.h \
    secondwindow.h \
    shipplacementwindow.h \
    battlewindow.h \
    multiplayermodewindow.h \
    hostsetupwindow.h \
    hostlobbywindow.h \
    connect_window.h \
    gameserver.h \
    gameclient.h

FORMS += \
    mainwindow.ui \
    secondwindow.ui \
    shipplacementwindow.ui \
    battlewindow.ui \
    multiplayermodewindow.ui \
    hostsetupwindow.ui \
    hostlobbywindow.ui \
    connect_window.ui

RESOURCES += resources.qrc  # если есть иконки и т.п.

# Установка для Linux
unix:!android {
    target.path = /usr/local/bin
    INSTALLS += target

    desktop.files = sea-battle.desktop
    desktop.path = /usr/share/applications
    INSTALLS += desktop

    icon.files = icons/app_icon.png
    icon.path = /usr/share/pixmaps
    INSTALLS += icon
}
