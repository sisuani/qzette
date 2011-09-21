/*
 *
 *
 *
 */

#include "supplierdlg.h"

#include <QMessageBox>
#include <QSqlRelationalDelegate>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include "common.h"
#include "suppliermanagewindow.h"

SupplierDlg::SupplierDlg(QWidget *parent, const QString &title, int selectedRow,
      QSqlRelationalTableModel *model)
    : QDialog(parent)
    , selectedRow(selectedRow)
    , model(model)
{
    setupUi(this);
    setWindowTitle(title);
    mapper = 0;

    cb_area->setModel(model->relationModel(SupplierManageWindow::idArea));
    cb_area->setModelColumn(model->relationModel(SupplierManageWindow::idArea)->fieldIndex("name"));

    if(selectedRow >= 0) {
	mapper = new QDataWidgetMapper(this);
	mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
	mapper->setModel(model);
	mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
	mapper->addMapping(le_name, SupplierManageWindow::Name);
	mapper->addMapping(cb_area, SupplierManageWindow::idArea);
	mapper->addMapping(le_address, SupplierManageWindow::Address);
	mapper->addMapping(le_location, SupplierManageWindow::Location);
	mapper->addMapping(le_phone, SupplierManageWindow::Phone);
	mapper->addMapping(le_fax, SupplierManageWindow::Fax);
	mapper->addMapping(le_email, SupplierManageWindow::Email);
	mapper->addMapping(le_contactName, SupplierManageWindow::ContactName);
	mapper->addMapping(le_contactPhone, SupplierManageWindow::ContactPhone);
	mapper->setCurrentIndex(selectedRow);

	le_name->selectAll();
    } else {
	cb_area->setCurrentIndex(0);
    }

    connect(pb_ok, SIGNAL(clicked()),
	    this, SLOT(accept()));
    connect(pb_cancel, SIGNAL(clicked()),
	    this, SLOT(reject()));
    connect(tb_areaUpdateCombo, SIGNAL(clicked()),
	    this, SLOT(areaUpdateCombo()));
}

SupplierDlg::~SupplierDlg()
{
    if(mapper)
	delete mapper;
}

void SupplierDlg::accept()
{
    if(le_name->text().trimmed().isEmpty() || !cb_area->currentIndex()) {
	QMessageBox::information(this,
	  trUtf8("Alta Proveedor"),
	  trUtf8("Debe llenar al menos los campos obligatorios."));
	le_name->setFocus();
	return;
    }

    if(selectedRow >= 0) {
	mapper->submit();
    } else {
	QSqlRecord record;

	QSqlField nameField("name", QVariant::String);
	QSqlField areaField("idArea", QVariant::Int);
	QSqlField addressField("address", QVariant::String);
	QSqlField locationField("location", QVariant::String);
	QSqlField phoneField("phone", QVariant::String);
	QSqlField faxField("fax", QVariant::String);
	QSqlField emailField("email", QVariant::String);
	QSqlField contactNameField("contactName", QVariant::String);
	QSqlField contactPhoneField("contactPhone", QVariant::String);

	int idA = cb_area->model()->data(cb_area->model()->index(
	  cb_area->currentIndex(), 0)).toInt();

	nameField.setValue(le_name->text());
	areaField.setValue(idA);
	addressField.setValue(le_address->text());
	locationField.setValue(le_location->text());
	phoneField.setValue(le_phone->text());
	faxField.setValue(le_fax->text());
	emailField.setValue(le_email->text());
	contactNameField.setValue(le_contactName->text());
	contactPhoneField.setValue(le_contactPhone->text());

	record.append(nameField);
	record.append(areaField);
	record.append(addressField);
	record.append(locationField);
	record.append(phoneField);
	record.append(faxField);
	record.append(emailField);
	record.append(contactNameField);
	record.append(contactPhoneField);

	if(!model->insertRecord(-1, record)) {
	    model->revertAll();
	    if(errorCommon(this, SUPPLIER_WINDOW, model->lastError().number())) {
		return;
	    }
	}
    }

    QDialog::accept();
}

void SupplierDlg::areaUpdateCombo()
{
    emit areaUpdate();
    cb_area->setCurrentIndex(0);
}
