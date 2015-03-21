// $Id: DbConnection.h 20 2006-10-10 08:24:36Z tb $
// Copyright 2006 Timo Bingmann
//
// Contains the classes for the database connection listing, the subtrees
// showing the containing tables and the schema display model.
//

#ifndef _DbConnection_H_
#define _DbConnection_H_

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>
#include <QtSql/QSqlIndex>
#include <QtSql/QSqlQuery>

#include <QtGui/QIcon>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QVector>
#include <QtCore/QSettings>
#include <QtCore/QUuid>

#include "DbTypes.h"

#include <coreplugin/icore.h>

struct DbParameter {
  QString label;
  QString hostname;
  int port;
  QString driver;
  QString username;
  QString password;
  int askpassword;
  QString database;
  int showsystables;
  QString connectOptions;

  void saveToSettings (QSettings &settings) const {
    settings.setValue (QStringLiteral ("label"), label);
    settings.setValue (QStringLiteral ("hostname"), hostname);
    settings.setValue (QStringLiteral ("port"), port);
    settings.setValue (QStringLiteral ("driver"), driver);
    settings.setValue (QStringLiteral ("username"), username);
    if (!askpassword) {
      settings.setValue (QStringLiteral ("password"), password);
    }
    settings.setValue (QStringLiteral ("askpassword"), askpassword);
    settings.setValue (QStringLiteral ("database"), database);
    settings.setValue (QStringLiteral ("showsystables"), showsystables);
    settings.setValue (QStringLiteral ("connectoptions"), connectOptions);
  }

  void loadFromSettings (QSettings &settings) {
    label = settings.value (QStringLiteral ("label")).toString ();
    hostname = settings.value (QStringLiteral ("hostname")).toString ();
    port = settings.value (QStringLiteral ("port"), 0).toUInt ();
    driver = settings.value (QStringLiteral ("driver")).toString ();
    username = settings.value (QStringLiteral ("username")).toString ();
    password = settings.value (QStringLiteral ("password")).toString ();
    askpassword = settings.value (QStringLiteral ("askpassword"), 0).toUInt ();
    database = settings.value (QStringLiteral ("database")).toString ();
    showsystables = settings.value (QStringLiteral ("showsystables"), 0).toUInt ();
    connectOptions = settings.value (QStringLiteral ("connectoptions")).toString ();
  }
};

class DbTable : public QObject {
  Q_OBJECT

  public:
    /// link to parent connection object
    class DbConnection *dbconn;

    /// guess
    QString tablename;

    /// 0 = user table, 1 = view, 2 = system table
    int tabletype;

    inline DbTable (class DbConnection *dbc, const QString &tab, int _tabletype)
      : QObject (), dbconn (dbc), tablename (tab), tabletype (_tabletype) {
    }
};

class DbError : public QObject, public QSqlError {
  Q_OBJECT

  public:
    /// link to parent connection object
    class DbConnection *dbconn;

    DbError (class DbConnection *_dbconn)
      : QObject (), QSqlError (), dbconn (_dbconn) {
    }

    DbError (const QSqlError &e)
      : QObject (), QSqlError (e) {
    }

    DbError &operator= (const QSqlError &e) {
      *(QSqlError *)this = e;
      return *this;
    }
};

class DbConnection : public QObject {
  Q_OBJECT

  public:
    /// parameters of the connection
    DbParameter dbparam;

    /// because qt requires us to use addDatabase with a unique id, we use the
    /// only random function aviablable in qt itself here.
    QUuid dbuuid;

    QSqlDatabase db;

    DbError connecterror;

    DbConnection (const DbParameter &params)
      : QObject (), dbparam (params),
      dbuuid (QUuid::createUuid () ),
      connecterror (this) {
    }

    QSqlError connect (class DbList &dblist);

    void disconnect (class DbList &dblist);

    typedef QVector<DbTable *> tablelist_t;

    QVector<DbTable *> tablelist;

    inline int numChildren () const {
      if (db.isOpen ()) {
        return tablelist.size ();
      }
      else if (connecterror.isValid ()) {
        return 2;
      }
      else{
        return 0;
      }
    }
};

class DbList : public QAbstractItemModel {
  Q_OBJECT

  public:
    inline ~DbList () {
      clear ();
    }

    /// the primary list of data connections
    QVector<DbConnection *>  list;

