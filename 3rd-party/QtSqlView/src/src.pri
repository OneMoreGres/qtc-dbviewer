QT += sql
INCLUDEPATH += $$PWD
VPATH += $$PWD
FORMS += WMain.ui WConnection.ui
HEADERS += WMain.h WConnection.h DbConnection.h DbTypes.h SQLHighlighter.h
SOURCES += WMain.cpp SQLHighlighter.cpp
RESOURCES = QtSqlView.qrc
