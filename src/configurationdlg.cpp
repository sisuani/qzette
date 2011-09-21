/*
 *
 *
 *
 */

#include "configurationdlg.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include "core.h"
#include "common.h"

ConfigurationDialog::ConfigurationDialog(QWidget *parent, Core *core)
    : QDialog(parent),
      core(core)
{
    setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose);

    /* initialize data */
    area_model = 0;
    categories_model = 0;

    /* connecting */
    connect(pb_add_area, SIGNAL(clicked()),
	    this, SLOT(addArea()));
    connect(pb_remove_area, SIGNAL(clicked()),
	    this, SLOT(removeArea()));
    connect(pb_edit_area, SIGNAL(clicked()),
	    this, SLOT(editArea()));

    connect(pb_add_categories, SIGNAL(clicked()),
	    this, SLOT(addCategories()));
    connect(pb_remove_categories, SIGNAL(clicked()),
	    this, SLOT(removeCategories()));
    connect(pb_edit_categories, SIGNAL(clicked()),
	    this, SLOT(editCategories()));

    connect(pb_close, SIGNAL(clicked()),
	    this, SLOT(close()));
    connect(lw_list, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
	    this, SLOT(changePage(QListWidgetItem *, QListWidgetItem *)));

    QTimer::singleShot(0, this, SLOT(init()));
}

ConfigurationDialog::~ConfigurationDialog()
{
    if(area_model) {
	delete area_model;
    }

    if(categories_model) {
	delete categories_model;
    }
}

void ConfigurationDialog::init()
{
    bool r;

    // area
    area_model = new QSqlTableModel(0, core->database()->database());
    r = core->databaseQuery()->areas(area_model);
    if(!r) {
	errorHandling(DATABASE_ERROR, core->databaseQuery()->lastErrorNumber());
	return;
    }

    area_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    lv_area->setModel(area_model);
    lv_area->setModelColumn(1);
    if(area_model->rowCount() > 0) {
	pb_remove_area->setEnabled(true);
	pb_edit_area->setEnabled(true);
    }

    // categories
    categories_model = new QSqlTableModel(0, core->database()->database());
    r = core->databaseQuery()->categories(categories_model);
    if(!r) {
	errorHandling(DATABASE_ERROR, core->databaseQuery()->lastErrorNumber());
	return;
    }

    categories_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    lv_categories->setModel(categories_model);
    lv_categories->setModelColumn(1);
    if(categories_model->rowCount() > 0) {
	pb_remove_categories->setEnabled(true);
	pb_edit_categories->setEnabled(true);
    }

    //
    sw_widgets->setCurrentIndex(0);
}

//////////////////////////////////////
// Area
//////////////////////////////////////

void ConfigurationDialog::addArea()
{
    bool ok;
    QString area = QInputDialog::getText(this,
      trUtf8("Agregar Rubro"),
      trUtf8("Nombre del Rubro: "),
      QLineEdit::Normal,
      "",
      &ok);

    if(!ok || area.trimmed().isEmpty())
	return;

    for(int i = 0; i < area_model->rowCount(); i++) {
	QString a = area_model->data(area_model->index(i, 1)).toString();
	if(area.compare(a, Qt::CaseInsensitive) == 0) {
	    QMessageBox::warning(this,
	      trUtf8("Dato duplicado"),
	      trUtf8("El Rubro ya existe."));
	    return;
	}
    }

    QSqlRecord areaRec;
    QSqlField areaField("name", QVariant::String);
    areaField.setValue(area);
    areaRec.append(areaField);
    if(!area_model->insertRecord(-1, areaRec)) {
	if(errorCommon(this, CONFIGURATION_DIALOG, area_model->lastError().number()))
	    return;
    }

    pb_remove_area->setEnabled(area_model->rowCount() > 0);
    pb_edit_area->setEnabled(area_model->rowCount() > 0);
    pb_add_area->setFocus();
}

void ConfigurationDialog::removeArea()
{
    int currentRow = lv_area->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::warning(this,
	  trUtf8("Eliminar"),
	  trUtf8("Seleccione el Rubro que desea eliminar."));
	return;
    }

    QString area = area_model->data(area_model->index(
      currentRow, 1), Qt::DisplayRole).toString();
    int result;
    QString msg = trUtf8("¿Está seguro dar de baja el\nRubro: ");
    msg += area;
    msg += trUtf8("?");
    result = QMessageBox::question(this,
      trUtf8("Confirmación Eliminación"),
      msg,
      QMessageBox::Ok | QMessageBox::Cancel);

    if(result != QMessageBox::Ok)
	return;

    if(!area_model->removeRow(currentRow)) {
	errorHandling(DATABASE_ERROR, 1451);
	return;
    }

    pb_remove_area->setEnabled(area_model->rowCount() > 0);
    pb_edit_area->setEnabled(area_model->rowCount() > 0);
}

