TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    parser.cpp

LIBS += -lglut -lGLU -lGL -lGLEW

QMAKE_CFLAGS += -std=c11

HEADERS += \
    parser.h
