/*
 *
 *
 *
 */

#include "customermanagewindow.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>
#include <QRegExp>
#include <QValidator>
#include <QShortcut>
#include <QSqlError>

#include "customerdlg.h"

CustomerManageWindow::CustomerManageWindow(QWidget *parent, Core *core)
    : QWidget(parent),
      core(core)
{
    setupUi(this);

    proxyModel = new QSortFilterProxyModel();
    proxyModel->setDynamicSortFilter(true);
    m_model = 0;

    connect(tb_add, SIGNAL(clicked()),
	    this, SLOT(addCustomer()));
    connect(tb_remove, SIGNAL(clicked()),
	    this, SLOT(removeCustomer()));
    connect(tb_edit, SIGNAL(clicked()),
	    this, SLOT(editCustomer()));
    connect(tv_customer, SIGNAL(doubleClicked(const QModelIndex &)),
	    this, SLOT(editCustomer()));
    connect(tb_close, SIGNAL(clicked()),
	    this, SLOT(closeWindow()));
    connect(le_filter, SIGNAL(textChanged(const QString &)),
	    this, SLOT(filterChanged(const QString &)));
    connect(le_filter, SIGNAL(returnPressed()),
	    this, SLOT(filterReturnPressed()));

    QTimer::singleShot(0, this, SLOT(init()));
}

CustomerManageWindow::~CustomerManageWindow()
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

void CustomerManageWindow::closeWindow()
{
    emit closeWindow(this);
}

void CustomerManageWindow::init()
{
    m_model = new QSqlTableModel(0, core->database()->database());
    bool r = core->databaseQuery()->customers(m_model);
    if(!r) {
	errorHandling(DATABASE_ERROR, core->databaseQuery()->lastErrorNumber());
	return;
    }

    proxyModel->setSourceModel(m_model);
    tv_customer->setModel(proxyModel);

    tv_customer->verticalHeader()->hide();
    tv_customer->horizontalHeader()->setResizeMode(Email, QHeaderView::Stretch);
    m_model->setHeaderData(LastName, Qt::Horizontal, trUtf8("Apellido"));
    m_model->setHeaderData(Name, Qt::Horizontal, trUtf8("Nombre"));
    m_model->setHeaderData(Address, Qt::Horizontal, trUtf8("Dirección"));
    m_model->setHeaderData(Location, Qt::Horizontal, trUtf8("Localidad"));
    m_model->setHeaderData(CellPhone, Qt::Horizontal, trUtf8("Móvil"));
    m_model->setHeaderData(Phone, Qt::Horizontal, trUtf8("Teléfono"));
    m_model->setHeaderData(Email, Qt::Horizontal, trUtf8("Mail"));
    tv_customer->setColumnHidden(CustomerManageWindow::id, true);

    if(m_model->rowCount() == 0) {
	tb_edit->setEnabled(false);
	tb_remove->setEnabled(false);
    }

    le_filter->setFocus();
}

void CustomerManageWindow::filterChanged(const QString &text)
{
    QRegExp regExp(text, Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterRegExp(regExp);
}

void CustomerManageWindow::addCustomer()
{
    CustomerDlg *customerDlg = new CustomerDlg(this, trUtf8("Alta Cliente"), -1, m_model);
    int n = customerDlg->exec();
    delete customerDlg;

    if(n == QDialog::Rejected)
	return;

    if(!tb_edit->isEnabled()) {
	tb_edit->setEnabled(true);
	tb_remove->setEnabled(true);
    }

    le_filter->setFocus();
}

void CustomerManageWindow::removeCustomer()
{
    int currentRow = tv_customer->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::critical(this,
	  trUtf8("Error"),
	  trUtf8("Debe seleccionar el Cliente que desea dar de baja."));
	return;
    }

    currentRow = proxyModel->mapToSource(proxyModel->index(currentRow, 0)).row();
    QString lastName = m_model->data(m_model->index(currentRow, LastName)).toString();
    QString name = m_model->data(m_model->index(currentRow, Name)).toString();

    QString msg;
    msg = trUtf8("¿Está seguro dar de baja al Cliente: ");
    msg += lastName;
    msg += trUtf8(", ");
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

void CustomerManageWindow::editCustomer()
{
   int currentRow = tv_customer->selectionModel()->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::critical(this,
	  trUtf8("Error"),
	  trUtf8("Debe seleccionar el Cliente que desea Modificar."));
	return;
    }

    currentRow = proxyModel->mapToSource(proxyModel->index(currentRow, 0)).row();
    QString lastName = m_model->data(m_model->index(currentRow, LastName)).toString();
    QString name = m_model->data(m_model->index(currentRow, Name)).toString();

    QString title = trUtf8("Modificación Cliente: ");
    title += lastName;
    title += ", ";
    title += name;

    CustomerDlg *customerDlg = new CustomerDlg(this, title,
      currentRow, m_model);
    int n = customerDlg->exec();
    delete customerDlg;

    le_filter->setFocus();

    if(n == QDialog::Rejected) {
	if(!le_filter->text().isEmpty())
	    le_filter->selectAll();
    }
}

void CustomerManageWindow::filterReturnPressed()
{
    if(proxyModel->rowCount() > 0) {
	tv_customer->selectRow(0);
	tv_customer->setFocus();
	if(proxyModel->rowCount() == 1) {
	    editCustomer();
	}
    }
}

QString CustomerManageWindow::toHtml(bool *ok)
{
    if(!proxyModel->rowCount()) {
	int a = QMessageBox::question(this,
		  trUtf8("Datos vacíos"),
		  trUtf8("La planilla de Clientes no contiene datos.\n"
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
    html += "<h2><strong>" + trUtf8("Listado de Clientes") + "</strong></h2>";
    html += "<table cellpadding=\"2\" cellspacing=\"0\">";
    html += "<tr>";
    html += "<th>"+trUtf8("Apellido")+"</th>";
    html += "<th>"+trUtf8("Nombre")+"</th>";
    html += "<th>"+trUtf8("Dirección")+"</th>";
    html += "<th>"+trUtf8("Localidad")+"</th>";
    html += "<th>"+trUtf8("Móvil")+"</th>";
    html += "<th>"+trUtf8("Teléfono")+"</th>";
    html += "<th>"+trUtf8("Mail")+"</th>";
    html += "</tr>";

    for(int i = 0; i < proxyModel->rowCount(); i++) {
	html += "<tr>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, LastName)).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, Name)).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, Address)).toString()+"</td>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, Location)).toString()+"</td>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, CellPhone)).toString()+"</td>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, Phone)).toString()+"</td>";
	html += "<td align=center>"+proxyModel->data(proxyModel->index(i, Email)).toString()+"</td>";
	html += "</tr>";
    }
    html += "</table></body></html>";
    *ok = true;
    return html;
}

void CustomerManageWindow::errorHandling(int error_type, int error)
{
    switch(error_type) {
	case DATABASE_ERROR:
	    if(!errorCommon(this, CUSTOMER_WINDOW, error)) {
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
