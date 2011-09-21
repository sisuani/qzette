/*
 *
 *
 *
 */

#ifndef CREATERECEIPTDLG_H
#define CREATERECEIPTDLG_H

#include <QDialog>
#include <QSqlRelationalTableModel>
#include <QDataWidgetMapper>

#include "ui_createreceipt.h"

class CreateReceiptDlg : public QDialog, public Ui::CreateReceipt
{
    Q_OBJECT

public:
    CreateReceiptDlg(QWidget *parent = 0, QSqlTableModel *model = 0,
      QSqlTableModel *employedModel = 0, QSqlTableModel *buildingModel = 0);
    ~CreateReceiptDlg();

private slots:
    void accept();
    void searchEmployed();
    void searchBuilding();
    void employedComboChanged(int index);
    void buildingComboChanged(int index);

private:
    QSqlTableModel *model;
    QSqlTableModel *employedModel;
    QSqlTableModel *buildingModel;
};

#endif // CREATERECEIPTDLG_H
