QT       += core gui serialport multimedia multimediawidgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camerawidget.cpp \
    camerawindow.cpp \
    datadisplay.cpp \
    datadisplaytext.cpp \
    datahandler.cpp \
    datatabledialog.cpp \
    dialog.cpp \
    gauss.cpp \
    main.cpp \
    mainwindow.cpp \
    table.cpp \
    table_termo.cpp \
    tablechart.cpp \
    tablechartmlx.cpp

HEADERS += \
    camerawidget.h \
    camerawindow.h \
    datadisplay.h \
    datadisplaytext.h \
    datahandler.h \
    datatabledialog.h \
    dialog.h \
    gauss.h \
    mainwindow.h \
    table.h \
    table_termo.h \
    tablechart.h \
    tablechartmlx.h

FORMS += \
    camerawindow.ui \
    dialog.ui \
    mainwindow.ui \
    table.ui \
    table_termo.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rsc.qrc

TRANSLATIONS = translations/app_en.ts translations/app_pl.ts
