// $Id: WMain.cpp 20 2006-10-10 08:24:36Z tb $
// Copyright 2006 Timo Bingmann
//
// The main application loader and some DbConnection functions that couldnt go
// into the header and after too short to start an extra file.
//

#include <QtCore/QDebug>

#include "WMain.h"

#include <QApplication>

#include <assert.h>

QSqlError DbConnection::connect (DbList &dblist) {
  assert (!dbuuid.isNull ());

  if (!QSqlDatabase::isDriverAvailable (dbparam.driver)) {
    QSqlError e = QSqlError (tr ("Could not connect to database"),
                             tr ("Database driver %1 is not available.").arg (dbparam.driver),
                             QSqlError::ConnectionError);
    dblist.tablelist_seterror (*this, e);
    return e;
  }

  db = QSqlDatabase::addDatabase (dbparam.driver, dbuuid.toString ());

  db.setHostName (dbparam.hostname);
  if (dbparam.port > 0) {
    db.setPort (dbparam.port);
  }
  db.setDatabaseName (dbparam.database);
  db.setUserName (dbparam.username);
  db.setConnectOptions (dbparam.connectOptions);

  if (dbparam.askpassword && dbparam.password.isEmpty ()) {
    bool ok;
    QString passwd = QInputDialog::getText (NULL, tr ("QtSqlView Password Prompt"),
                                            tr ("Enter password for '%1':").arg (dbparam.label),
                                            QLineEdit::Password, QString::null, &ok);

    if (!ok) {
      QSqlError e = QSqlError (tr ("Could not connect to database"),
                               tr ("Password prompt failed."),
                               QSqlError::ConnectionError);
      dblist.tablelist_seterror (*this, e);
      return e;
    }
    dbparam.password = passwd;
  }
  db.setPassword (dbparam.password);

  if (!db.open ()) {
    QSqlError e = db.lastError ();
    db = QSqlDatabase ();
    QSqlDatabase::removeDatabase (dbuuid.toString ());
    dblist.tablelist_seterror (*this, e);
    return e;
  }

  dblist.tablelist_load (*this);

  return QSqlError ();
}

void DbConnection::disconnect (class DbList &dblist) {
  if (db.isOpen ()) {
    db.close ();
    db = QSqlDatabase ();

    QSqlDatabase::removeDatabase (dbuuid.toString ());
  }
  dblist.tablelist_clear (*this);
}

