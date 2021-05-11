QT       += core gui

QMAKE_CXXFLAGS += -std=c++17

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 \
    config

win32:RC_ICONS += cloud.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    createuser.cpp \
    files.cpp \
    ipdialog.cpp \
    listwidgetdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    userworkspace.cpp \
    utils.cpp

HEADERS += \
    createuser.h \
    files.h \
    ipdialog.h \
    listwidgetdialog.h \
    mainwindow.h \
    userworkspace.h \
    utils.h

FORMS += \
    createuser.ui \
    ipdialog.ui \
    listwidgetdialog.ui \
    mainwindow.ui \
    userworkspace.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -lcurl

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += openssl

RESOURCES += \
    resources.qrc

win32: INCLUDEPATH += $$PWD/../../../include
win32: DEPENDPATH += $$PWD/../../../include
win32: LIBS += -L$$PWD/../../../lib/ -lcurl
win32: LIBS += -L$$PWD/../../../lib/ -lcrypto
win32: LIBS += -L$$PWD/../../../lib/ -lssl
