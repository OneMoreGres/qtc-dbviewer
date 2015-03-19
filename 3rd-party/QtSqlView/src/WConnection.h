// $Id: WConnection.h 20 2006-10-10 08:24:36Z tb $
// Copyright 2006 Timo Bingmann
//
// Controller for the connection editing dialog
//

#ifndef _WConnection_H_
#define _WConnection_H_

#include <QtGui/QDialog>
#include <QtGui/QFileDialog>
#include <QtSql/QSqlDatabase>

#include "ui_WConnection.h"

#include "DbConnection.h"

class WConnection : public QDialog, private Ui::WConnection
{
    Q_OBJECT

    QString getDescription(QString drv)
    {
	if (0) return "";
	else if (drv == "QDB2")		return "IBM DB2";
	else if (drv == "QIBASE")	return "Borland InterBase";
	else if (drv == "QOCI")		return "Oracle Call Interface";
	else if (drv == "QODBC")	return "ODBC";
	else if (drv == "QODBC3")	return "ODBC";
	else if (drv == "QTDS")		return "Sybase Adaptive Server";

	else if (drv == "QMYSQL")	return "MySQL 4.x";
	else if (drv == "QMYSQL3")	return "MySQL 3.x";
	else if (drv == "QPSQL")	return "PostgreSQL 8.x";
	else if (drv == "QPSQL7")	return "PostgreSQL 7.x";
	else if (drv == "QSQLITE")	return "SQLite 3.x";
	else if (drv == "QSQLITE2")	return "SQLite 2.x";
	else return QString::null;
    }

public:
    WConnection(QWidget *parent, const DbParameter *dbp)
	: QDialog(parent)
    {
	setupUi(this);

	Q_FOREACH(QString drv, QSqlDatabase::drivers())
	{
	    QString desc = getDescription(drv);
	    if (desc.isNull())
		comboType->addItem(drv, drv);
	    else
		comboType->addItem(QString("%1 (%2)").arg(desc).arg(drv),
				   drv);
	}

	if (dbp)
	{
	    editLabel->setText(dbp->label);
	    comboType->setCurrentIndex( comboType->findData(dbp->driver) );
	    editHostname->setText(dbp->hostname);
	    spinPort->setValue(dbp->port);
	    editUsername->setText(dbp->username);
	    editPassword->setText(dbp->password);
	    checkAskPassword->setChecked(dbp->askpassword);
	    editDatabase->setText(dbp->database);
	    checkSysTables->setChecked(dbp->showsystables);
	}
	
	updatePasswordStatus();
    }

    DbParameter	dbp;
    
    void fetchDbParameter()
    {
	dbp.label = editLabel->text();
	dbp.driver = comboType->itemData( comboType->currentIndex() ).toString();
	dbp.hostname = editHostname->text();
	dbp.port = spinPort->value();
	dbp.username = editUsername->text();
	dbp.password = editPassword->text();
	dbp.askpassword = checkAskPassword->isChecked();
	if (dbp.askpassword)
	    dbp.password.clear();
	dbp.database = editDatabase->text();
	dbp.showsystables = checkSysTables->isChecked();
    }

    void updatePasswordStatus()
    {
	if (!checkAskPassword->isEnabled()) return;
	editPassword->setEnabled( !checkAskPassword->isChecked() );
    }

    void updateFields()
    {
	const QString &drv = dbp.driver;

	if (drv == "QSQLITE" || drv == "QSQLITE2")
	{
	    editHostname->setEnabled(false);
	    spinPort->setEnabled(false);
	    editUsername->setEnabled(false);
	    editPassword->setEnabled(false);
	    checkAskPassword->setEnabled(false);
	    labelDatabase->setText("Filename");
	    buttonSelectFile->setEnabled(true);
	}
	else if (drv == "QMYSQL" || drv == "QMYSQL3")
	{
	    editHostname->setEnabled(true);
	    spinPort->setEnabled(true);
	    editUsername->setEnabled(true);
	    checkAskPassword->setEnabled(true);
	    updatePasswordStatus();
	    labelDatabase->setText("Database");
	    buttonSelectFile->setEnabled(false);
	}
	else if (drv == "QPSQL" || drv == "QPSQL7")
	{
	    editHostname->setEnabled(true);
	    spinPort->setEnabled(true);
	    editUsername->setEnabled(true);
	    checkAskPassword->setEnabled(true);
	    updatePasswordStatus();
	    labelDatabase->setText("Database");
	    buttonSelectFile->setEnabled(false);
	}
	else
	{
	    editHostname->setEnabled(true);
	    spinPort->setEnabled(true);
	    editUsername->setEnabled(true);
	    checkAskPassword->setEnabled(true);
	    updatePasswordStatus();
	    labelDatabase->setText("Database");
	    buttonSelectFile->setEnabled(false);
	}
    }

public slots:

    void on_okButton_clicked()
    {
	fetchDbParameter();
	accept();
    }

    void on_testButton_clicked()
    {
	fetchDbParameter();

	DbConnection conn(dbp);
	DbList dblist;

	QSqlError ce = conn.connect(dblist);

	if (ce.isValid())
	{
	    QMessageBox::critical(this, "Testing Connection",
				  QString("Connection failed:\n%1\n%2")
				  .arg(ce.driverText())
				  .arg(ce.databaseText()));
	}
	else
	{
	    QMessageBox::information(this, "Testing Connection",
				     QString("Connection established successfully."));
	}
    }

    void on_checkAskPassword_clicked()
    {
	updatePasswordStatus();
    }

    void on_comboType_currentIndexChanged(int index)
    {
	dbp.driver = comboType->itemData(index).toString();
	updateFields();
    }

    void on_buttonSelectFile_clicked()
    {
	QString filename = QFileDialog::getOpenFileName(this, "Choose a SQLite database file",
							QString(),
							"SQLite databases (*.db);;All Files (*.*)");

	if (filename.isEmpty()) return;
	editDatabase->setText(filename);
    }
};

#endif // _WConnection_H_
