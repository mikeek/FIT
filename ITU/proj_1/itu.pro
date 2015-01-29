TEMPLATE = app

QT += qml quick widgets

CONFIG += c++11

SOURCES += main.cpp \
    KeyTranslator.cpp \
    Trie.cpp \
    TrieNode.cpp \
    TextHandler.cpp \
    NormalInput.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    KeyTranslator.h \
    Trie.h \
    TrieNode.h \
    TextHandler.h \
    NormalInput.h
