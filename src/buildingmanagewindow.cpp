/*
 *
 *
 *
 */

#include "buildingmanagewindow.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>
#include <QValidator>
#include <QDebug>
#include <QSqlError>

#include "buildingdlg.h"

BuildingManageWindow::BuildingManageWindow(QWidget *parent, Core *core)
    : QWidget(parent),
      core(core)
{
    setupUi(this);

    proxyModel = new ProxyModel();
    proxyModel->setDynamicSortFilter(true);
    proxyModel->filterBetweenColumns(Name, Type);
    m_model = 0;
    customerModel = 0;

    QTimer::singleShot(0, this, SLOT(init()));
}

BuildingManageWindow::~BuildingManageWindow()
{
    if(m_model) {
	delete m_model;
	m_model = 0;
    }

    if(proxyModel) {
	delete proxyModel;
	proxyModel = 0;
    }

    if(customerModel)
	delete customerModel;
}

void BuildingManageWindow::closeWindow()
{
    emit closeWindow(this);
}

void BuildingManageWindow::init()
{
    bool r;
    customerModel = new QSqlTableModel(0, core->database()->database());
    r = core->databaseQuery()->customers(customerModel);
    if(!r) {
	errorHandling(DATABASE_ERROR, core->databaseQuery()->lastErrorNumber());
	return;
    }

    //
    m_model = new QSqlTableModel(0, core->database()->database());
    r = core->databaseQuery()->buildings(m_model);
    if(!r) {
	errorHandling(DATABASE_ERROR, core->databaseQuery()->lastErrorNumber());
	return;
    }

    //
    proxyModel->setSourceModel(m_model);
    tv_building->setModel(proxyModel);
    tv_building->verticalHeader()->hide();
    tv_building->horizontalHeader()->setResizeMode(Name, QHeaderView::Stretch);

    m_model->setHeaderData(Name, Qt::Horizontal, trUtf8("Nombre"));
    //m_model->setHeaderData(idCustomer, Qt::Horizontal, trUtf8("Cliente"));
    m_model->setHeaderData(Address, Qt::Horizontal, trUtf8("Dirección"));
    m_model->setHeaderData(Location, Qt::Horizontal, trUtf8("Localidad"));
    m_model->setHeaderData(Type, Qt::Horizontal, trUtf8("Tipo"));

    tv_building->setColumnHidden(id, true);
    tv_building->setColumnHidden(idCustomer, true);

    if(m_model->rowCount() == 0) {
	tb_edit->setEnabled(false);
	tb_remove->setEnabled(false);
    }

    connect(tb_add, SIGNAL(clicked()),
	    this, SLOT(addBuilding()));
    connect(tb_remove, SIGNAL(clicked()),
	    this, SLOT(removeBuilding()));
    connect(tb_edit, SIGNAL(clicked()),
	    this, SLOT(editBuilding()));
    connect(tv_building, SIGNAL(doubleClicked(const QModelIndex &)),
	    this, SLOT(editBuilding()));
    connect(tb_close, SIGNAL(clicked()),
	    this, SLOT(closeWindow()));
    connect(le_filter, SIGNAL(textChanged(const QString &)),
	    this, SLOT(filterChanged(const QString &)));
    connect(le_filter, SIGNAL(returnPressed()),
	    this, SLOT(filterReturnPressed()));

    le_filter->setFocus();
}

void BuildingManageWindow::updateData()
{
    m_model->query().exec();
}

void BuildingManageWindow::filterChanged(const QString &text)
{
    QRegExp regExp(text, Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterRegExp(regExp);
}

void BuildingManageWindow::addBuilding()
{
    BuildingDlg *buildingDlg = new BuildingDlg(this,
      trUtf8("Alta Obra"), -1, m_model, customerModel);
    int n = buildingDlg->exec();
    delete buildingDlg;

    if(n == QDialog::Rejected)
	return;

    if(!tb_edit->isEnabled()) {
	tb_edit->setEnabled(true);
	tb_remove->setEnabled(true);
    }

    le_filter->setFocus();
}

void BuildingManageWindow::removeBuilding()
{
    int currentRow = tv_building->selectionModel()->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::critical(this,
	  trUtf8("Error"),
	  trUtf8("Debe seleccionar la Obra que desea dar de baja."));
	return;
    }

    currentRow = proxyModel->mapToSource(proxyModel->index(currentRow, 0)).row();
    QString name = m_model->data(m_model->index(currentRow, Name)).toString();

    QString msg;
    msg = trUtf8("¿Está seguro dar de baja la Obra: ");
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

void BuildingManageWindow::editBuilding()
{
    int currentRow = tv_building->selectionModel()->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::critical(this,
	  trUtf8("Error"),
	  trUtf8("Debe seleccionar la Obra que desea Modificar."));
	return;
    }

    currentRow = proxyModel->mapToSource(proxyModel->index(currentRow, 0)).row();
    QString name = m_model->data(m_model->index(currentRow, Name)).toString();

    QString title = trUtf8("Modificación Obra: ");
    title += name;
    BuildingDlg *buildingDlg = new BuildingDlg(this,
      title, currentRow, m_model, customerModel);
    int n = buildingDlg->exec();
    delete buildingDlg;

    le_filter->setFocus();

    if(n == QDialog::Rejected) {
	if(!le_filter->text().isEmpty())
	    le_filter->selectAll();
    }
}

void BuildingManageWindow::filterReturnPressed()
{
    if(proxyModel->rowCount() > 0) {
	tv_building->selectRow(0);
	tv_building->setFocus();
	if(proxyModel->rowCount() == 1) {
	    editBuilding();
	}
    }
}

QString BuildingManageWindow::toHtml(bool *ok)
{
    if(!proxyModel->rowCount()) {
	int a = QMessageBox::question(this,
		  trUtf8("Datos vacíos"),
		  trUtf8("La planilla de Obras no contiene datos.\n"
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
    html += "<h2><strong>" + trUtf8("Listado de Obras") + "</strong></h2>";
    html += "<table cellpadding=\"2\" cellspacing=\"0\">";
    html += "<tr>";
    html += "<th>"+trUtf8("Nombre")+"</th>";
    html += "<th>"+trUtf8("Dirección")+"</th>";
    html += "<th>"+trUtf8("Localidad")+"</th>";
    html += "<th>"+trUtf8("Tipo")+"</th>";
    html += "</tr>";

    for(int i = 0; i < proxyModel->rowCount(); i++) {
	html += "<tr>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, Name)).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, Address)).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, Location)).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, Type)).toString()+"</td>";
	html += "</tr>";
    }
    html += "</table></body></html>";
    *ok = true;
    return html;

    return "";
}

void BuildingManageWindow::errorHandling(int error_type, int error)
{
    switch(error_type) {
	case DATABASE_ERROR:
	    if(!errorCommon(this, BUILDING_WINDOW, error)) {
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
