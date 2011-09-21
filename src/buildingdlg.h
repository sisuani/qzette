/*
 *
 *
 *
 */

#ifndef BUILDINGDLG_H
#define BUILDINGDLG_H

#include <QDialog>
#include <QDataWidgetMapper>
#include <QSqlTableModel>

#include "ui_building.h"

class BuildingDlg : public QDialog, public Ui::BuildingDlg
{
    Q_OBJECT

public:
    BuildingDlg(QWidget *parent = 0, const QString &title = 0,
      int selectedRow = -1, QSqlTableModel *model = 0,
      QSqlTableModel *customerModel = 0);
    ~BuildingDlg();

private slots:
    void accept();
    void customerSearch();

private:
    int getCustomerRow();
    void setCustomer(int row);
    void setBuildingType();

    int selectedRow;
    int idCustomer;

    QDataWidgetMapper *mapper;
    QSqlTableModel *model;
    QSqlTableModel *customerModel;
};

#endif // BUILDINGDLG_H
