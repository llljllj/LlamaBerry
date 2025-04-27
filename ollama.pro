QT += core gui widgets network

TARGET = ollama_qt
TEMPLATE = app

SOURCES += main.cpp \
           ollama.cpp

HEADERS += ollama.h

QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS += $$system(python3-config --includes)
LIBS += $$system(python3-config --ldflags)