    /// correctly delete all dbconnections
    void clear () {
      if (!list.isEmpty ()) {
        // first delete children, then the db entries. otherwise the model
        // goes crazy
        Q_FOREACH (DbConnection * dbc, list) {
          dbc->disconnect (*this);
        }

        beginRemoveRows (QModelIndex (), 0, list.size () - 1);

        Q_FOREACH (DbConnection * dbc, list) {
          delete dbc;
        }
        list.clear ();

        endRemoveRows ();
      }
    }

    /// add a new dbconnection
    void addDbConnection (DbParameter &dbp) {
      beginInsertRows (QModelIndex (), list.size (), list.size ());

      list.push_back (new DbConnection (dbp) );

      endInsertRows ();
    }

    /// change a dbconnection
    void editDbConnection (int num, DbParameter &dbp) {
      if (0 > num || num >= list.size ()) {
        return;
      }

      // close old connection and change parameters
      list[num]->disconnect (*this);

      list[num]->dbparam = dbp;

      emit dataChanged (createIndex (num, 0, list[num]),
                        createIndex (num, 0, list[num]));
    }

    /// remove a dbconnection
    void delDbConnection (int num) {
      if (0 > num || num >= list.size ()) {
        return;
      }

      beginRemoveRows (QModelIndex (), num, num);

      list[num]->disconnect (*this);
      delete list[num];

      list.remove (num);

      endRemoveRows ();
    }

    /// save all database parameters to settings
    void saveToSettings () {
      Q_ASSERT (Core::ICore::settings () != NULL);
      QSettings &settings = *(Core::ICore::settings ());

      settings.beginWriteArray (QStringLiteral ("connections"));
      int i = 0;
      Q_FOREACH (const DbConnection * dbc, list) {
        settings.setArrayIndex (i++);

        dbc->dbparam.saveToSettings (settings);
      }
      settings.endArray ();
    }

    /// load all database parameters from settings
    void loadFromSettings () {
      Q_ASSERT (Core::ICore::settings () != NULL);
      QSettings &settings = *(Core::ICore::settings ());

      clear ();

      int connnum = settings.beginReadArray (QStringLiteral ("connections"));
      for (int i = 0; i < connnum; ++i) {
        settings.setArrayIndex (i);

        DbParameter dbp;
        dbp.loadFromSettings (settings);

        list.push_back (new DbConnection (dbp) );
      }
      settings.endArray ();
    }

  public: // *** implements QAbstractItemModel

    // *** Columns

    virtual int columnCount (const QModelIndex &) const {
      return 1;
    }

