QT       += core core5compat gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    algorithms.cpp \
    exportdialog.cpp \
    formgenerator.cpp \
    graphpreprocess.cpp \
    imageaxis.cpp \
    imagepoint.cpp \
    imagepreprocess.cpp \
    imageview.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    algorithms.h \
    exportdialog.h \
    formgenerator.h \
    graphpreprocess.h \
    imageaxis.h \
    imagepoint.h \
    imagepreprocess.h \
    imageview.h \
    mainwindow.h

FORMS += \
    exportdialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
