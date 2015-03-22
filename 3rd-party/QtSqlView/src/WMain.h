// $Id: WMain.h 25 2006-10-10 09:50:32Z tb $
// Copyright 2006 Timo Bingmann
//
// The main dialog controller class with a load of slots to process input from
// all tabs.
//

#ifndef _WMain_H_
#define _WMain_H_

#include <QMainWindow>
#include <QFileDialog>
#include <QtGui/QClipboard>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QProcess>
#include <QtCore/QUrl>
#include <QStyledItemDelegate>

#include "ui_WMain.h"

#include "WConnection.h"
#include "SQLHighlighter.h"

#ifdef Q_WS_WIN
#  include <windows.h>
#  include <shlobj.h>
#  include <intshcut.h>
#endif

class SqlFieldDelegate : public QStyledItemDelegate {
  Q_OBJECT

  public:
    QString displayText (const QVariant &value, const QLocale & /*locale*/) const {
      if (value.type () != QVariant::ByteArray) {
        return value.toString ();
      }
      QString stringed = value.toString ();
      QByteArray raw = value.toByteArray ();
      if (stringed.size () == raw.size ()) {
        return stringed;
      }
      return tr ("Binary [size %1]").arg (raw.size ());
    }
};

class WMain : public QMainWindow, public Ui::WMain {
  Q_OBJECT

  public:
    WMain ()
      : QMainWindow (),
      datatablemodel (NULL) {
      setupUi (this);
      statusBar ()->hide ();
      menuBar ()->hide ();

      dblist.loadFromSettings ();

      treeDbList->setModel (&dblist);
      if (dblist.rowCount (QModelIndex ()) > 0) {
        treeDbList->setCurrentIndex (dblist.index (0,0));
      }

      treeDbList->setContextMenuPolicy (Qt::CustomContextMenu);
      connect (treeDbList, SIGNAL (customContextMenuRequested (const QPoint &)),
               this, SLOT (show_treeDbList_contextMenu (const QPoint &)));

      dataTable->setItemDelegate (new SqlFieldDelegate);
      dataTable->setContextMenuPolicy (Qt::CustomContextMenu);
      connect (dataTable, SIGNAL (customContextMenuRequested (const QPoint &)),
               this, SLOT (show_dataTable_contextMenu (const QPoint &)));

      connect (dataTable->horizontalHeader (), SIGNAL (sectionDoubleClicked (int)),
               this, SLOT (slot_dataTable_horizontalHeader_sectionDoubleClicked (int)));

      schemaTable->setModel (&schemamodel);
      schemaTable->verticalHeader ()->hide ();

      queryTable->setItemDelegate (new SqlFieldDelegate);
      queryTable->hide ();
      queryTable->setModel (&userquerymodel);

      // configure query editor
      QFont font (QStringLiteral ("Courier"), 10);
      font.setFixedPitch (true);
      editQuery->setFont (font);

      new SQLHighlighter (editQuery->document ());
    }

    DbList dblist;

    ~WMain () {
      dblist.saveToSettings ();
    }

    QSqlTableModel  *datatablemodel;
    int datatablemodel_lastsort;

    DbSchemaModel schemamodel;

    QSqlQueryModel userquerymodel;

  public slots:
    // *** Menu Actions ***

    void on_action_AddConnection_triggered () {
      WConnection wc (this, NULL);

      if (wc.exec () == QDialog::Accepted) {
        dblist.addDbConnection (wc.dbp);
      }
    }

    void on_action_EditConnection_triggered () {
      QModelIndex selected = treeDbList->currentIndex ();

      int selectednum = dblist.getDbConnectionNum (selected);

      if (selectednum < 0) {
        QMessageBox::critical (this, tr ("QtSqlView"),
                               tr ("No database connection selected. Click on one of the entries in the database list."));
      }
      else{
        WConnection wc (this, &dblist.list[selectednum]->dbparam);

        if (wc.exec () == QDialog::Accepted) {
          dblist.editDbConnection (selectednum, wc.dbp);
        }
      }
    }

