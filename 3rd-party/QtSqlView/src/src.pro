# $Id: src.pro 17 2006-10-09 16:59:35Z tb $

TEMPLATE = app

CONFIG += release warn_on thread qt
QT += sql

TARGET = QtSqlView

FORMS += WMain.ui WConnection.ui

HEADERS += WMain.h WConnection.h DbConnection.h DbTypes.h SQLHighlighter.h

SOURCES += WMain.cpp SQLHighlighter.cpp

RESOURCES = QtSqlView.qrc

RC_FILE = resources.rc
