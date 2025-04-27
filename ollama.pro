QT += core gui widgets network

TARGET = ollama_qt
TEMPLATE = app

SOURCES += main.cpp \
           ollama.cpp

HEADERS += ollama.h

# 修改！直接加到编译参数而不是INCLUDEPATH
QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS += $$system(python3-config --includes)
LIBS += $$system(python3-config --ldflags)
