QT       += core gui openglwidgets
QT += opengl

CONFIG += c++14
LIBS += -lopengl32
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tp02
TEMPLATE = app

SOURCES += main.cpp\
        banc.cpp \
        obstacle.cpp \
        poisson.cpp \
        predateur.cpp \
        princ.cpp \
        glarea.cpp \
        quadtree.cpp

HEADERS  += princ.h \
        banc.h \
        glarea.h \
        obstacle.h \
        poisson.h \
        predateur.h \
        quadtree.h

FORMS    += princ.ui

RESOURCES += \
    tp02.qrc
