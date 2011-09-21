/*
 *
 *
 *
 */

#include "suppliermanagewindow.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>
#include <QValidator>
#include <QSqlRelationalDelegate>
#include <QDebug>
#include <QSqlError>

#include "supplierdlg.h"

SupplierManageWindow::SupplierManageWindow(QWidget *parent, Core *core)
    : QWidget(parent),
      core(core)
{
    setupUi(this);

    proxyModel = new ProxyModel();
    proxyModel->setDynamicSortFilter(true);
    proxyModel->filterBetweenColumns(Name, ContactPhone);
    m_model = 0;

    QTimer::singleShot(0, this, SLOT(init()));
}

SupplierManageWindow::~SupplierManageWindow()
{
    if(m_model) {
	delete m_model;
	m_model = 0;
    }

    if(proxyModel) {
	delete proxyModel;
	proxyModel = 0;
    }
}

void SupplierManageWindow::closeWindow()
{
    emit closeWindow(this);
}

void SupplierManageWindow::init()
{
    m_model = new QSqlRelationalTableModel(0, core->database()->database());
    bool r = core->databaseQuery()->suppliers(m_model);
    if(!r) {
	errorHandling(DATABASE_ERROR, core->databaseQuery()->lastErrorNumber());
	return;
    }

    //
    int areaSize = m_model->relationModel(idArea)->rowCount();
    if(!areaSize) {
	QMessageBox::critical(0,
	  trUtf8("Datos incompletos"),
	  trUtf8("Debe ingresar al menos un Rubro en:\n"
		 "Panel General -> Rubros"));
	tb_add->setEnabled(false);
    }
    areaUpdateCombo(false);

    //
    proxyModel->setSourceModel(m_model);
    tv_supplier->setModel(proxyModel);
    tv_supplier->verticalHeader()->hide();
    tv_supplier->horizontalHeader()->setResizeMode(Email, QHeaderView::Stretch);

    m_model->setHeaderData(Name, Qt::Horizontal, trUtf8("Nombre"));
    m_model->setHeaderData(idArea, Qt::Horizontal, trUtf8("Rubro"));
    m_model->setHeaderData(Address, Qt::Horizontal, trUtf8("Dirección"));
    m_model->setHeaderData(Location, Qt::Horizontal, trUtf8("Localidad"));
    m_model->setHeaderData(Phone, Qt::Horizontal, trUtf8("Teléfono"));
    m_model->setHeaderData(Fax, Qt::Horizontal, trUtf8("Fax"));
    m_model->setHeaderData(Email, Qt::Horizontal, trUtf8("Mail"));
    m_model->setHeaderData(ContactName, Qt::Horizontal, trUtf8("Nom. Contacto"));
    m_model->setHeaderData(ContactPhone, Qt::Horizontal, trUtf8("Tel. Contacto"));

    tv_supplier->setColumnHidden(id, true);

    if(m_model->rowCount() == 0) {
	tb_edit->setEnabled(false);
	tb_remove->setEnabled(false);
    }

    connect(tb_add, SIGNAL(clicked()),
	    this, SLOT(addSupplier()));
    connect(tb_remove, SIGNAL(clicked()),
	    this, SLOT(removeSupplier()));
    connect(tb_edit, SIGNAL(clicked()),
	    this, SLOT(editSupplier()));
    connect(tv_supplier, SIGNAL(doubleClicked(const QModelIndex &)),
	    this, SLOT(editSupplier()));
    connect(tb_close, SIGNAL(clicked()),
	    this, SLOT(closeWindow()));
    connect(le_filter, SIGNAL(textChanged(const QString &)),
	    this, SLOT(filterChanged(const QString &)));
    connect(le_filter, SIGNAL(returnPressed()),
	    this, SLOT(filterReturnPressed()));
    connect(tb_areaUpdateCombo, SIGNAL(clicked()),
	    this, SLOT(areaUpdate()));
    connect(cb_area, SIGNAL(currentIndexChanged(int)),
	    this, SLOT(areaComboChanged(int)));


    le_filter->setFocus();
}

void SupplierManageWindow::updateData()
{
    m_model->select();
}

