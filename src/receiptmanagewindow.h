/*
 *
 *
 *
 */

#ifndef RECEIPTMANAGEWINDOW_H
#define RECEIPTMANAGEWINDOW_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QSqlTableModel>

#include "ui_receiptmanage.h"
#include "proxymodel.h"
#include "core.h"

class ReceiptManageWindow : public QWidget, public Ui::ReceiptManage
{
    Q_OBJECT

public:
    ReceiptManageWindow(QWidget *parent = 0, Core *core = 0);
    ~ReceiptManageWindow();

    enum {
	id = 0,
	idEmployed,
	idBuilding,
  	OwnerName,
	Date,
	Type,
	Concept,
	Amount
    };

    enum {
	T_Date = 0,
	T_NReceipt,
  	T_OwnerName,
	T_Concept,
	T_Amount,
	T_Subtotal
    };

    void updateData();
    QString toHtml(bool *ok);

signals:
    void closeWindow(QWidget *);

private slots:
    void init();
    void addReceipt();
    void viewReceipt();
    void removeReceipt();
    void closeWindow();
    void filterChanged(const QString &text);
    void filterReturnPressed();
    void findReceipt();
    void radioButtonClicked();
    void searchEmployed();
    void searchBuilding();
    void employedComboChanged(int index);
    void buildingComboChanged(int index);

private:
    void updateCombos();
    void updateTableModel();
    void errorHandling(int error_type, int error);

    QStandardItemModel *tableModel;
    QSqlTableModel *receiptModel;
    QSqlTableModel *employedModel;
    QSqlTableModel *buildingModel;
    ProxyModel *proxyModel;
    Core *core;
};

#endif // RECEIPTMANAGEWINDOW_H