    virtual QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const {
      if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
          switch (section) {
            case 0:
              return tr ("Connections");
            default:
              return tr ("Column %1").arg (section);
          }
        }
        else{
          return QVariant ();
        }
      }
      else{
        if (role != Qt::DisplayRole) {
          return QVariant ();
        }

        return tr ("Row %1").arg (section);
      }
    }

    // *** Rows

    virtual int rowCount (const QModelIndex &index) const {
      if (!index.isValid ()) {
        return list.size ();
      }
      else{
        QObject *obj = (QObject *)index.internalPointer ();

        if (const DbConnection *dbc = qobject_cast<const DbConnection *>(obj)) {
          return dbc->numChildren ();
        }
      }

      return 0;
    }

    virtual bool hasChildren (const QModelIndex &parent = QModelIndex ()) const {
      if (!parent.isValid ()) {
        return true;
      }

      QObject *obj = (QObject *)parent.internalPointer ();

      if (const DbConnection *dbc = qobject_cast<const DbConnection *>(obj)) {
        dbc++;
        return true;
      }

      return false;
    }

    virtual QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const {
      if (!index.isValid ()) {
        return QVariant ();
      }

      QObject *obj = (QObject *)index.internalPointer ();

      if (const DbConnection *dbc = qobject_cast<const DbConnection *>(obj)) {
        if (role == Qt::DisplayRole) {
          if (dbc->dbparam.label.isEmpty ()) {
            return tr ("<no label>");
          }

          return dbc->dbparam.label;
        }
        else if (role == Qt::DecorationRole) {
          static QIcon dbicon (QStringLiteral (":/img/database.png"));
          return qVariantFromValue (dbicon);
        }
      }
      else if (const DbTable *dbt = qobject_cast<const DbTable *>(obj)) {
        if (role == Qt::DisplayRole) {
          return dbt->tablename;
        }
        else if (role == Qt::DecorationRole) {
          static QIcon tableicon (QStringLiteral (":/img/table.png"));
          static QIcon tablesysicon (QStringLiteral (":/img/tablesys.png"));
          static QIcon viewicon (QStringLiteral (":/img/view.png"));

          if (dbt->tabletype == 0) {
            return qVariantFromValue (tableicon);
          }
          else if (dbt->tabletype == 1) {
            return qVariantFromValue (viewicon);
          }
          else if (dbt->tabletype == 2) {
            return qVariantFromValue (tablesysicon);
          }
        }
      }
      else if (const DbError *err = qobject_cast<const DbError *>(obj)) {
        if (role == Qt::DisplayRole) {
          if (index.row () == 0) {
            return err->driverText ();
          }
          else if (index.row () == 1) {
            return err->databaseText ();
          }
        }
        else if (role == Qt::DecorationRole) {
          static QIcon erroricon (QStringLiteral (":/img/error.png"));
          return qVariantFromValue (erroricon);
        }
      }

      return QVariant ();
    }

    virtual QModelIndex index (int row, int column, const QModelIndex &parent = QModelIndex ()) const {
      if (!parent.isValid ()) {
        if (row < list.size ()) {
          return createIndex (row, column, list[row]);
        }
      }
      else{
        QObject *obj = (QObject *)parent.internalPointer ();

        if (const DbConnection *dbc = qobject_cast<const DbConnection *>(obj)) {
          if (dbc->db.isOpen ()) {
            if (row < dbc->tablelist.size ()) {
              return createIndex (row, column, dbc->tablelist[row]);
            }
          }
          else if (dbc->connecterror.isValid ()) {
            return createIndex (row, column, (void *)&dbc->connecterror);
          }
        }
      }
      return QModelIndex ();
    }

    virtual QModelIndex parent (const QModelIndex &index) const {
      if (!index.isValid ()) {
        return QModelIndex ();
      }

      QObject *obj = (QObject *)index.internalPointer ();

      if (const DbConnection *dbc = qobject_cast<const DbConnection *>(obj)) {
        dbc++;
        // table entries have the root as parent
        return QModelIndex ();
      }
      else if (const DbTable *dbt = qobject_cast<const DbTable *>(obj)) {
        return createIndex (list.indexOf (dbt->dbconn), 0, dbt->dbconn);
      }
      else if (const DbError *err = qobject_cast<const DbError *>(obj)) {
        return createIndex (list.indexOf (err->dbconn), 0, err->dbconn);
      }
      return QModelIndex ();
    }

    int getDbConnectionNum (const QModelIndex &index) const {
      if (!index.isValid ()) {
        return -1;
      }

      QObject *obj = (QObject *)index.internalPointer ();

      if (DbConnection *dbc = qobject_cast<DbConnection *>(obj)) {
        return list.indexOf (dbc);
      }
      else if (const DbTable *dbt = qobject_cast<const DbTable *>(obj)) {
        return list.indexOf (dbt->dbconn);
      }
      return 0;
    }

    DbConnection *getDbConnection (const QModelIndex &index) const {
      if (!index.isValid ()) {
        return NULL;
      }

      QObject *obj = (QObject *)index.internalPointer ();

      if (DbConnection *dbc = qobject_cast<DbConnection *>(obj)) {
        return dbc;
      }
      else if (DbTable *dbt = qobject_cast<DbTable *>(obj)) {
        return dbt->dbconn;
      }
      return NULL;
    }

    DbTable *getDbTable (const QModelIndex &index) const {
      if (!index.isValid ()) {
        return NULL;
      }

      QObject *obj = (QObject *)index.internalPointer ();

      if (DbTable *dbt = qobject_cast<DbTable *>(obj)) {
        return dbt;
      }
      return NULL;
    }

    void expanding (const QModelIndex &index) {
      if (!index.isValid ()) {
        return;
      }

      QObject *obj = (QObject *)index.internalPointer ();

      if (DbConnection *dbc = qobject_cast<DbConnection *>(obj)) {
        if (!dbc->db.isOpen ()) {
          dbc->connect (*this);
        }
      }
    }

    void collapsed (const QModelIndex &index) {
      if (!index.isValid ()) {
        return;
      }

      QObject *obj = (QObject *)index.internalPointer ();

      if (DbConnection *dbc = qobject_cast<DbConnection *>(obj)) {
        if (dbc->db.isOpen ()) {
          dbc->disconnect (*this);
        }
      }
    }

    void refresh () {
      Q_FOREACH (DbConnection * dbc, list) {
        if (dbc->db.isOpen ()) {
          tablelist_clear (*dbc);

          tablelist_load (*dbc);
        }
      }
    }

    // *** Tablelist Functions are located here, because they have to emit signals

    void tablelist_clear (class DbConnection &dbc) {
      if (!dbc.tablelist.isEmpty ()) {
        beginRemoveRows (createIndex (list.indexOf (&dbc), 0, &dbc),
                         0, dbc.tablelist.size () - 1);

        Q_FOREACH (DbTable * t, dbc.tablelist) {
          delete t;
        }
        dbc.tablelist.clear ();

        endRemoveRows ();
      }
      if (dbc.connecterror.isValid ()) {
        beginRemoveRows (createIndex (list.indexOf (&dbc), 0, &dbc),
                         0, 1);


        dbc.connecterror = QSqlError ();

        endRemoveRows ();
      }
    }

    void tablelist_load (class DbConnection &dbc) {
      tablelist_clear (dbc);

      if (!dbc.db.isOpen ()) {
        return;
      }

      QVector<DbTable *> newtablelist;

      QStringList tables;
      tables = dbc.db.tables ();
      tables.sort (Qt::CaseInsensitive);
      Q_FOREACH (QString table, tables) {
        newtablelist.push_back (new DbTable (&dbc, table, 0) );
      }

      tables = dbc.db.tables (QSql::Views);
      tables.sort (Qt::CaseInsensitive);
      Q_FOREACH (QString table, dbc.db.tables (QSql::Views)) {
        newtablelist.push_back (new DbTable (&dbc, table, 1) );
      }

      if (dbc.dbparam.showsystables) {
        tables = dbc.db.tables (QSql::SystemTables);
        tables.sort (Qt::CaseInsensitive);
        Q_FOREACH (QString table, tables) {
          newtablelist.push_back (new DbTable (&dbc, table, 2) );
        }
      }

      beginInsertRows (createIndex (list.indexOf (&dbc), 0, &dbc),
                       0, newtablelist.size () - 1);

      dbc.tablelist = newtablelist;

      endInsertRows ();
    }

    void tablelist_seterror (class DbConnection &dbc, QSqlError e) {
      tablelist_clear (dbc);

      beginInsertRows (createIndex (list.indexOf (&dbc), 0, &dbc),
                       0, 1);

      dbc.connecterror = e;

      endInsertRows ();
    }
};