void SupplierManageWindow::filterChanged(const QString &text)
{
    QRegExp regExp(text, Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterRegExp(regExp);
}

void SupplierManageWindow::addSupplier()
{
    SupplierDlg *supplierDlg = new SupplierDlg(this, trUtf8("Alta Proveedor"), -1, m_model);
    connect(supplierDlg, SIGNAL(areaUpdate()),
	    this, SLOT(areaUpdate()));
    int n = supplierDlg->exec();
    delete supplierDlg;

    if(n == QDialog::Rejected)
	return;

    if(!tb_edit->isEnabled()) {
	tb_edit->setEnabled(true);
	tb_remove->setEnabled(true);
    }

    le_filter->setFocus();
}

void SupplierManageWindow::removeSupplier()
{
    int currentRow = tv_supplier->selectionModel()->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::critical(this,
	  trUtf8("Error"),
	  trUtf8("Debe seleccionar el Proveedor que desea dar de baja."));
	return;
    }

    currentRow = proxyModel->mapToSource(proxyModel->index(currentRow, 0)).row();
    QString name = m_model->data(m_model->index(currentRow, Name)).toString();

    QString msg;
    msg = trUtf8("¿Está seguro dar de baja al Proveedor: ");
    msg += name;
    msg += trUtf8("?");

    int r = QMessageBox::warning(this,
      trUtf8("Confirme la baja"),
      msg,
      QMessageBox::Ok | QMessageBox::Cancel);
    if(r != QMessageBox::Ok)
	return;

    if(!m_model->removeRow(currentRow)) {
	errorHandling(DATABASE_ERROR, m_model->lastError().number());
	return;
    }

    if(m_model->rowCount() == 0) {
	tb_edit->setEnabled(false);
	tb_remove->setEnabled(false);
    }

    le_filter->setFocus();
}

void SupplierManageWindow::editSupplier()
{
    int currentRow = tv_supplier->selectionModel()->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::critical(this,
	  trUtf8("Error"),
	  trUtf8("Debe seleccionar el Proveedor que desea Modificar."));
	return;
    }

    currentRow = proxyModel->mapToSource(proxyModel->index(currentRow, 0)).row();
    QString name = m_model->data(m_model->index(currentRow, Name)).toString();

    QString title = trUtf8("Modificación Proveedor: ");
    title += name;
    SupplierDlg *supplierDlg = new SupplierDlg(this, title, currentRow, m_model);
    connect(supplierDlg, SIGNAL(areaUpdate()),
	    this, SLOT(areaUpdate()));
    int n = supplierDlg->exec();
    delete supplierDlg;

    le_filter->setFocus();

    if(n == QDialog::Rejected) {
	if(!le_filter->text().isEmpty())
	    le_filter->selectAll();
    }
}

void SupplierManageWindow::filterReturnPressed()
{
    if(proxyModel->rowCount() > 0) {
	tv_supplier->selectRow(0);
	tv_supplier->setFocus();
	if(proxyModel->rowCount() == 1) {
	    editSupplier();
	}
    }
}

void SupplierManageWindow::areaUpdate()
{
    areaUpdateCombo(true);
}

void SupplierManageWindow::areaUpdateCombo(bool select)
{
    QSqlTableModel *ptrModel = m_model->relationModel(SupplierManageWindow::idArea);
    if(select)
	ptrModel->select();
    ptrModel->insertRow(0);
    ptrModel->setData(ptrModel->index(0, ptrModel->fieldIndex("name")),
      trUtf8("-- Seleccione Rubro --"), Qt::EditRole);
    cb_area->setModel(ptrModel);
    cb_area->setModelColumn(ptrModel->fieldIndex("name"));
    cb_area->setCurrentIndex(0);
}

void SupplierManageWindow::areaComboChanged(int index)
{
    if(index == 0) {
	proxyModel->removeFilterColumn(idArea);
    } else {
	proxyModel->setFilterColumn(idArea, cb_area->currentText());
    }
    proxyModel->invalidateFilter();
}

QString SupplierManageWindow::toHtml(bool *ok)
{
    if(!proxyModel->rowCount()) {
	int a = QMessageBox::question(this,
		  trUtf8("Datos vacíos"),
		  trUtf8("La planilla de Proveedores no contiene datos.\n"
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
    html += "<h2><strong>" + trUtf8("Listado de Proveedores") + "</strong></h2>";
    html += "<table cellpadding=\"2\" cellspacing=\"0\">";
    html += "<tr>";
    html += "<th>"+trUtf8("Nombre")+"</th>";
    html += "<th>"+trUtf8("Rubro")+"</th>";
    html += "<th>"+trUtf8("Dirección")+"</th>";
    html += "<th>"+trUtf8("Localidad")+"</th>";
    html += "<th>"+trUtf8("Teléfono")+"</th>";
    html += "<th>"+trUtf8("Fax")+"</th>";
    html += "<th>"+trUtf8("Nom. Contacto")+"</th>";
    html += "<th>"+trUtf8("Tel. Contacto")+"</th>";
    html += "</tr>";

    for(int i = 0; i < proxyModel->rowCount(); i++) {
	html += "<tr>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, Name)).toString()+"</td>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, idArea)).toString()+"</td>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, Address)).toString()+"</td>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, Location)).toString()+"</td>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, Phone)).toString()+"</td>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, Fax)).toString()+"</td>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, ContactName)).toString()+"</td>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, ContactPhone)).toString()+"</td>";
	html += "</tr>";
    }
    html += "</table></body></html>";
    *ok = true;
    return html;
}

void SupplierManageWindow::errorHandling(int error_type, int error)
{
    switch(error_type) {
	case DATABASE_ERROR:
	    if(!errorCommon(this, SUPPLIER_WINDOW, error)) {
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