void ConfigurationDialog::editArea()
{
    int currentRow = lv_area->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::warning(this,
	  trUtf8("Editar"),
	  trUtf8("Seleccione el Rubro que desea modificar."));
	return;
    }

    QString area = area_model->data(area_model->index(
      currentRow, 1), Qt::DisplayRole).toString();

    bool ok;
    QString areaNew = QInputDialog::getText(this,
      trUtf8("Modificar Rubro"),
      trUtf8("Rubro: "),
      QLineEdit::Normal,
      area,
      &ok);

    if(!ok || areaNew.trimmed().isEmpty())
	return;

    if(area.compare(areaNew, Qt::CaseSensitive) == 0)
	return;

    if(!area_model->setData(area_model->index(currentRow, 1), areaNew, Qt::EditRole))
	errorHandling(-1, -1);
}

//////////////////////////////////////
// Iva Condition
//////////////////////////////////////

void ConfigurationDialog::addCategories()
{
    bool ok;
    QString category = QInputDialog::getText(this,
      trUtf8("Agregar Categoría"),
      trUtf8("Categoría: "),
      QLineEdit::Normal,
      "",
      &ok);

    if(!ok || category.trimmed().isEmpty())
	return;

    for(int i = 0; i < categories_model->rowCount(); i++) {
	QString c = categories_model->data(categories_model->index(i, 1)).toString();
	if(category.compare(c, Qt::CaseInsensitive) == 0) {
	    QMessageBox::warning(this,
	      trUtf8("Dato duplicado"),
	      trUtf8("La categoría ya existe."));
	    return;
	}
    }

    QSqlRecord categoryRec;
    QSqlField categoryField("name", QVariant::String);
    categoryField.setValue(category);
    categoryRec.append(categoryField);
    if(!categories_model->insertRecord(-1, categoryRec)) {
	if(errorCommon(this, CONFIGURATION_DIALOG, categories_model->lastError().number()))
	    return;
    }

    if(!pb_remove_categories->isEnabled()) {
	pb_remove_categories->setEnabled(categories_model->rowCount() > 0);
	pb_edit_categories->setEnabled(categories_model->rowCount() > 0);
    }

    pb_add_categories->setFocus();
}

void ConfigurationDialog::removeCategories()
{
    int currentRow = lv_categories->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::warning(this,
	  trUtf8("Eliminar"),
	  trUtf8("Seleccione la Categoría que desea eliminar."));
	return;
    }

    QString category = categories_model->data(categories_model->index(
			    currentRow, 1), Qt::DisplayRole).toString();
    int result;
    QString msg = trUtf8("¿Está seguro dar de baja la\nCategoría: ");
    msg += category;
    msg += trUtf8("?");
    result = QMessageBox::question(this,
      trUtf8("Confirmación Eliminación"),
      msg,
      QMessageBox::Ok | QMessageBox::Cancel);

    if(result != QMessageBox::Ok)
	return;

    if(!categories_model->removeRow(currentRow)) {
	errorHandling(DATABASE_ERROR, 1451);
	return;
    }

    if(categories_model->rowCount() == 0) {
	pb_remove_categories->setEnabled(false);
	pb_edit_categories->setEnabled(false);
    }
}

void ConfigurationDialog::editCategories()
{
    int currentRow = lv_categories->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::warning(this,
	  trUtf8("Editar"),
	  trUtf8("Seleccione la Categoría que desea modificar."));
	return;
    }

    QString category = categories_model->data(categories_model->index(currentRow,
      1), Qt::DisplayRole).toString();

    bool ok;
    QString categoriesNew = QInputDialog::getText(this,
      trUtf8("Modificar Categorías"),
      trUtf8("Categoría:: "),
      QLineEdit::Normal,
      category,
      &ok);

    if(!ok || categoriesNew.trimmed().isEmpty())
	return;

    if(category.compare(categoriesNew, Qt::CaseSensitive) == 0)
	return;

    if(!categories_model->setData(categories_model->index(currentRow, 1), categoriesNew, Qt::EditRole))
	errorHandling(-1, -1);
}

//////////////////////////////////////
// Pages
//////////////////////////////////////

void ConfigurationDialog::changePage(QListWidgetItem *current,
	QListWidgetItem *previous)
{
    if(!current) {
	current = previous;
    }

    int row = lw_list->row(current);

    QString title;
    QPixmap pixmap;
    switch(row) {
	case ConfigurationDialog::Area:
	    title = trUtf8("Rubro");
	    pixmap.load(QString::fromUtf8(":/app/images/app/area_32.png"));
	    break;
	case ConfigurationDialog::Category:
	    title = trUtf8("Categoría");
	    pixmap.load(QString::fromUtf8(":/app/images/app/category_32.png"));
	    break;
    }

    lb_icon->setPixmap(pixmap);
    lb_title->setText(title);
    sw_widgets->setCurrentIndex(row);
}

//////////////////////////////////////
// Error
//////////////////////////////////////

void ConfigurationDialog::errorHandling(int error_type, int error)
{
    switch(error_type) {
	case DATABASE_ERROR:
	    if(!errorCommon(this, -1, error)) {
		setEnabled(false);
	    }
	    break;
	default:
	    setEnabled(false);
	    QMessageBox::critical(this,
	      trUtf8("Error"),
	      trUtf8("Error desconocido.\nTipo: ") +
	      QString::number(error_type) + " - " +
	      trUtf8("Valor: ") + QString::number(error));
	    break;
    }
}