    void on_action_RemoveConnection_triggered () {
      QModelIndex selected = treeDbList->currentIndex ();

      int selectednum = dblist.getDbConnectionNum (selected);

      if (selectednum < 0) {
        QMessageBox::critical (this, tr ("QtSqlView"),
                               tr ("No database connection selected. Click on one of the entries in the database list."));
      }
      else{
        dblist.delDbConnection (selectednum);
      }
    }

    void on_action_RefreshTablelist_triggered () {
      dblist.refresh ();
    }

    void on_action_Exit_triggered () {
      close ();
    }

    void on_action_About_triggered () {
      QMessageBox::about (this, QStringLiteral ("About QtSqlView"),
                          QStringLiteral ("QtSqlView is a simple SQL database browser\n") +
                          QStringLiteral ("built on Qt's excellent components.\n\n") +
                          QStringLiteral ("Copyright 2006 Timo Bingmann\n") +
                          QStringLiteral ("Released under the GNU General Public License v2\n\n") +
                          QStringLiteral ("Visit http://idlebox.net/2006/qtsqlview/\n") +
                          QStringLiteral ("for updates and more.\n"));
    }

    // copied from qt-4.2.0's QDesktopServices

    inline static bool launch (const QUrl &url, const QString &client) {
      return (QProcess::startDetached (client + QStringLiteral (" ") + QString::fromUtf8 (url.toEncoded ())));
    }

    void on_action_VisitWebsite_triggered () {
      QUrl url (QStringLiteral ("http://idlebox.net/2006/qtsqlview/"));

#ifdef Q_WS_WIN
      QT_WA ({ ShellExecute (0, 0, (TCHAR *)QString (url.toEncoded ()).utf16 (), 0, 0, SW_SHOWNORMAL); },
             { ShellExecuteA (0, 0, QString (url.toEncoded ()).toLocal8Bit (), 0, 0, SW_SHOWNORMAL); });
#else
      if (launch (url, QStringLiteral ("firefox"))) {
        return;
      }
      if (launch (url, QStringLiteral ("mozilla"))) {
        return;
      }
      if (launch (url, QStringLiteral ("netscape"))) {
        return;
      }
      if (launch (url, QStringLiteral ("opera"))) {
        return;
      }
#endif
    }

    void on_action_AboutQt_triggered () {
      QMessageBox::aboutQt (this, QStringLiteral ("About Qt"));
    }

    // *** Triggers of the DbList TreeView

    void on_treeDbList_clicked (const QModelIndex &index) {
      DbTable *dbt = dblist.getDbTable (index);

      if (!dbt) {
        return;
      }

      if (datatablemodel) {
        delete datatablemodel;
      }

      datatablemodel = new QSqlTableModel (this, dbt->dbconn->db);
      dataTable->setModel (datatablemodel);

      datatablemodel->setTable (dbt->tablename);
      datatablemodel->setEditStrategy (QSqlTableModel::OnManualSubmit);

      datatablemodel->select ();
      datatablemodel_lastsort = -1;

      schemamodel.setRecord (dbt, datatablemodel->record ());
    }

    void on_treeDbList_expanded (const QModelIndex &index) {
      dblist.expanding (index);
    }

    void on_treeDbList_collapsed (const QModelIndex &index) {
      dblist.collapsed (index);
    }

    void show_treeDbList_contextMenu (const QPoint &position) {
      QMenu contextmenu (this);

      QModelIndex index = treeDbList->indexAt (position);
      if (index.isValid ()) {
        contextmenu.addAction (action_EditConnection);
        contextmenu.addAction (action_RemoveConnection);
        contextmenu.addSeparator ();
      }

      contextmenu.addAction (action_AddConnection);
      contextmenu.addAction (action_RefreshTablelist);

      contextmenu.exec (treeDbList->mapToGlobal (position));
    }

    // *** Data Table Tab ***

    void show_dataTable_contextMenu (const QPoint &position) {
      QMenu contextmenu (this);

      contextmenu.addAction (action_RevertData);
      contextmenu.addAction (action_RefreshData);
      contextmenu.addSeparator ();
      contextmenu.addAction (action_AddRow);
      contextmenu.addAction (action_DelRow);
      contextmenu.addSeparator ();
      contextmenu.addAction (action_DownloadRaw);
      contextmenu.addAction (action_UploadRaw);
      QModelIndex index = dataTable->indexAt (position);
      bool isRaw = (index.isValid () && index.data ().type () == QVariant::ByteArray);
      action_DownloadRaw->setEnabled (isRaw);
      action_UploadRaw->setEnabled (isRaw);

      contextmenu.exec (dataTable->mapToGlobal (position));
    }


