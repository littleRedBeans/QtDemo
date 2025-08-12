QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network charts sql

CONFIG += c++17
DESTDIR = $$_PRO_FILE_PWD_/../bin
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.h

FORMS += \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include($$_PRO_FILE_PWD_/Network/Network.pri)
include($$_PRO_FILE_PWD_/UI/UI.pri)
include($$_PRO_FILE_PWD_/Database/Database.pri)
include($$_PRO_FILE_PWD_/Model/Model.pri)
