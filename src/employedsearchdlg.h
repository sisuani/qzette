/*
 *
 *
 *
 */

#ifndef EMPLOYEDSEARCHDLG_H
#define EMPLOYEDSEARCHDLG_H

#include <QDialog>
#include <QSortFilterProxyModel>

#include <QSqlTableModel>
#include "ui_employedsearch.h"

class Core;

class EmployedSearchDlg : public QDialog, public Ui::EmployedSearch
{
    Q_OBJECT

public:
    EmployedSearchDlg(QWidget *parent = 0, QSqlTableModel *model = 0, int *row = 0);
    ~EmployedSearchDlg();

private slots:
    void addCurrentEmployed();
    void employedAccepted(const QModelIndex &index);
    void filterChanged(const QString &text);
    void filterReturnPressed();

private:
    void init();

    int *row;

    QSortFilterProxyModel *proxyModel;
    QSqlTableModel *model;
};

#endif /* EMPLOYEDSEARCHDLG_H */
