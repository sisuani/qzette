/*
 *
 *
 *
 */

#include "employedsearchdlg.h"

#include <QMessageBox>
#include <QHeaderView>

#include "employedmodel.h"

EmployedSearchDlg::EmployedSearchDlg(QWidget *parent, QSqlTableModel *model, int *row)
    : QDialog(parent)
    , row(row)
    , model(model)
{
    setupUi(this);

    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose);

    proxyModel = new QSortFilterProxyModel();
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(model);

    init();
}

EmployedSearchDlg::~EmployedSearchDlg()
{
}

void EmployedSearchDlg::init()
{
    proxyModel->setHeaderData(EmployedModel::LastName, Qt::Horizontal, trUtf8("Apellido"));
    proxyModel->setHeaderData(EmployedModel::Name, Qt::Horizontal, trUtf8("Nombre"));
    proxyModel->setHeaderData(EmployedModel::Dni, Qt::Horizontal, trUtf8("DNI"));
    proxyModel->setHeaderData(EmployedModel::Address, Qt::Horizontal, trUtf8("Dirección"));
    proxyModel->setHeaderData(EmployedModel::Location, Qt::Horizontal, trUtf8("Localidad"));
    proxyModel->setHeaderData(EmployedModel::Phone, Qt::Horizontal, trUtf8("Teléfono"));

    tv_employed->setModel(proxyModel);
    tv_employed->setColumnHidden(EmployedModel::id, true);
    tv_employed->setColumnHidden(EmployedModel::idCategory, true);
    tv_employed->setColumnHidden(EmployedModel::Rating, true);
    tv_employed->setRowHidden(0, true);

    tv_employed->verticalHeader()->hide();
    tv_employed->horizontalHeader()->setResizeMode(EmployedModel::Phone, QHeaderView::Stretch);

    connect(pb_add, SIGNAL(clicked()),
	    this, SLOT(addCurrentEmployed()));
    connect(pb_close, SIGNAL(clicked()),
	    this, SLOT(reject()));
    connect(tv_employed, SIGNAL(doubleClicked(const QModelIndex &)),
	    this, SLOT(employedAccepted(const QModelIndex &)));
    connect(le_filter, SIGNAL(textChanged(const QString &)),
            this, SLOT(filterChanged(const QString &)));
    connect(le_filter, SIGNAL(returnPressed()),
            this, SLOT(filterReturnPressed()));
}

void EmployedSearchDlg::filterChanged(const QString &text)
{
    QRegExp regExp(text, Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterRegExp(regExp);
}

void EmployedSearchDlg::filterReturnPressed()
{
    if(proxyModel->rowCount() > 0) {
        tv_employed->selectRow(0);
        tv_employed->setFocus();

        if(proxyModel->rowCount() == 1)
	    addCurrentEmployed();
    }
}

void EmployedSearchDlg::addCurrentEmployed()
{
    QModelIndex index = tv_employed->selectionModel()->currentIndex();
    if(index.row() < 0) {
	QMessageBox::information(this,
	  trUtf8("Seleccione Empleado"),
	  trUtf8("Primero seleccione el empleado."));
	return;
    }

    employedAccepted(index);
}

void EmployedSearchDlg::employedAccepted(const QModelIndex &index)
{
    if(!index.isValid())
	return;

    *row = proxyModel->mapToSource(index).row();
    QDialog::accept();
}
