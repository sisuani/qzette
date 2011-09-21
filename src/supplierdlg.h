/*
 *
 *
 *
 */

#ifndef SUPPLIERDLG_H
#define SUPPLIERDLG_H

#include <QDialog>
#include <QDataWidgetMapper>
#include <QSqlRelationalTableModel>

#include "ui_supplier.h"

class SupplierDlg : public QDialog, public Ui::SupplierDlg
{
    Q_OBJECT

public:
    SupplierDlg(QWidget *parent = 0, const QString &title = 0,
      int selectedRow = -1, QSqlRelationalTableModel *model = 0);
    ~SupplierDlg();

signals:
    void areaUpdate();

private slots:
    void accept();
    void areaUpdateCombo();

private:
    int selectedRow;

    QDataWidgetMapper *mapper;
    QSqlRelationalTableModel *model;
};

#endif // SUPPLIERDLG_H
