// $Id: WMain.cpp 20 2006-10-10 08:24:36Z tb $
// Copyright 2006 Timo Bingmann
//
// The main application loader and some DbConnection functions that couldnt go
// into the header and after too short to start an extra file.
//

#include <QtCore/QDebug>

#include "WMain.h"

#include <QtGui/QApplication>

int main(int argc, char **argv)
{
    QApplication a( argc, argv );

    WMain wmain;
    wmain.show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    
    return a.exec();
}

QSqlError DbConnection::connect(DbList &dblist)
{
    assert(!dbuuid.isNull());

    if (!QSqlDatabase::isDriverAvailable(dbparam.driver))
    {
	QSqlError e = QSqlError("Could not connect to database",
				QString("Database driver %1 is not available.").arg(dbparam.driver),
				QSqlError::ConnectionError);
	dblist.tablelist_seterror(*this, e);
	return e;
    }

    db = QSqlDatabase::addDatabase(dbparam.driver, dbuuid);
	
    db.setHostName(dbparam.hostname);
    if (dbparam.port > 0) db.setPort(dbparam.port);
    db.setDatabaseName(dbparam.database);
    db.setUserName(dbparam.username);

    if (dbparam.askpassword)
    {
	bool ok;
	QString passwd = QInputDialog::getText(NULL, "QtSqlView Password Prompt",
					       QString("Enter password for '%1':").arg(dbparam.label),
					       QLineEdit::Password, QString::null, &ok);

	if (!ok) {
	    QSqlError e = QSqlError("Could not connect to database",
				    "Password prompt failed.",
				    QSqlError::ConnectionError);
	    dblist.tablelist_seterror(*this, e);
	    return e;
	}
    }
    else {
	db.setPassword(dbparam.password);
    }
	
    if (!db.open()) {
	QSqlError e = db.lastError();
	db = QSqlDatabase();
	QSqlDatabase::removeDatabase(dbuuid);
	dblist.tablelist_seterror(*this, e);
	return e;
    }

    dblist.tablelist_load(*this);

    return QSqlError();
}

void DbConnection::disconnect(class DbList &dblist)
{
    if (db.isOpen())
    {
	db.close();
	db = QSqlDatabase();

	QSqlDatabase::removeDatabase(dbuuid);
    }
    dblist.tablelist_clear(*this);
}

