/*
 *
 *
 *
 */

#include "customersearchdlg.h"

#include <QMessageBox>
#include <QHeaderView>

#include "customermanagewindow.h"
#include "customerdlg.h"

CustomerSearchDlg::CustomerSearchDlg(QWidget *parent, QSqlTableModel *model, int *row)
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

CustomerSearchDlg::~CustomerSearchDlg()
{
}

void CustomerSearchDlg::init()
{
    proxyModel->setHeaderData(CustomerManageWindow::LastName, Qt::Horizontal, trUtf8("Apellido"));
    proxyModel->setHeaderData(CustomerManageWindow::Name, Qt::Horizontal, trUtf8("Nombre"));
    proxyModel->setHeaderData(CustomerManageWindow::Address, Qt::Horizontal, trUtf8("Dirección"));
    proxyModel->setHeaderData(CustomerManageWindow::Location, Qt::Horizontal, trUtf8("Localidad"));
    proxyModel->setHeaderData(CustomerManageWindow::Phone, Qt::Horizontal, trUtf8("Teléfono"));
    proxyModel->setHeaderData(CustomerManageWindow::Email, Qt::Horizontal, trUtf8("Mail"));

    tv_customer->setModel(proxyModel);
    tv_customer->setColumnHidden(CustomerManageWindow::id, true);

    tv_customer->verticalHeader()->hide();
    tv_customer->horizontalHeader()->setResizeMode(CustomerManageWindow::Email, QHeaderView::Stretch);

    connect(pb_add, SIGNAL(clicked()),
	    this, SLOT(addCurrentCustomer()));
    connect(pb_close, SIGNAL(clicked()),
	    this, SLOT(reject()));
    connect(tv_customer, SIGNAL(doubleClicked(const QModelIndex &)),
	    this, SLOT(customerAccepted(const QModelIndex &)));
    connect(le_filter, SIGNAL(textChanged(const QString &)),
            this, SLOT(filterChanged(const QString &)));
    connect(le_filter, SIGNAL(returnPressed()),
            this, SLOT(filterReturnPressed()));
    connect(pb_update, SIGNAL(clicked()),
	    this, SLOT(updateData()));
    connect(pb_addNewCustomer, SIGNAL(clicked()),
	    this, SLOT(addNewCustomer()));
}

void CustomerSearchDlg::filterChanged(const QString &text)
{
    QRegExp regExp(text, Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterRegExp(regExp);
}

void CustomerSearchDlg::filterReturnPressed()
{
    if(proxyModel->rowCount() > 0) {
        tv_customer->selectRow(0);
        tv_customer->setFocus();

        if(proxyModel->rowCount() == 1)
	    addCurrentCustomer();
    }
}

void CustomerSearchDlg::addCurrentCustomer()
{
    QModelIndex index = tv_customer->selectionModel()->currentIndex();
    if(index.row() < 0) {
	QMessageBox::information(this,
	  trUtf8("Seleccione Cliente"),
	  trUtf8("Seleccione el cliente que desea relacionar a la obra."));
	return;
    }

    customerAccepted(index);
}

void CustomerSearchDlg::customerAccepted(const QModelIndex &index)
{
    if(!index.isValid())
	return;

    *row = proxyModel->mapToSource(index).row();
    QDialog::accept();
}

void CustomerSearchDlg::updateData()
{
    model->select();
}

void CustomerSearchDlg::addNewCustomer()
{
    CustomerDlg *customerDlg = new CustomerDlg(this, trUtf8("Alta Cliente"), -1, model);
    customerDlg->exec();
}
