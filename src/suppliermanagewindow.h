/*
 *
 *
 *
 */

#ifndef SUPPLIERMANAGEWINDOW_H
#define SUPPLIERMANAGEWINDOW_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QSqlRelationalTableModel>

#include "ui_suppliermanage.h"
#include "proxymodel.h"
#include "core.h"

class SupplierManageWindow : public QWidget, public Ui::SupplierManage
{
    Q_OBJECT

public:
    SupplierManageWindow(QWidget *parent = 0, Core *core = 0);
    ~SupplierManageWindow();

    enum {
	id = 0,
	Name,
	idArea,
	Address,
	Location,
	Phone,
	Fax,
	Email,
	ContactName,
	ContactPhone
    };

    void updateData();
    QString toHtml(bool *ok);

signals:
    void closeWindow(QWidget *);

private slots:
    void init();
    void addSupplier();
    void removeSupplier();
    void editSupplier();
    void closeWindow();
    void filterChanged(const QString &text);
    void filterReturnPressed();
    void areaUpdate();
    void areaComboChanged(int index);

private:
    void areaUpdateCombo(bool select);
    void errorHandling(int error_type, int error);

    QSqlRelationalTableModel *m_model;
    ProxyModel *proxyModel;
    Core *core;
};

#endif // SUPPLIERMANAGEWINDOW_H
