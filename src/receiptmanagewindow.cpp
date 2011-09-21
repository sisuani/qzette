/*
 *
 *
 *
 */

#include "receiptmanagewindow.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>
#include <QValidator>
#include <QSqlRelationalDelegate>
#include <QDebug>
#include <QSqlError>

#include "createreceiptdlg.h"
#include "receiptdlg.h"
#include "employedsearchdlg.h"
#include "employedmodel.h"
#include "buildingsearchdlg.h"
#include "buildingmanagewindow.h"

ReceiptManageWindow::ReceiptManageWindow(QWidget *parent, Core *core)
    : QWidget(parent),
      core(core)
{
    setupUi(this);

    proxyModel = new ProxyModel();
    proxyModel->setDynamicSortFilter(true);
    proxyModel->filterBetweenColumns(idEmployed, Concept);

    tableModel = 0;
    receiptModel = 0;
    employedModel = 0;
    buildingModel = 0;

    QTimer::singleShot(0, this, SLOT(init()));
}

ReceiptManageWindow::~ReceiptManageWindow()
{
    if(tableModel)
	delete tableModel;

    if(receiptModel)
	delete receiptModel;

    if(proxyModel)
	delete proxyModel;

    if(employedModel)
	delete employedModel;

    if(buildingModel)
	delete buildingModel;
}

void ReceiptManageWindow::closeWindow()
{
    emit closeWindow(this);
}

void ReceiptManageWindow::init()
{
    bool r;
    receiptModel = new QSqlTableModel(0, core->database()->database());
    r = core->databaseQuery()->receipts(receiptModel);
    if(!r) {
	errorHandling(DATABASE_ERROR, core->databaseQuery()->lastErrorNumber());
	return;
    }

    //
    employedModel = new QSqlTableModel(0, core->database()->database());
    r = core->databaseQuery()->employeesTable(employedModel);
    if(!r) {
	errorHandling(DATABASE_ERROR, core->databaseQuery()->lastErrorNumber());
	return;
    }

    //
    buildingModel = new QSqlTableModel(0, core->database()->database());
    r = core->databaseQuery()->buildings(buildingModel);
    if(!r) {
	errorHandling(DATABASE_ERROR, core->databaseQuery()->lastErrorNumber());
	return;
    }

    //
    updateCombos();

    // Table Model
    tableModel = new QStandardItemModel();
    tableModel->setColumnCount(6);
    tableModel->setHeaderData(T_Date, Qt::Horizontal, trUtf8("Fecha"));
    tableModel->setHeaderData(T_NReceipt, Qt::Horizontal, trUtf8("Nº Recibo"));
    tableModel->setHeaderData(T_OwnerName, Qt::Horizontal, trUtf8("Emitido por"));
    tableModel->setHeaderData(T_Concept, Qt::Horizontal, trUtf8("Concepto"));
    tableModel->setHeaderData(T_Amount, Qt::Horizontal, trUtf8("Monto"));
    tableModel->setHeaderData(T_Subtotal, Qt::Horizontal, trUtf8("Total"));
    proxyModel->setSourceModel(tableModel);
    tv_receipt->setModel(proxyModel);
    tv_receipt->verticalHeader()->hide();
    tv_receipt->horizontalHeader()->setResizeMode(T_Concept, QHeaderView::Stretch);

    tb_remove->setEnabled(false);
    tb_view->setEnabled(false);

    buildingComboChanged(0);
    employedComboChanged(0);

    connect(tb_add, SIGNAL(clicked()),
	    this, SLOT(addReceipt()));
    connect(tb_remove, SIGNAL(clicked()),
	    this, SLOT(removeReceipt()));
    connect(tb_close, SIGNAL(clicked()),
	    this, SLOT(closeWindow()));
    connect(tb_view, SIGNAL(clicked()),
	    this, SLOT(viewReceipt()));
    connect(tv_receipt, SIGNAL(doubleClicked(const QModelIndex &)),
	    this, SLOT(viewReceipt()));
    connect(le_filter, SIGNAL(textChanged(const QString &)),
	    this, SLOT(filterChanged(const QString &)));
    connect(le_filter, SIGNAL(returnPressed()),
	    this, SLOT(filterReturnPressed()));
    connect(pb_employedSearch, SIGNAL(clicked()),
	    this, SLOT(searchEmployed()));
    connect(pb_buildingSearch, SIGNAL(clicked()),
	    this, SLOT(searchBuilding()));
    connect(buildingRadioButton, SIGNAL(clicked()),
	    this, SLOT(radioButtonClicked()));
    connect(extraRadioButton, SIGNAL(clicked()),
	    this, SLOT(radioButtonClicked()));
    connect(cb_building, SIGNAL(currentIndexChanged(int)),
	    this, SLOT(buildingComboChanged(int)));
    connect(cb_employed, SIGNAL(currentIndexChanged(int)),
	    this, SLOT(employedComboChanged(int)));

    le_filter->setFocus();
}

