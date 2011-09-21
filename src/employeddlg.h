/*
 *
 *
 *
 */

#ifndef EMPLOYEDDLG_H
#define EMPLOYEDDLG_H

#include <QDialog>
#include <QDataWidgetMapper>
#include <QSqlRelationalTableModel>

#include "ui_employed.h"
#include "employedmodel.h"

class EmployedDlg : public QDialog, public Ui::EmployedDlg
{
    Q_OBJECT

public:
    EmployedDlg(QWidget *parent = 0, const QString &title = 0,
      int selectedRow = -1, EmployedModel *model = 0);
    ~EmployedDlg();

signals:
    void categoryUpdate();

private slots:
    void accept();
    void categoryUpdateCombo();

private:
    int selectedRow;

    QDataWidgetMapper *mapper;
    EmployedModel *model;
};

#endif // EMPLOYEDDLG_H
