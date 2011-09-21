/*
 *
 *
 *
 */

#include "employeddlg.h"

#include <QMessageBox>
#include <QSqlRelationalDelegate>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include "common.h"
#include "employedmodel.h"

EmployedDlg::EmployedDlg(QWidget *parent, const QString &title, int selectedRow,
      EmployedModel *model)
    : QDialog(parent)
    , selectedRow(selectedRow)
    , model(model)
{
    setupUi(this);
    setWindowTitle(title);
    mapper = 0;

    cb_category->setModel(model->relationModel(EmployedModel::idCategory));
    cb_category->setModelColumn(model->relationModel(EmployedModel::idCategory)->fieldIndex("name"));

    if(selectedRow >= 0) {
	mapper = new QDataWidgetMapper(this);
	mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
	mapper->setModel(model);
	mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
	mapper->addMapping(le_lastName, EmployedModel::LastName);
	mapper->addMapping(le_name, EmployedModel::Name);
	mapper->addMapping(cb_category, EmployedModel::idCategory);
	mapper->addMapping(le_dni, EmployedModel::Dni);
	mapper->addMapping(le_address, EmployedModel::Address);
	mapper->addMapping(le_location, EmployedModel::Location);
	mapper->addMapping(le_cellphone, EmployedModel::CellPhone);
	mapper->addMapping(le_phone, EmployedModel::Phone);
	mapper->addMapping(cb_rating, EmployedModel::Rating, "currentIndex");
	mapper->setCurrentIndex(selectedRow);

	le_lastName->selectAll();
    } else {
	cb_category->setCurrentIndex(0);
    }

    connect(pb_ok, SIGNAL(clicked()),
	    this, SLOT(accept()));
    connect(pb_cancel, SIGNAL(clicked()),
	    this, SLOT(reject()));
    connect(tb_categoryUpdateCombo, SIGNAL(clicked()),
	    this, SLOT(categoryUpdateCombo()));
}

EmployedDlg::~EmployedDlg()
{
    if(mapper)
	delete mapper;
}

void EmployedDlg::accept()
{
    if(le_name->text().trimmed().isEmpty() || le_lastName->text().trimmed().isEmpty() ||
	cb_category->currentIndex() == 0 || le_dni->text().trimmed().isEmpty()) {
	QMessageBox::information(this,
	  trUtf8("Datos incompletos"),
	  trUtf8("Debe llenar al menos los campos obligatorios."));
	le_lastName->setFocus();
	return;
    }

    if(selectedRow >= 0) {
	mapper->submit();
    } else {
	QSqlRecord record;

	QSqlField lastNameField("lastName", QVariant::String);
	QSqlField nameField("name", QVariant::String);
	QSqlField categoryField("idCategory", QVariant::Int);
	QSqlField dniField("dni", QVariant::String);
	QSqlField addressField("address", QVariant::String);
	QSqlField locationField("location", QVariant::String);
	QSqlField cellPhoneField("cellphone", QVariant::String);
	QSqlField phoneField("phone", QVariant::String);
	QSqlField ratingField("rating", QVariant::Int);

	int idC = cb_category->model()->data(cb_category->model()->index(
	  cb_category->currentIndex(), 0)).toInt();
	int idR = cb_rating->currentIndex();

	lastNameField.setValue(le_lastName->text());
	nameField.setValue(le_name->text());
	categoryField.setValue(idC);
	dniField.setValue(le_dni->text());
	addressField.setValue(le_address->text());
	locationField.setValue(le_location->text());
	cellPhoneField.setValue(le_cellphone->text());
	phoneField.setValue(le_phone->text());
	ratingField.setValue(idR);

	record.append(lastNameField);
	record.append(nameField);
	record.append(categoryField);
	record.append(dniField);
	record.append(addressField);
	record.append(locationField);
	record.append(cellPhoneField);
	record.append(phoneField);
	record.append(ratingField);

	if(!model->insertRecord(-1, record)) {
	    model->revertAll();
	    if(errorCommon(this, EMPLOYED_WINDOW, model->lastError().number())) {
		return;
	    }
	}
    }

    QDialog::accept();
}

void EmployedDlg::categoryUpdateCombo()
{
    emit categoryUpdate();
    cb_category->setCurrentIndex(0);
}