void ReceiptManageWindow::updateData()
{
    findReceipt();
    updateCombos();
}

void ReceiptManageWindow::updateCombos()
{
    employedModel->select();
    buildingModel->select();

    //
    int employedSize = employedModel->rowCount();
    if(!employedSize) {
	QMessageBox::critical(0,
	  trUtf8("Datos incompletos"),
	  trUtf8("Para ingresar recibos, ingrese primero el/los empleado(s)."));
    }

    int buildingSize = buildingModel->rowCount();
    if(!buildingSize) {
	QMessageBox::critical(0,
	  trUtf8("Datos incompletos"),
	  trUtf8("Para ingresar recibos, ingrese primero la obra."));
    }

    //
    cb_employed->setModel(employedModel);
    cb_employed->setModelColumn(1);
    cb_employed->insertItem(0, trUtf8("-- Seleccione Empleado --"));
    cb_employed->setCurrentIndex(0);
    cb_building->setModel(buildingModel);
    cb_building->setModelColumn(1);
    cb_building->insertItem(0, trUtf8("-- Seleccione Obra --"));
    cb_building->setCurrentIndex(0);
}

void ReceiptManageWindow::radioButtonClicked()
{
    findReceipt();
}

void ReceiptManageWindow::filterChanged(const QString &text)
{
    QRegExp regExp(text, Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterRegExp(regExp);
}

void ReceiptManageWindow::addReceipt()
{
    CreateReceiptDlg *receiptDlg = new CreateReceiptDlg(this, receiptModel,
      employedModel, buildingModel);
    int n = receiptDlg->exec();
    delete receiptDlg;

    if(n == QDialog::Rejected)
	return;

    findReceipt();
    le_filter->setFocus();
}

void ReceiptManageWindow::removeReceipt()
{
    int currentRow = tv_receipt->selectionModel()->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::critical(this,
	  trUtf8("Error"),
	  trUtf8("Debe seleccionar el Recibo que desea eliminar."));
	return;
    }

    currentRow = proxyModel->mapToSource(proxyModel->index(currentRow, 0)).row();
    QString receipt = receiptModel->data(receiptModel->index(currentRow, id)).toString();

    QString msg;
    msg = trUtf8("¿Está seguro que desea eliminar el recibo Nº: ");
    msg += receipt + trUtf8("?");

    int r = QMessageBox::warning(this,
      trUtf8("Confirme la eliminación"),
      msg,
      QMessageBox::Ok | QMessageBox::Cancel);
    if(r != QMessageBox::Ok)
	return;

    if(!receiptModel->removeRow(currentRow)) {
	errorHandling(DATABASE_ERROR, receiptModel->lastError().number());
	return;
    }

    findReceipt();
    le_filter->setFocus();
}

void ReceiptManageWindow::viewReceipt()
{
    int currentRow = tv_receipt->selectionModel()->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::warning(this,
	  trUtf8("Recibo no seleccionado"),
	  trUtf8("Seleccione el recibio que desea ver."));
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
    QString receiptNum = tableModel->data(tableModel->index(currentRow,
      T_NReceipt)).toString();
    QString ownerName = tableModel->data(tableModel->index(currentRow,
      T_OwnerName)).toString();
    QDate date = tableModel->data(tableModel->index(currentRow,
      T_Date)).toDate();
    double amount = tableModel->data(tableModel->index(currentRow,
      T_Amount)).toDouble();
    QString concept = tableModel->data(tableModel->index(currentRow,
      T_Concept)).toString();
    QString buildingName = buildingModel->data(buildingModel->index(
      cb_building->currentIndex(), BuildingManageWindow::Name)).toString();
    QString buildingLocation = buildingModel->data(buildingModel->index(
      cb_building->currentIndex(), BuildingManageWindow::Address)).toString();

    ReceiptDlg *receipt = new ReceiptDlg(this, receiptNum, ownerName, date, employed,
      amount, buildingName, buildingLocation, concept);
    receipt->show();
}

void ReceiptManageWindow::filterReturnPressed()
{
    if(proxyModel->rowCount() > 0) {
	tv_receipt->selectRow(0);
	tv_receipt->setFocus();
	if(proxyModel->rowCount() == 1) {
	    viewReceipt();
	}
    }
}

void ReceiptManageWindow::searchBuilding()
{
    int row = -1;
    BuildingSearchDlg *bs = new BuildingSearchDlg(this, buildingModel, &row);
    int n = bs->exec();
    if(n == QDialog::Rejected)
	return;
    cb_building->setCurrentIndex(row);
}

void ReceiptManageWindow::searchEmployed()
{
    int row = -1;
    EmployedSearchDlg *es = new EmployedSearchDlg(this, employedModel, &row);
    int n = es->exec();
    if(n == QDialog::Rejected)
	return;
    cb_employed->setCurrentIndex(row);
}

