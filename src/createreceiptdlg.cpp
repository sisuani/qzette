/*
 *
 *
 *
 */

#include "createreceiptdlg.h"

#include <QMessageBox>
#include <QSqlField>
#include <QSqlError>

#include "receiptmanagewindow.h"
#include "receiptdlg.h"
#include "employedsearchdlg.h"
#include "employedmodel.h"
#include "buildingsearchdlg.h"
#include "buildingmanagewindow.h"

CreateReceiptDlg::CreateReceiptDlg(QWidget *parent, QSqlTableModel *model,
  QSqlTableModel *employedModel, QSqlTableModel *buildingModel)
    : QDialog(parent)
    , model(model)
    , employedModel(employedModel)
    , buildingModel(buildingModel)
{
    setupUi(this);

    //
    cb_employed->insertItem(0, trUtf8("-- Seleccione Empleado --"));
    cb_employed->setModel(employedModel);
    cb_employed->setModelColumn(1);
    cb_employed->setCurrentIndex(0);
    cb_building->insertItem(0, trUtf8("-- Seleccione Obra --"));
    cb_building->setModel(buildingModel);
    cb_building->setModelColumn(1);
    cb_building->setCurrentIndex(0);

    //
    dateEdit->setDate(QDate::currentDate());

    //
    buildingComboChanged(0);
    employedComboChanged(0);

    connect(pb_ok, SIGNAL(clicked()),
	    this, SLOT(accept()));
    connect(pb_cancel, SIGNAL(clicked()),
	    this, SLOT(reject()));
    connect(cb_building, SIGNAL(currentIndexChanged(int)),
	    this, SLOT(buildingComboChanged(int)));
    connect(cb_employed, SIGNAL(currentIndexChanged(int)),
	    this, SLOT(employedComboChanged(int)));
    connect(pb_employedSearch, SIGNAL(clicked()),
	    this, SLOT(searchEmployed()));
    connect(pb_buildingSearch, SIGNAL(clicked()),
	    this, SLOT(searchBuilding()));

}

CreateReceiptDlg::~CreateReceiptDlg()
{
}

void CreateReceiptDlg::accept()
{
  if (le_ownername->text().isEmpty()) {
    QMessageBox::critical(this,
      trUtf8("Datos incompletos"),
      trUtf8("El campo \"Emisión a nombre de\" no puede ser vacío"));
    le_ownername->setFocus();
    return;
  }
    if(!cb_building->currentIndex()) {
	QMessageBox::critical(this,
	  trUtf8("Datos incompletos"),
	  trUtf8("Seleccione la obra de la que se va generar el recibo."));
	cb_building->setFocus();
	return;
    }

    if(!cb_employed->currentIndex()) {
	QMessageBox::critical(this,
	  trUtf8("Datos incompletos"),
	  trUtf8("Seleccione el empleado al que le va generar el recibo."));
	cb_employed->setFocus();
	return;
    }

    if(amountSpinBox->value() == 0) {
	QMessageBox::critical(this,
	  trUtf8("Datos incompletos"),
	  trUtf8("Debe ingresar el monto."));
	return;
	amountSpinBox->setFocus();
    }

    int idEmployed = employedModel->data(employedModel->index(
      cb_employed->currentIndex(), 0)).toInt();
    int idBuilding = buildingModel->data(buildingModel->index(
      cb_building->currentIndex(), 0)).toInt();

    QSqlRecord rec;
    QSqlField employedFiel("idEmployed", QVariant::Int);
    QSqlField buildingField("idBuilding", QVariant::Int);
    QSqlField ownernameField("ownername", QVariant::String);
    QSqlField dateField("date", QVariant::Date);
    QSqlField typeField("type", QVariant::String);
    QSqlField amountField("amount", QVariant::Double);
    QSqlField conceptField("concept", QVariant::String);

    employedFiel.setValue(idEmployed);
    buildingField.setValue(idBuilding);
    ownernameField.setValue(le_ownername->text());
    dateField.setValue(dateEdit->date());
    if(extraRadioButton->isChecked())
	typeField.setValue("extra");
    else
	typeField.setValue("obra");
    amountField.setValue(amountSpinBox->value());
    conceptField.setValue(conceptTextBrowser->toPlainText());

    rec.append(employedFiel);
    rec.append(buildingField);
    rec.append(ownernameField);
    rec.append(dateField);
    rec.append(typeField);
    rec.append(amountField);
    rec.append(conceptField);

    if(!model->insertRecord(-1, rec)) {
	model->revertAll();
	if(errorCommon(this, CREATERECEIPT_WINDOW, model->lastError().number()))
	    return;
    }

    int currentEmployed = cb_employed->currentIndex();
    if(!currentEmployed)
	return;

    QString employed = employedModel->data(employedModel->index(currentEmployed,
      EmployedModel::LastName)).toString();
    employed += trUtf8(", ");
    employed += employedModel->data(employedModel->index(currentEmployed,
      EmployedModel::Name)).toString();
    QString receiptNum = model->query().lastInsertId().toString();
    QString buildingName = buildingModel->data(buildingModel->index(
      cb_building->currentIndex(), BuildingManageWindow::Name)).toString();
    QString buildingLocation = buildingModel->data(buildingModel->index(
      cb_building->currentIndex(), BuildingManageWindow::Address)).toString();

    ReceiptDlg *receipt = new ReceiptDlg(parentWidget(), receiptNum, le_ownername->text(), dateEdit->date(), employed,
      amountSpinBox->value(), buildingName, buildingLocation, conceptTextBrowser->toPlainText());
    receipt->show();

    QDialog::accept();
}

void CreateReceiptDlg::searchBuilding()
{
    int row = -1;
    BuildingSearchDlg *bs = new BuildingSearchDlg(this, buildingModel, &row);
    int n = bs->exec();
    if(n == QDialog::Rejected)
	return;
    cb_building->setCurrentIndex(row);
}

void CreateReceiptDlg::searchEmployed()
{
    int row = -1;
    EmployedSearchDlg *es = new EmployedSearchDlg(this, employedModel, &row);
    int n = es->exec();
    if(n == QDialog::Rejected)
	return;
    cb_employed->setCurrentIndex(row);
}

void CreateReceiptDlg::buildingComboChanged(int index)
{
    if(index == 0) {
	lb_building->setText("<span style=\" color:#aa0000;\"> " +
	  trUtf8("Obra no seleccionada.") + "</span>");
    } else {
	QString name = buildingModel->data(buildingModel->index(index,
	  BuildingManageWindow::Name)).toString();
	QString str = "<span style=\"font-weight:600;\">";
	str += name + "</span>";
	lb_building->setText(str);
    }
}

void CreateReceiptDlg::employedComboChanged(int index)
{
    if(index == 0) {
	lb_employed->setText("<span style=\" color:#aa0000;\"> " +
	  trUtf8("Empleado no seleccionado.") + "</span>");
    } else {
	QString lastName = employedModel->data(employedModel->index(index,
	  EmployedModel::LastName)).toString();
	QString name = employedModel->data(employedModel->index(index,
	  EmployedModel::Name)).toString();
	QString dni = employedModel->data(employedModel->index(index,
	  EmployedModel::Dni)).toString();
	QString str = "<span style=\"font-weight:600;\">";
	str += lastName + ", " + name + " - " + dni + "</span>";
	lb_employed->setText(str);
    }
}

