/*
 *
 *
 *
 */

#ifndef BUILDINGSEARCHDLG_H
#define BUILDINGSEARCHDLG_H

#include <QDialog>
#include <QSortFilterProxyModel>

#include <QSqlTableModel>
#include "ui_buildingsearch.h"

class Core;

class BuildingSearchDlg : public QDialog, public Ui::BuildingSearch
{
    Q_OBJECT

public:
    BuildingSearchDlg(QWidget *parent = 0, QSqlTableModel *model = 0, int *row = 0);
    ~BuildingSearchDlg();

private slots:
    void addCurrentBuilding();
    void buildingAccepted(const QModelIndex &index);
    void filterChanged(const QString &text);
    void filterReturnPressed();

private:
    void init();

    int *row;

    QSortFilterProxyModel *proxyModel;
    QSqlTableModel *model;
};

#endif /* BUILDINGSEARCHDLG_H */
