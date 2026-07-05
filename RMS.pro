QT += core gui widgets sql

CONFIG += c++17 warn_on

TARGET = RMS
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/ui/loginwindow.cpp \
    src/ui/admindashboard.cpp \
    src/ui/userdashboard.cpp \
    src/services/database.cpp \
    src/services/validation.cpp \
    src/data_structures/linkedlist.cpp \
    src/data_structures/menu_linkedlist.cpp

HEADERS += \
    src/ui/loginwindow.h \
    src/ui/admindashboard.h \
    src/ui/userdashboard.h \
    src/services/database.h \
    src/services/validation.h \
    src/data_structures/linkedlist.h \
    src/data_structures/menu_linkedlist.h \
    src/models/menuitem.h \
    src/models/order.h \
    src/models/user.h

INCLUDEPATH += \
    src \
    src/ui \
    src/services \
    src/models \
    src/data_structures

OTHER_FILES += README.md