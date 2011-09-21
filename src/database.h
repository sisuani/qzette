/*
 *
 *
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>


class Database : public QObject
{
    Q_OBJECT

public:
    Database();
    ~Database();
    bool addDatabase(const QString &hostName, const QString &databaseName,
		  const QString &userName, const QString &password);
    QSqlDatabase database();
    bool isOpen();
    QSqlQuery *execQuery(const QString &strQuery);
    const QString &lastError() const;
    const int lastErrorNumber() const;

private:
    bool restore();
    bool error;

    QString m_tError;
    int m_iError;
    /* database */
    QSqlDatabase maindb;
};

#endif /* DATABASE_H */