    void on_action_DownloadRaw_triggered () {
      if (!datatablemodel) {
        return;
      }
      QModelIndex index = dataTable->currentIndex ();
      if (!index.isValid ()) {
        return;
      }

      QString filename = QFileDialog::getSaveFileName (this, tr ("Choose a raw data file"),
                                                       QString (),
                                                       tr ("All Files (*.*)"));

      if (filename.isEmpty ()) {
        return;
      }

      QFile file (filename);
      if (!file.open (QIODevice::WriteOnly)) {
        QMessageBox::critical (this, tr ("QtSqlView"), tr ("Could not save raw data file"));
        return;
      }

      QByteArray data = index.data ().toByteArray ();
      file.write (data);
    }


    void on_action_UploadRaw_triggered () {
      if (!datatablemodel) {
        return;
      }
      QModelIndex index = dataTable->currentIndex ();
      if (!index.isValid ()) {
        return;
      }

      QString filename = QFileDialog::getOpenFileName (this, tr ("Choose a raw data file"),
                                                       QString (),
                                                       tr ("All Files (*.*)"));

      if (filename.isEmpty ()) {
        return;
      }

      QFile file (filename);
      if (!file.open (QIODevice::ReadOnly)) {
        QMessageBox::critical (this, tr ("QtSqlView"), tr ("Could not load raw data file"));
        return;
      }

      QByteArray data = file.readAll ();
      if (!datatablemodel->setData (index, data)) {
        QMessageBox::critical (this, tr ("QtSqlView"), tr ("Failed to set loaded raw data"));
      }
    }


    void on_action_AddRow_triggered () {
      if (!datatablemodel) {
        return;
      }

      QModelIndex index = dataTable->currentIndex ();

      int row = index.row () == -1 ? 0 : index.row ();
      datatablemodel->insertRow (row);

      index = datatablemodel->index (row, 0);
      dataTable->setCurrentIndex (index);
      dataTable->edit (index);
    }

    void on_action_DelRow_triggered () {
      if (!datatablemodel) {
        return;
      }

      QModelIndexList indexlist = dataTable->selectionModel ()->selectedIndexes ();

      for (int i = 0; i < indexlist.count (); ++i) {
        datatablemodel->removeRow (indexlist.at (i).row ());
      }
    }

    void on_action_RefreshData_triggered () {
      if (!datatablemodel) {
        return;
      }

      datatablemodel->select ();
    }

    void on_action_SaveData_triggered () {
      if (!datatablemodel) {
        return;
      }

      datatablemodel->submitAll ();
    }

    void on_action_RevertData_triggered () {
      if (!datatablemodel) {
        return;
      }

      datatablemodel->revertAll ();
    }

    void on_addRowButton_clicked () {
      on_action_AddRow_triggered ();
    }

    void on_delRowButton_clicked () {
      on_action_DelRow_triggered ();
    }

    void on_refreshDataButton_clicked () {
      on_action_RefreshData_triggered ();
    }

    void on_saveDataButton_clicked () {
      on_action_SaveData_triggered ();
    }

    void on_revertDataButton_clicked () {
      on_action_RevertData_triggered ();
    }

    void slot_dataTable_horizontalHeader_sectionDoubleClicked (int logicalIndex) {
      if (!datatablemodel) {
        return;
      }

      Qt::SortOrder order = Qt::AscendingOrder;

      if (logicalIndex == datatablemodel_lastsort) {
        order = Qt::DescendingOrder;
        datatablemodel_lastsort = -1;
      }
      else {
        datatablemodel_lastsort = logicalIndex;
      }

      datatablemodel->sort (logicalIndex, order);
    }

