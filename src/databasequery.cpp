/*
 *
 *
 *
 */

#include "databasequery.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QByteArray>
#include <QBuffer>
#include <QDebug>
#include <QMessageBox>
#include <QSqlRelation>

#include "log.h"
#include "core.h"
#include "mainwin.h"
#include "suppliermanagewindow.h"
#include "employedmodel.h"
#include "buildingmanagewindow.h"
#include "receiptmanagewindow.h"

DatabaseQuery::DatabaseQuery(QObject *parent, Core *core)
    : QObject(parent),
      core(core)
{
}

DatabaseQuery::~DatabaseQuery()
{
}

void DatabaseQuery::setErrorText(const QString &msg)
{
    m_tError = msg;
}

const QString &DatabaseQuery::lastErrorText()
{
    return m_tError;
}

const int DatabaseQuery::lastErrorNumber()
{
    return m_iError;
}

bool DatabaseQuery::execQuery(const QString &strQuery, QSqlQuery **q)
{
    *q = core->database()->execQuery(strQuery);
    if(*q) {
	return true;
    }

    setErrorText(core->database()->lastError());
    m_iError = core->database()->lastErrorNumber();
    return false;
}

void DatabaseQuery::freeQuery(QSqlQuery **q)
{
    if(*q) {
	delete *q;
	*q = 0;
    }
}

const QVariant DatabaseQuery::getValue(QSqlQuery *q, const QString &value)
{
    if(q) {
	return q->value(q->record().indexOf(value));
    }

    return "";
}


//////////////////////////////////////////////////
// SELECTS
//////////////////////////////////////////////////

bool DatabaseQuery::areas(QSqlTableModel *model)
{
    model->setTable("Area");
    if(model->select())
	return true;

    setErrorText(model->lastError().text());
    return false;
}

bool DatabaseQuery::categories(QSqlTableModel *model)
{
    model->setTable("Category");
    if(model->select()) {
        return true;
    }

    setErrorText(model->lastError().text());
    return false;
}

bool DatabaseQuery::customers(QSqlTableModel *model)
{
    model->setTable("Customer");
    if(model->select()) {
        return true;
    }

    setErrorText(model->lastError().text());
    return false;
}

bool DatabaseQuery::suppliers(QSqlRelationalTableModel *model)
{
    model->setTable("Supplier");
    model->setRelation(SupplierManageWindow::idArea,
      QSqlRelation("Area", "idArea", "name"));

    if(model->select())
	return true;

    setErrorText(model->lastError().text());
    return false;
}

bool DatabaseQuery::employees(QSqlRelationalTableModel *model)
{
    model->setTable("Employed");
    model->setRelation(EmployedModel::idCategory,
      QSqlRelation("Category", "idCategory", "name"));

    if(model->select())
	return true;

    setErrorText(model->lastError().text());
    return false;
}

bool DatabaseQuery::employeesTable(QSqlTableModel *model)
{
    model->setTable("Employed");

    if(model->select())
	return true;

    setErrorText(model->lastError().text());
    return false;
}

bool DatabaseQuery::buildings(QSqlTableModel *model)
{
    model->setTable("Building");

    if(model->select())
	return true;

    return false;
}

bool DatabaseQuery::receipts(QSqlTableModel *model)
{
    model->setTable("Receipt");
    model->setSort(ReceiptManageWindow::Date, Qt::AscendingOrder);

    if(model->select())
	return true;
    return false;
}
