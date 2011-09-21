/*
 *
 *
 *
 */

#ifndef CONFIGURATIONDLG_H
#define CONFIGURATIONDLG_H

#include <QDialog>
#include <QSqlTableModel>

#include "ui_configuration.h"

class Core;

class ConfigurationDialog : public QDialog, public Ui::Configuration
{
    Q_OBJECT

public:
    ConfigurationDialog(QWidget *parent = 0, Core *core = 0);
    ~ConfigurationDialog();

private slots:
    void init();
    void addArea();
    void removeArea();
    void editArea();
    void addCategories();
    void editCategories();
    void removeCategories();
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private:
    enum {
	Area = 0,
	Category
    };

    void errorHandling(int error_type, int error);

    QSqlTableModel *area_model;
    QSqlTableModel *categories_model;
    Core *core;
};

#endif // CONFIGURATIONDLG_H
