/*
 *
 *
 *
 */

#include "buildingsearchdlg.h"

#include <QMessageBox>
#include <QHeaderView>

#include "buildingmanagewindow.h"

BuildingSearchDlg::BuildingSearchDlg(QWidget *parent, QSqlTableModel *model, int *row)
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

BuildingSearchDlg::~BuildingSearchDlg()
{
}

void BuildingSearchDlg::init()
{
    proxyModel->setHeaderData(BuildingManageWindow::Name, Qt::Horizontal, trUtf8("Nombre"));
    proxyModel->setHeaderData(BuildingManageWindow::Address, Qt::Horizontal, trUtf8("Dirección"));
    proxyModel->setHeaderData(BuildingManageWindow::Location, Qt::Horizontal, trUtf8("Localidad"));
    proxyModel->setHeaderData(BuildingManageWindow::Type, Qt::Horizontal, trUtf8("Teléfono"));

    tv_building->setModel(proxyModel);
    tv_building->setColumnHidden(BuildingManageWindow::id, true);
    tv_building->setColumnHidden(BuildingManageWindow::idCustomer, true);
    tv_building->setRowHidden(0, true);

    tv_building->verticalHeader()->hide();
    tv_building->horizontalHeader()->setResizeMode(BuildingManageWindow::Name, QHeaderView::Stretch);

    connect(pb_add, SIGNAL(clicked()),
	    this, SLOT(addCurrentBuilding()));
    connect(pb_close, SIGNAL(clicked()),
	    this, SLOT(reject()));
    connect(tv_building, SIGNAL(doubleClicked(const QModelIndex &)),
	    this, SLOT(buildingAccepted(const QModelIndex &)));
    connect(le_filter, SIGNAL(textChanged(const QString &)),
            this, SLOT(filterChanged(const QString &)));
    connect(le_filter, SIGNAL(returnPressed()),
            this, SLOT(filterReturnPressed()));
}

void BuildingSearchDlg::filterChanged(const QString &text)
{
    QRegExp regExp(text, Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterRegExp(regExp);
}

void BuildingSearchDlg::filterReturnPressed()
{
    if(proxyModel->rowCount() > 0) {
        tv_building->selectRow(0);
        tv_building->setFocus();

        if(proxyModel->rowCount() == 1)
	    addCurrentBuilding();
    }
}

void BuildingSearchDlg::addCurrentBuilding()
{
    QModelIndex index = tv_building->selectionModel()->currentIndex();
    if(index.row() < 0) {
	QMessageBox::information(this,
	  trUtf8("Seleccione Obra"),
	  trUtf8("Primero seleccione la obra."));
	return;
    }

    buildingAccepted(index);
}

void BuildingSearchDlg::buildingAccepted(const QModelIndex &index)
{
    if(!index.isValid())
	return;

    *row = proxyModel->mapToSource(index).row();
    QDialog::accept();
}