    static void save_to_clipboard (QSqlQuery query, const QItemSelection &sellist, QClipboard::Mode mode) {
      if (!query.isSelect () || !query.isActive ()) {
        qDebug () << tr ("bad query");
        return;
      }

      QString seltext;

      Q_FOREACH (const QItemSelectionRange &selrange, sellist) {
        if (!query.seek (selrange.top ())) {
          qDebug () << tr ("Could not seek in result");
          continue;
        }

        for (int rowi = selrange.top (); rowi <= selrange.bottom (); ++rowi) {
          for (int fi = selrange.left (); fi <= selrange.right (); ++fi) {
            if (fi != selrange.left ()) {
              seltext += QStringLiteral ("\t");
            }
            seltext += query.value (fi).toString ();
          }
          seltext += QStringLiteral ("\n");
          query.next ();
        }
      }

      QClipboard *clip = QApplication::clipboard ();
      clip->setText (seltext, mode);
    }

    void on_copyDataButton_clicked () {
      if (!datatablemodel) {
        return;
      }

      QItemSelectionModel *selmodel = dataTable->selectionModel ();
      const QItemSelection &sellist = selmodel->selection ();

      save_to_clipboard (datatablemodel->query (), sellist, QClipboard::Clipboard);
    }

    // *** Query Tab ***

    void on_goQueryButton_clicked () {
      DbConnection *dbc = dblist.getDbConnection (treeDbList->currentIndex () );

      if (!dbc) {
        queryTable->hide ();
        queryResultText->show ();

        queryResultText->setPlainText (tr ("No database connection selected.\nAdd and activate a connection in the left tree view."));
        return;
      }

      if (!dbc->db.isOpen ()) {
        QSqlError ce = dbc->connect (dblist);
        if (ce.isValid ()) {
          queryTable->hide ();
          queryResultText->show ();

          queryResultText->setPlainText (QString (QStringLiteral ("%1\n%2"))
                                         .arg (ce.driverText ())
                                         .arg (ce.databaseText ()));
          return;
        }
      }

      // initialize new sql query object
      userquerymodel.setQuery (editQuery->toPlainText (), dbc->db);

      if (userquerymodel.lastError ().isValid ()) {
        queryTable->hide ();
        queryResultText->show ();

        queryResultText->setPlainText (QString (QStringLiteral ("%1\n%2"))
                                       .arg (userquerymodel.lastError ().driverText ())
                                       .arg (userquerymodel.lastError ().databaseText ()));
      }
      else{
        // query was successful

        if (userquerymodel.query ().isSelect ()) {
          queryResultText->hide ();
          queryTable->show ();
        }
        else{
          queryTable->hide ();
          queryResultText->show ();

          queryResultText->setPlainText (tr ("%1 rows affected.")
                                         .arg (userquerymodel.query ().numRowsAffected () ));
        }
      }
    }

    void on_copyQueryDataButton_clicked () {
      QItemSelectionModel *selmodel = queryTable->selectionModel ();
      const QItemSelection &sellist = selmodel->selection ();

      save_to_clipboard (userquerymodel.query (), sellist, QClipboard::Clipboard);
    }

    void on_clearQueryButton_clicked () {
      editQuery->clear ();
    }

    void on_loadQueryButton_clicked () {
      QString filename = QFileDialog::getOpenFileName (this, tr ("Choose a SQL text file"),
                                                       QString (),
                                                       tr ("SQL text files (*.sql *.txt);;All Files (*.*)"));

      if (filename.isEmpty ()) {
        return;
      }

      QFile file (filename);
      if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical (this, tr ("QtSqlView"),
                               tr ("Could not load sql query text file"));
        return;
      }

      editQuery->setPlainText (QString::fromLocal8Bit (file.readAll ()) );
    }

    void on_saveQueryButton_clicked () {
      QString filename = QFileDialog::getSaveFileName (this, tr ("Choose a SQL text file"),
                                                       QString (),
                                                       tr ("SQL text files (*.sql *.txt);;All Files (*.*)"));

      if (filename.isEmpty ()) {
        return;
      }

      QFile file (filename);
      if (!file.open (QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical (this, tr ("QtSqlView"),
                               tr ("Could not save sql query text file"));
        return;
      }

      QTextStream out (&file);
      out << editQuery->toPlainText ().toLocal8Bit ();
    }
};

#endif // _WMain_H_
