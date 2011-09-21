/*
 *
 *
 *
 */

#ifndef BUILDINGMANAGEWINDOW_H
#define BUILDINGMANAGEWINDOW_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QSqlTableModel>

#include "ui_buildingmanage.h"
#include "proxymodel.h"
#include "core.h"

class BuildingManageWindow : public QWidget, public Ui::BuildingManage
{
    Q_OBJECT

public:
    BuildingManageWindow(QWidget *parent = 0, Core *core = 0);
    ~BuildingManageWindow();

    enum {
	id = 0,
	Name,
	idCustomer,
	Address,
	Location,
	Type
    };

    void updateData();
    QString toHtml(bool *ok);

signals:
    void closeWindow(QWidget *);

private slots:
    void init();
    void addBuilding();
    void removeBuilding();
    void editBuilding();
    void closeWindow();
    void filterChanged(const QString &text);
    void filterReturnPressed();

private:
    void errorHandling(int error_type, int error);

    QSqlTableModel *m_model;
    QSqlTableModel *customerModel;
    ProxyModel *proxyModel;
    Core *core;
};

#endif // BUILDINGMANAGEWINDOW_H
