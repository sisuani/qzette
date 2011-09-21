/*
 *
 *
 */

#include "database.h"

#include <QSqlError>

#include "log.h"

Database::Database()
{
    m_iError = 0;
    error = false;
}

Database::~Database()
{
    if(maindb.isOpen()) {
	maindb.close();
    }
}

bool Database::addDatabase(const QString &hostName, const QString &databaseName,
		  const QString &userName, const QString &password)
{
    if(hostName.trimmed().isEmpty() || databaseName.trimmed().isEmpty()) {
	Log::error("hostName | databaseName: empty values");
	return false;
    }

    maindb = QSqlDatabase::addDatabase("QMYSQL", "maindb");

    maindb.setHostName(hostName);
    maindb.setDatabaseName(databaseName);
    maindb.setUserName(userName);
    maindb.setPassword(password);

    if(!maindb.open()) {
	Log::error(maindb.lastError().text());
	return false;
    }

    Log::print(4, "Connecting to db ... OK");
    return true;
}

QSqlDatabase Database::database()
{
    return maindb;
}

bool Database::isOpen()
{
    return maindb.isOpen();
}

QSqlQuery *Database::execQuery(const QString &strQuery)
{
    if(error) {
	error = false;
	return 0;
    }

    if(!isOpen()) {
	if(!restore()) {
	    return 0;
	}
    }

    QSqlQuery *query = 0;
    query = new QSqlQuery(maindb);

    if(!query->exec(strQuery)) {
	m_tError = query->lastError().text();
	m_iError = query->lastError().number();
	Log::error("!query->exec(): " + strQuery);
	restore();
	delete query;
	/* retry query */
	error = true;
	return execQuery(strQuery);
    }

    if(!query->isActive()) {
	m_tError = query->lastError().text();
	m_iError = query->lastError().number();
	Log::error("!query->isActive(): " + strQuery);
	restore();
	delete query;
	/* retry query */
	error = true;
	return execQuery(strQuery);
    }

    Log::print(4, "Query: " + strQuery);

    return query;
}

bool Database::restore()
{
    //XXX: close (?) : produce violacion de segmento en los table models
    maindb.close();
    if(maindb.open()) {
	Log::print(3, "restore() sucessfully");
	return true;
    }

    return false;
}

const QString &Database::lastError() const
{
    return m_tError;
}

const int Database::lastErrorNumber() const
{
    return m_iError;
}