void ReceiptManageWindow::buildingComboChanged(int index)
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

    findReceipt();
}

void ReceiptManageWindow::employedComboChanged(int index)
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

    findReceipt();
}

void ReceiptManageWindow::findReceipt()
{
    receiptModel->select();
    int buildingIndex = cb_building->currentIndex();
    int employedIndex = cb_employed->currentIndex();
    if(!buildingIndex || !employedIndex) {
	if(tableModel->rowCount()) {
	    tableModel->setRowCount(0);
	    tb_remove->setEnabled(false);
	    tb_view->setEnabled(false);
	}
	return;
    }

    QString filter;
    filter += "idEmployed='" + employedModel->data(employedModel->index(
      employedIndex, EmployedModel::id)).toString() + "' AND ";
    filter += "idBuilding='" + buildingModel->data(buildingModel->index(
      buildingIndex, BuildingManageWindow::id)).toString() + "' AND ";
    if(extraRadioButton->isChecked())
	filter += "type='extra'";
    else
	filter += "type='obra'";

    receiptModel->setFilter(filter);
    updateTableModel();
    tb_remove->setEnabled(tableModel->rowCount() > 0);
    tb_view->setEnabled(tableModel->rowCount() > 0);
}

void ReceiptManageWindow::updateTableModel()
{
    tableModel->setRowCount(receiptModel->rowCount());
    double subtotal = 0.0;
    for(int i = 0; i < receiptModel->rowCount(); i++) {
	QString date = receiptModel->data(receiptModel->index(i,
	  Date)).toString();
	QString nReceipt = receiptModel->data(receiptModel->index(i,
	  id)).toString();
    QString ownerName = receiptModel->data(receiptModel->index(i,
      OwnerName)).toString();
	QString concept = receiptModel->data(receiptModel->index(i,
	  Concept)).toString();
	double amount = receiptModel->data(receiptModel->index(i,
	  Amount)).toDouble();
	subtotal += amount;

	tableModel->setData(tableModel->index(i, T_Date), date);
	tableModel->setData(tableModel->index(i, T_NReceipt), nReceipt);
  	tableModel->setData(tableModel->index(i, T_OwnerName), ownerName);
	tableModel->setData(tableModel->index(i, T_Concept), concept);
	tableModel->setData(tableModel->index(i, T_Amount), QString::number(amount, 'f', 2));
	tableModel->setData(tableModel->index(i, T_Subtotal), QString::number(subtotal, 'f', 2));
    }
}

QString ReceiptManageWindow::toHtml(bool *ok)
{
    if(!proxyModel->rowCount()) {
	int a = QMessageBox::question(this,
		  trUtf8("Datos vacíos"),
		  trUtf8("La planilla de Recibos no contiene datos.\n"
		    "¿seguro desea imprimirla?"),
		  QMessageBox::Yes | QMessageBox::No);
	if(a == QMessageBox::No)
	    return "";
    }

    QString html = "<html>";
    html += "<head>";
    html += "<style type=\"text/css\"> ";
    html += "table{";
    html += "border-width: 1px;";
    html += "padding: 0px;";
    html += "border-style: solid;";
    html += "border-color: #000000;";
    html += "background-color: white;";
    html += "}";
    html += "</style>";
    html += "</head>";
    html += "<body align=center>";
    html += "<h2 align=center><strong>" + trUtf8("Lista de Recibos") + "</strong></h2>";
    html += "<table align=\"left\">";
    html += "<tr><td><strong>Empleado: </strong></td><td>"+cb_employed->currentText()+"</td></tr>";
    html += "<tr><td><strong>Obra: </strong></td><td>"+cb_building->currentText()+"</td></tr>";
    html += "</table>";
    html += "<table cellpadding=\"2\" cellspacing=\"0\">";
    html += "<tr>";
    html += "<th>"+trUtf8("Fecha")+"</th>";
    html += "<th>"+trUtf8("Nº Recibo")+"</th>";
    html += "<th>"+trUtf8("Concepto")+"</th>";
    html += "<th>"+trUtf8("Monto")+"</th>";
    html += "<th>"+trUtf8("Total")+"</th>";
    html += "</tr>";

    for(int i = 0; i < proxyModel->rowCount(); i++) {
	html += "<tr>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, T_Date)).toString()+"</td>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, T_NReceipt)).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, T_Concept)).toString()+"</td>";
	html += "<td align=right>$"+proxyModel->data(proxyModel->index(i, T_Amount)).toString()+"</td>";
	html += "<td align=right>$"+proxyModel->data(proxyModel->index(i, T_Subtotal)).toString()+"</td>";
	html += "</tr>";
    }
    html += "</table></body></html>";
    *ok = true;
    return html;

    return "";
}

void ReceiptManageWindow::errorHandling(int error_type, int error)
{
    switch(error_type) {
	case DATABASE_ERROR:
	    if(!errorCommon(this, RECEIPT_WINDOW, error)) {
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
