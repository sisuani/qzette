/*
 *
 *
 *
 */

#include "buildingdlg.h"

#include <QMessageBox>
#include <QSqlRelationalDelegate>
#include <QStringListModel>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QDebug>

#include "common.h"
#include "buildingmanagewindow.h"
#include "customersearchdlg.h"
#include "customermanagewindow.h"

BuildingDlg::BuildingDlg(QWidget *parent, const QString &title, int selectedRow,
      QSqlTableModel *model, QSqlTableModel *customerModel)
    : QDialog(parent)
    , selectedRow(selectedRow)
    , model(model)
    , customerModel(customerModel)
{
    setupUi(this);
    setWindowTitle(title);
    mapper = 0;
    le_customerID->hide();

    QStringListModel *typeModel = new QStringListModel();
    QStringList typeList;
    typeList << trUtf8("Ampliación") << trUtf8("Construcción");
    typeModel->setStringList(typeList);
    cb_type->setModel(typeModel);

    if(selectedRow >= 0) {
	setCustomer(getCustomerRow());
	setBuildingType();
	mapper = new QDataWidgetMapper(this);
	mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
	mapper->setModel(model);
	mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
	mapper->addMapping(le_name, BuildingManageWindow::Name);
	mapper->addMapping(le_customerID, BuildingManageWindow::idCustomer);
	mapper->addMapping(le_address, BuildingManageWindow::Address);
	mapper->addMapping(le_location, BuildingManageWindow::Location);
	mapper->addMapping(cb_type, BuildingManageWindow::Type, "currentText");
	mapper->setCurrentIndex(selectedRow);

	le_name->selectAll();
    } else {
	setCustomer(-1);
    }

    connect(pb_ok, SIGNAL(clicked()),
	    this, SLOT(accept()));
    connect(pb_cancel, SIGNAL(clicked()),
	    this, SLOT(reject()));
    connect(pb_customerSearch, SIGNAL(clicked()),
	    this, SLOT(customerSearch()));
}

BuildingDlg::~BuildingDlg()
{
    if(mapper)
	delete mapper;
}

void BuildingDlg::accept()
{
    if(le_name->text().trimmed().isEmpty()) {
	QMessageBox::information(this,
	  trUtf8("Alta Obra"),
	  trUtf8("Debe llenar al menos los campos obligatorios."));
	le_name->setFocus();
	return;
    }

    if(idCustomer < 0) {
	QMessageBox::critical(this,
	  trUtf8("Alta Obra"),
	  trUtf8("Debe seleccionar el Cliente de la Obra."));
	pb_customerSearch->setFocus();
	return;
    }

    if(selectedRow >= 0) {
	mapper->submit();
    } else {
	QSqlRecord record;

	QSqlField nameField("name", QVariant::String);
	QSqlField customerField("idCustomer", QVariant::Int);
	QSqlField addressField("address", QVariant::String);
	QSqlField locationField("location", QVariant::String);
	QSqlField typeField("type", QVariant::String);

	nameField.setValue(le_name->text());
	customerField.setValue(idCustomer);
	addressField.setValue(le_address->text());
	locationField.setValue(le_location->text());
	typeField.setValue(cb_type->currentText());

	record.append(nameField);
	record.append(customerField);
	record.append(addressField);
	record.append(locationField);
	record.append(typeField);

	if(!model->insertRecord(-1, record)) {
	    model->revertAll();
	    if(errorCommon(this, BUILDING_WINDOW, model->lastError().number())) {
		return;
	    }
	}
    }

    QDialog::accept();
}

void BuildingDlg::customerSearch()
{
    int row = -1;
    CustomerSearchDlg *customerSearchDlg = new CustomerSearchDlg(this, customerModel, &row);
    int n = customerSearchDlg->exec();
    if(n == QDialog::Accepted && row >= 0)
	setCustomer(row);
}

void BuildingDlg::setCustomer(int row)
{
    QString text;
    if(row < 0) {
	idCustomer = -1;
	text = "<span style=\" color:#aa0000;\"> ";
	text += trUtf8("Cliente no seleccionado.");
	text += "</span>";
	lb_customer->setText(text);
    } else {
	int idC = customerModel->data(customerModel->index(row,
	  CustomerManageWindow::id)).toInt();
	QString lastName = customerModel->data(customerModel->index(row,
	  CustomerManageWindow::LastName)).toString();
	QString name = customerModel->data(customerModel->index(row,
	  CustomerManageWindow::Name)).toString();

	//
	idCustomer = idC;
	le_customerID->setText(QString::number(idC));
	//
	QString text = "<span style=\" color:#0000AA;\"> ";
	text += lastName + trUtf8(", ") + name + "</span>";
	lb_customer->setText(text);

    }
}

int BuildingDlg::getCustomerRow()
{
    int id = model->data(model->index(selectedRow,
      BuildingManageWindow::idCustomer)).toInt();

    for(int i = 0; i < customerModel->rowCount(); i++) {
	int idC = customerModel->data(customerModel->index(i, 0)).toInt();
	if(id == idC)
	    return i;
    }
    return -1;
}

void BuildingDlg::setBuildingType()
{
    QString buildingType = model->data(model->index(selectedRow,
      BuildingManageWindow::Type)).toString();
    cb_type->setCurrentIndex(cb_type->findText(buildingType));
}
