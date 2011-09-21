/*
 *
 *
 *
 */

#include "employedmanagewindow.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>
#include <QValidator>
#include <QSqlRelationalDelegate>
#include <QDebug>
#include <QSqlError>

#include "employeddlg.h"

EmployedManageWindow::EmployedManageWindow(QWidget *parent, Core *core)
    : QWidget(parent),
      core(core)
{
    setupUi(this);

    proxyModel = new ProxyModel();
    proxyModel->setDynamicSortFilter(true);
    proxyModel->filterBetweenColumns(EmployedModel::LastName, EmployedModel::Rating);
    m_model = 0;

    QTimer::singleShot(0, this, SLOT(init()));
}

EmployedManageWindow::~EmployedManageWindow()
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

void EmployedManageWindow::closeWindow()
{
    emit closeWindow(this);
}

void EmployedManageWindow::init()
{
    m_model = new EmployedModel(0, core->database()->database());
    bool r = core->databaseQuery()->employees(m_model);
    if(!r) {
	errorHandling(DATABASE_ERROR, core->databaseQuery()->lastErrorNumber());
	return;
    }

    //
    int categorySize = m_model->relationModel(EmployedModel::idCategory)->rowCount();
    if(!categorySize) {
	QMessageBox::critical(0,
	  trUtf8("Datos incompletos"),
	  trUtf8("Debe ingresar al menos una Categoría en:\n"
		 "Panel General -> Categorías"));
	tb_add->setEnabled(false);
    }
    categoryUpdateCombo(false);

    //
    updateColumns();
    proxyModel->setSourceModel(m_model);
    tv_employed->setModel(proxyModel);
    tv_employed->verticalHeader()->hide();
    tv_employed->horizontalHeader()->setResizeMode(EmployedModel::LastName, QHeaderView::Stretch);

    m_model->setHeaderData(EmployedModel::LastName, Qt::Horizontal, trUtf8("Apellido"));
    m_model->setHeaderData(EmployedModel::Name, Qt::Horizontal, trUtf8("Nombre"));
    m_model->setHeaderData(EmployedModel::idCategory, Qt::Horizontal, trUtf8("Categoría"));
    m_model->setHeaderData(EmployedModel::Dni, Qt::Horizontal, trUtf8("DNI"));
    m_model->setHeaderData(EmployedModel::Address, Qt::Horizontal, trUtf8("Dirección"));
    m_model->setHeaderData(EmployedModel::Location, Qt::Horizontal, trUtf8("Localidad"));
    m_model->setHeaderData(EmployedModel::CellPhone, Qt::Horizontal, trUtf8("Móvil"));
    m_model->setHeaderData(EmployedModel::Phone, Qt::Horizontal, trUtf8("Teléfono"));
    m_model->setHeaderData(EmployedModel::RatingPix, Qt::Horizontal, trUtf8("Calificación"));

    tv_employed->setColumnHidden(EmployedModel::id, true);
    tv_employed->setColumnHidden(EmployedModel::Rating, true);

    if(m_model->rowCount() == 0) {
	tb_edit->setEnabled(false);
	tb_remove->setEnabled(false);
    }

    connect(tb_add, SIGNAL(clicked()),
	    this, SLOT(addEmployed()));
    connect(tb_remove, SIGNAL(clicked()),
	    this, SLOT(removeEmployed()));
    connect(tb_edit, SIGNAL(clicked()),
	    this, SLOT(editEmployed()));
    connect(tv_employed, SIGNAL(doubleClicked(const QModelIndex &)),
	    this, SLOT(editEmployed()));
    connect(tb_close, SIGNAL(clicked()),
	    this, SLOT(closeWindow()));
    connect(le_filter, SIGNAL(textChanged(const QString &)),
	    this, SLOT(filterChanged(const QString &)));
    connect(le_filter, SIGNAL(returnPressed()),
	    this, SLOT(filterReturnPressed()));
    connect(cb_rating, SIGNAL(currentIndexChanged(int)),
	    this, SLOT(ratingComboChanged(int)));
    connect(tb_categoryUpdateCombo, SIGNAL(clicked()),
	    this, SLOT(categoryUpdate()));
    connect(cb_category, SIGNAL(currentIndexChanged(int)),
	    this, SLOT(categoryComboChanged(int)));


    le_filter->setFocus();
}

