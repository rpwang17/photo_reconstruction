include(../common.pri)
TARGET = retrospective_correction

SOURCES += \
    DialogSelectFolder.cpp \
    DialogWelcome.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    DialogSelectFolder.h \
    DialogWelcome.h \
    MainWindow.h 

FORMS += \
    DialogSelectFolder.ui \
    DialogWelcome.ui \
    MainWindow.ui

RESOURCES += \
  registration.qrc

