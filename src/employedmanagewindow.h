/*
 *
 *
 *
 */

#ifndef EMPLOYEDMANAGEWINDOW_H
#define EMPLOYEDMANAGEWINDOW_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QSqlRelationalTableModel>

#include "ui_employedmanage.h"
#include "employedmodel.h"
#include "proxymodel.h"
#include "core.h"

class EmployedManageWindow : public QWidget, public Ui::EmployedManage
{
    Q_OBJECT

public:
    EmployedManageWindow(QWidget *parent = 0, Core *core = 0);
    ~EmployedManageWindow();

    void updateData();
    QString toHtml(bool *ok);

signals:
    void closeWindow(QWidget *);

private slots:
    void init();
    void addEmployed();
    void removeEmployed();
    void editEmployed();
    void closeWindow();
    void filterChanged(const QString &text);
    void filterReturnPressed();
    void ratingComboChanged(int index);
    void categoryUpdate();
    void categoryComboChanged(int index);

private:
    void categoryUpdateCombo(bool select);
    void updateColumns();
    void errorHandling(int error_type, int error);

    EmployedModel *m_model;
    ProxyModel *proxyModel;
    Core *core;
};

#endif // EMPLOYEDMANAGEWINDOW_H
