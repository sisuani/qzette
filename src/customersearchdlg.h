/*
 *
 *
 *
 */

#ifndef CUSTOMERSEARCHDLG_H
#define CUSTOMERSEARCHDLG_H

#include <QDialog>
#include <QSortFilterProxyModel>

#include <QSqlTableModel>
#include "ui_customersearch.h"

class Core;

class CustomerSearchDlg : public QDialog, public Ui::CustomerSearch
{
    Q_OBJECT

public:
    CustomerSearchDlg(QWidget *parent = 0, QSqlTableModel *model = 0, int *row = 0);
    ~CustomerSearchDlg();

private slots:
    void addCurrentCustomer();
    void customerAccepted(const QModelIndex &index);
    void filterChanged(const QString &text);
    void filterReturnPressed();
    void updateData();
    void addNewCustomer();

private:
    void init();

    int *row;

    QSortFilterProxyModel *proxyModel;
    QSqlTableModel *model;
};

#endif /* CUSTOMERSEARCHDLG_H */