void EmployedManageWindow::updateData()
{
    m_model->select();
    updateColumns();
}

void EmployedManageWindow::filterChanged(const QString &text)
{
    QRegExp regExp(text, Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterRegExp(regExp);
}

void EmployedManageWindow::addEmployed()
{
    EmployedDlg *employedDlg = new EmployedDlg(this, trUtf8("Alta Empleado"), -1, m_model);
    connect(employedDlg, SIGNAL(categoryUpdate()),
	    this, SLOT(categoryUpdate()));
    int n = employedDlg->exec();
    delete employedDlg;

    if(n == QDialog::Rejected)
	return;

    updateColumns();
    if(!tb_edit->isEnabled()) {
	tb_edit->setEnabled(true);
	tb_remove->setEnabled(true);
    }

    le_filter->setFocus();
}

void EmployedManageWindow::removeEmployed()
{
    int currentRow = tv_employed->selectionModel()->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::critical(this,
	  trUtf8("Error"),
	  trUtf8("Debe seleccionar el Empleado que desea dar de baja."));
	return;
    }

    currentRow = proxyModel->mapToSource(proxyModel->index(currentRow, 0)).row();
    QString lastName = m_model->data(m_model->index(currentRow,
      EmployedModel::LastName), Qt::DisplayRole).toString();
    QString name = m_model->data(m_model->index(currentRow,
      EmployedModel::Name), Qt::DisplayRole).toString();

    QString msg;
    msg = trUtf8("¿Está seguro dar de baja al Empleado: ");
    msg += lastName + ", " + name + trUtf8("?");

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
    updateColumns();

    if(m_model->rowCount() == 0) {
	tb_edit->setEnabled(false);
	tb_remove->setEnabled(false);
    }

    le_filter->setFocus();
}

void EmployedManageWindow::editEmployed()
{
    int currentRow = tv_employed->selectionModel()->currentIndex().row();
    if(currentRow < 0) {
	QMessageBox::critical(this,
	  trUtf8("Error"),
	  trUtf8("Debe seleccionar el Empleado que desea Modificar."));
	return;
    }

    currentRow = proxyModel->mapToSource(proxyModel->index(currentRow, 0)).row();
    QString lastName = m_model->data(m_model->index(currentRow,
      EmployedModel::LastName), Qt::DisplayRole).toString();
    QString name = m_model->data(m_model->index(currentRow,
      EmployedModel::Name), Qt::DisplayRole).toString();

    QString title = trUtf8("Modificación Empleado: ");
    title += lastName + ", " + name;
    EmployedDlg *employedDlg = new EmployedDlg(this, title, currentRow, m_model);
    connect(employedDlg, SIGNAL(categoryUpdate()),
	    this, SLOT(categoryUpdate()));
    int n = employedDlg->exec();
    delete employedDlg;

    le_filter->setFocus();
    if(n == QDialog::Rejected) {
	if(!le_filter->text().isEmpty())
	    le_filter->selectAll();
	return;
    }
    updateColumns();
}

void EmployedManageWindow::updateColumns()
{
    m_model->insertColumn(EmployedModel::RatingPix);
    proxyModel->setSourceModel(m_model);
}

void EmployedManageWindow::filterReturnPressed()
{
    if(proxyModel->rowCount() > 0) {
	tv_employed->selectRow(0);
	tv_employed->setFocus();
	if(proxyModel->rowCount() == 1) {
	    editEmployed();
	}
    }
}

void EmployedManageWindow::categoryComboChanged(int index)
{
    if(index == 0) {
	proxyModel->removeFilterColumn(EmployedModel::idCategory);
    } else {
	proxyModel->setFilterColumn(EmployedModel::idCategory, cb_category->currentText());
    }
    proxyModel->invalidateFilter();
}

