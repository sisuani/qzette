/*
 *
 *
 *
 */


#ifndef DATABASEQUERY_H
#define DATABASEQUERY_H

#include <QObject>
#include <QVariant>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QSqlRelationalTableModel>
#include <QDateTime>

#include "database.h"

class Core;
//class Customer;
//class Supplier;

class DatabaseQuery : public QObject
{

public:

    DatabaseQuery(QObject *parent = 0, Core *core = 0);
    ~DatabaseQuery();

    const QString &lastErrorText();
    const int lastErrorNumber();

    /* selects */
    bool areas(QSqlTableModel *model);
    bool categories(QSqlTableModel *model);
    bool customers(QSqlTableModel *model);
    bool suppliers(QSqlRelationalTableModel *model);
    bool employees(QSqlRelationalTableModel *model);
    bool employeesTable(QSqlTableModel *model);
    bool buildings(QSqlTableModel *model);
    bool receipts(QSqlTableModel *model);

    /* errors */
    bool errorHandling(int WINDOW, int error);

private:
    void setErrorText(const QString &msg);
    bool execQuery(const QString &strQuery, QSqlQuery **q);
    void freeQuery(QSqlQuery **q);
    const QVariant getValue(QSqlQuery *q, const QString &value);

    QString m_tError;
    int m_iError;
    Core *core;
};

#endif /* DATABASEQUERY_H */
