# Qt Creator SQL database viewer plugin

![build status](https://github.com/OneMoreGres/qtc-dbviewer/workflows/Plugin%20build/badge.svg)

## Introduction
Plugin allows to view databases, supported by QtSql module.

![Screen](util/screen.png?raw=true)

## Features
1. Add several database connections with different drivers or connection options.
2. Browse tables list, table's data and schema.
3. Execute sql query.
4. Copy table's or query's contents to clipboard.

## Acknowledgements
Plugin based on Timo Bingmann's <tb@idlebox.net> [QtSqlView application](http://idlebox.net/2006/qtsqlview/)

## Downloads
Binaries can be downloaded from [github releases](https://github.com/OneMoreGres/qtc-dbviewer/releases).


## Installation
IMPORTANT: plugin's version must match Qt Creator's version (difference in last digit is acceptable)

### From source
1. Change paths.pri:

 - set `QTCREATOR_SOURCES` = path to Qt Creator source dir (with qtcreator.pro)
 - set `IDE_BUILD_TREE` = path to compiled Qt Creator dir (with bin,lib,libexec,...)

2. Compile plugin.

### From binaries
1. Extract/copy files from archive into Qt Creator's dir (archive already contains proper paths).
2. Enable the `QtcDbViewer` plugin (Help → About Plugins...)


## More info
Additional information can be obtained [here](http://gres.biz/qtc-dbviewer/ "Homepage") (in russian)