void EmployedManageWindow::ratingComboChanged(int index)
{
    if(index == 0) {
	proxyModel->removeFilterColumn(EmployedModel::Rating);
    } else {
	proxyModel->setFilterColumn(EmployedModel::Rating, QString::number(cb_rating->currentIndex() - 1));
    }
    proxyModel->invalidateFilter();
}

void EmployedManageWindow::categoryUpdate()
{
    categoryUpdateCombo(true);
}

void EmployedManageWindow::categoryUpdateCombo(bool select)
{
    QSqlTableModel *ptrModel = m_model->relationModel(EmployedModel::idCategory);
    if(select)
	ptrModel->select();
    ptrModel->insertRow(0);
    ptrModel->setData(ptrModel->index(0, ptrModel->fieldIndex("name")),
      trUtf8("-- Seleccione Categoría --"), Qt::EditRole);
    cb_category->setModel(ptrModel);
    cb_category->setModelColumn(ptrModel->fieldIndex("name"));
    cb_category->setCurrentIndex(0);
}

QString EmployedManageWindow::toHtml(bool *ok)
{
    if(!proxyModel->rowCount()) {
	int a = QMessageBox::question(this,
		  trUtf8("Datos vacíos"),
		  trUtf8("La planilla de Empleados no contiene datos.\n"
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
    html += "<h2><strong>" + trUtf8("Listado de empleados") + "</strong></h2>";
    html += "<table cellpadding=\"2\" cellspacing=\"0\">";
    html += "<tr>";
    html += "<th>"+trUtf8("Apellido")+"</th>";
    html += "<th>"+trUtf8("Nombre")+"</th>";
    html += "<th>"+trUtf8("Categoría")+"</th>";
    html += "<th>"+trUtf8("DNI")+"</th>";
    html += "<th>"+trUtf8("Dirección")+"</th>";
    html += "<th>"+trUtf8("Localidad")+"</th>";
    html += "<th>"+trUtf8("Móvil")+"</th>";
    html += "<th>"+trUtf8("Teléfono")+"</th>";
    html += "<th>"+trUtf8("Calificación")+"</th>";
    html += "</tr>";

    for(int i = 0; i < proxyModel->rowCount(); i++) {
	html += "<tr>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, EmployedModel::LastName),
		  Qt::DisplayRole).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, EmployedModel::Name),
		  Qt::DisplayRole).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, EmployedModel::idCategory),
		  Qt::DisplayRole).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, EmployedModel::Dni),
		  Qt::DisplayRole).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, EmployedModel::Address),
		  Qt::DisplayRole).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, EmployedModel::Location),
		  Qt::DisplayRole).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, EmployedModel::CellPhone),
		  Qt::DisplayRole).toString()+"</td>";
	html += "<td>"+proxyModel->data(proxyModel->index(i, EmployedModel::Phone),
		  Qt::DisplayRole).toString()+"</td>";
	int rating = proxyModel->data(proxyModel->index(i, EmployedModel::Rating),
		  Qt::DisplayRole).toInt();
	QString ratingStr;
	switch(rating) {
	    case 0:
	      ratingStr = trUtf8("Excelente");
	      break;
	    case 1:
	      ratingStr = trUtf8("Muy bueno");
	      break;
	    case 2:
	      ratingStr = trUtf8("Bueno");
	      break;
	    case 3:
	      ratingStr = trUtf8("Regular");
	      break;
	    case 4:
	      ratingStr = trUtf8("Malo");
	      break;
	}
	html += "<td>"+ratingStr+"</td>";
	html += "</tr>";
    }
    html += "</table></body></html>";
    *ok = true;
    return html;
}

void EmployedManageWindow::errorHandling(int error_type, int error)
{
    switch(error_type) {
	case DATABASE_ERROR:
	    if(!errorCommon(this, EMPLOYED_WINDOW, error)) {
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
