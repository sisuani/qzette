/*
 *
 *
 *
 */

#include "customerdlg.h"

#include <QMessageBox>
#include <QRegExp>
#include <QValidator>
#include <QRegExpValidator>
#include <QSqlField>
#include <QSqlError>

#include "customermanagewindow.h"

CustomerDlg::CustomerDlg(QWidget *parent, const QString &title, int selectedRow,
  QSqlTableModel *model)
    : QDialog(parent)
    , selectedRow(selectedRow)
    , model(model)
{
    setupUi(this);
    setWindowTitle(title);
    mapper = 0;

    QRegExp regExpMail("([\\w\\.]*@[\\w\\.]*)");
    QValidator *emailValidator = new QRegExpValidator(regExpMail, this);
    le_email->setValidator(emailValidator);

    if(selectedRow >= 0) {
	mapper = new QDataWidgetMapper(this);
	mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
	mapper->setModel(model);
	mapper->addMapping(le_lastName, CustomerManageWindow::LastName);
	mapper->addMapping(le_name, CustomerManageWindow::Name);
	mapper->addMapping(le_address, CustomerManageWindow::Address);
	mapper->addMapping(le_location, CustomerManageWindow::Location);
	mapper->addMapping(le_cellphone, CustomerManageWindow::CellPhone);
	mapper->addMapping(le_phone, CustomerManageWindow::Phone);
	mapper->addMapping(le_email, CustomerManageWindow::Email);
	mapper->setCurrentIndex(selectedRow);

	le_lastName->selectAll();
    }

    connect(pb_ok, SIGNAL(clicked()),
	    this, SLOT(accept()));
    connect(pb_cancel, SIGNAL(clicked()),
	    this, SLOT(reject()));
}

CustomerDlg::~CustomerDlg()
{
    if(mapper)
	delete mapper;
}

void CustomerDlg::accept()
{
    if(le_lastName->text().trimmed().isEmpty()) {
	QMessageBox::information(this,
	  trUtf8("Datos incompletos"),
	  trUtf8("Debe llenar al menos los campos obligatorios."));
	le_lastName->setFocus();
	return;
    }

    if(selectedRow >= 0) {
	mapper->submit();
    } else {
	QSqlRecord rec;
	QSqlField lastNameFiel("lastName", QVariant::String);
	QSqlField nameField("name", QVariant::String);
	QSqlField addressField("address", QVariant::String);
	QSqlField locationField("location", QVariant::String);
	QSqlField cellPoneField("cellphone", QVariant::String);
	QSqlField phoneField("phone", QVariant::String);
	QSqlField emailField("email", QVariant::String);

	lastNameFiel.setValue(le_lastName->text());
	nameField.setValue(le_name->text());
	addressField.setValue(le_address->text());
	locationField.setValue(le_location->text());
	cellPoneField.setValue(le_cellphone->text());
	phoneField.setValue(le_phone->text());
	emailField.setValue(le_email->text());

	rec.append(lastNameFiel);
	rec.append(nameField);
	rec.append(addressField);
	rec.append(locationField);
	rec.append(cellPoneField);
	rec.append(phoneField);
	rec.append(emailField);

	if(!model->insertRecord(-1, rec)) {
	    model->revertAll();
	    if(errorCommon(this, CUSTOMER_WINDOW, model->lastError().number()))
		return;
	}
    }

    QDialog::accept();
}
