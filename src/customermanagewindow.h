/*
 *
 *
 *
 */

#ifndef CUSTOMERMANAGEWINDOW_H
#define CUSTOMERMANAGEWINDOW_H

#include <QWidget>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>

#include "ui_customermanage.h"
#include "core.h"

class Customer;

class CustomerManageWindow : public QWidget, public Ui::CustomerManage
{
    Q_OBJECT

public:
    CustomerManageWindow(QWidget *parent = 0, Core *core = 0);
    ~CustomerManageWindow();

    enum {
	id = 0,
	LastName,
	Name,
	Address,
	Location,
	CellPhone,
	Phone,
	Email
    };

    QString toHtml(bool *ok);

signals:
    void closeWindow(QWidget *);

private slots:
    void init();
    void addCustomer();
    void removeCustomer();
    void editCustomer();
    void closeWindow();
    void filterChanged(const QString &text);
    void filterReturnPressed();

private:
    void errorHandling(int error_type, int error);

    QSqlTableModel *m_model;
    QSortFilterProxyModel *proxyModel;
    Core *core;
};

#endif // CUSTOMERMANAGEWINDOW_H