class DbSchemaModel : public QAbstractTableModel {
  Q_OBJECT

  private:
    /// the record of which the schema is displayed
    QSqlRecord record;

    /// the database driver name
    QString driver;

    /// the primary index of the table
    QSqlIndex pindex;

  public:
    /// change the data in the model
    void setRecord (const DbTable *dbt, QSqlRecord r) {
      record = r;
      driver = dbt->dbconn->db.driverName ();
      pindex = dbt->dbconn->db.primaryIndex (dbt->tablename);

      emit layoutChanged ();
    }

  public: // *** implements QAbstractItemModel

    // *** Columns

    virtual int columnCount (const QModelIndex &) const {
      return 5;
    }

    virtual QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const {
      if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
          switch (section) {
            case 0:
              return tr ("Name");
            case 1:
              return tr ("Type");
            case 2:
              return tr ("Length");
            case 3:
              return tr ("Modifiers");
            case 4:
              return tr ("Default");
            default:
              return tr ("Column %1").arg (section);
          }
        }
      }
      return QVariant ();
    }

    // *** Rows

    virtual int rowCount (const QModelIndex &) const {
      return record.count ();
    }

    virtual QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const {
      if (!index.isValid ()) {
        return QVariant ();
      }

      if (role == Qt::DisplayRole) {
        if (index.row () >= record.count ()) {
          return QVariant ();
        }

        QSqlField field = record.field (index.row ());

        switch (index.column ()) {
          case 0:
            return field.name ();

          case 1:
            return QLatin1String (QVariant::typeToName (field.type ()));

          case 2:
            return field.length ();

          case 3: {
              QString mods = QStringLiteral ("");

              if (pindex.indexOf (field.name ()) >= 0) {
                mods += tr ("PRIMARY KEY");
              }

              if (field.requiredStatus () == QSqlField::Required) {
                if (!mods.isEmpty ()) {
                  mods += QStringLiteral (" ");
                }
                mods += tr ("NOT NULL");
              }
              else if (field.requiredStatus () == QSqlField::Optional) {
                if (!mods.isEmpty ()) {
                  mods += QStringLiteral (" ");
                }
                mods += tr ("NULL");
              }

              if (field.isAutoValue ()) {
                if (!mods.isEmpty ()) {
                  mods += QStringLiteral (" ");
                }
                mods += tr ("AUTO_INCREMENT");
              }

              return mods;
            }

          case 4:
            return field.defaultValue ();
        }
      }
      else if (role == Qt::DecorationRole) {
      }

      return QVariant ();
    }
};

#endif // _DbConnection_H_
