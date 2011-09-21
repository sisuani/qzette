/*
 *
 *
 *
 */

#ifndef CUSTOMERDLG_H
#define CUSTOMERDLG_H

#include <QDialog>
#include <QSqlTableModel>
#include <QDataWidgetMapper>

#include "ui_customer.h"

class CustomerDlg : public QDialog, public Ui::CustomerDlg
{
    Q_OBJECT

public:
    CustomerDlg(QWidget *parent = 0, const QString &title = 0,
      int selectedRow = -1, QSqlTableModel *model = 0);
    ~CustomerDlg();

private slots:
    void accept();

private:
    int selectedRow;

    QDataWidgetMapper *mapper;
    QSqlTableModel *model;
};

#endif // CUSTOMERDLG_H
