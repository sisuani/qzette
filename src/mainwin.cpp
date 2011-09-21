/*
 *
 *
 *
 */


#include "mainwin.h"

#include <QDir>
#include <QMdiSubWindow>
#include <QListWidgetItem>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintDialog>

#include "applicationinfo.h"
#include "configurationdlg.h"
#include "customermanagewindow.h"
#include "suppliermanagewindow.h"
#include "employedmanagewindow.h"
#include "buildingmanagewindow.h"
#include "receiptmanagewindow.h"

#define WindowFlag Qt::Window | Qt::CustomizeWindowHint

MainWin::MainWin(QWidget *parent, Core *core)
    : QMainWindow(parent),
      core(core)
{
    setupUi(this);
    menuToolBar->addAction(toolBarTools->toggleViewAction());

    if(!option.toolBarToolsVisible) {
	toolBarTools->setVisible(false);
    }

    connect(action_Quit, SIGNAL(triggered()),
	    this, SLOT(close()));
    connect(action_Print, SIGNAL(triggered()),
	    this, SLOT(print()));
    connect(action_GeneralPanel, SIGNAL(triggered()),
	    this, SLOT(configurationManagement()));
    connect(action_About, SIGNAL(triggered()),
	    this, SLOT(about()));
    connect(action_CustomerManage, SIGNAL(triggered()),
	    this, SLOT(customerManagement()));
    connect(action_SupplierManage, SIGNAL(triggered()),
	    this, SLOT(supplierManagement()));
    connect(action_EmployedManage, SIGNAL(triggered()),
	    this, SLOT(employedManagement()));
    connect(action_BuildingManage, SIGNAL(triggered()),
	    this, SLOT(buildingManagement()));
    connect(action_ReceiptManage, SIGNAL(triggered()),
	    this, SLOT(receiptManagement()));
}

MainWin::~MainWin()
{
    closeAllSubWindows();
}

void MainWin::closeAllSubWindows()
{
    windowList.clear();
    mdiArea->closeAllSubWindows();
}

void MainWin::print()
{
    QMdiSubWindow *subWindow = mdiArea->activeSubWindow();
    if(!subWindow)
	return;

    QWidget *widget = subWindow->widget();
    QHash<int, QMdiSubWindow *>::const_iterator i = windowList.constBegin();
    QString html;
    bool ok = false;
    while(i != windowList.constEnd()) {
	if(i.value()->widget()->winId() == widget->winId()) {
	    switch(i.key()) {
		case CUSTOMER_WINDOW:
		  html = static_cast<CustomerManageWindow *>(widget)->toHtml(&ok);
		  break;
		case SUPPLIER_WINDOW:
		  html = static_cast<SupplierManageWindow *>(widget)->toHtml(&ok);
		  break;
		case EMPLOYED_WINDOW:
		  html = static_cast<EmployedManageWindow *>(widget)->toHtml(&ok);
		  break;
		case BUILDING_WINDOW:
		  html = static_cast<BuildingManageWindow *>(widget)->toHtml(&ok);
		  break;
		case RECEIPT_WINDOW:
		  html = static_cast<ReceiptManageWindow *>(widget)->toHtml(&ok);
		  break;
	    }
	}
	i++;
    }

    if(!ok)
	return;

    QTextDocument document;
    document.setHtml(html);
    QPrinter printer;
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    if(dialog->exec() != QDialog::Accepted)
	return;
    document.print(&printer);
}

void MainWin::configurationManagement()
{
    ConfigurationDialog *configurationDlg = new ConfigurationDialog(this, core);
    configurationDlg->show();
}

void MainWin::about()
{
    QMessageBox::about(this,
      trUtf8("About Zette Sistema"),
      trUtf8("<h2>Zette Sistema 1.0</h2>"
	     "<p> Contacto: Samuel Isuani "
	     "<p> Mail: sisuani@gmail.com - Teléfono: 261 156683343"));
}

void MainWin::customerManagement()
{
    QMdiSubWindow *subWindow =
	  (QMdiSubWindow *) windowFind(CUSTOMER_WINDOW);

    if(!subWindow) {
	CustomerManageWindow *w = new CustomerManageWindow(0, core);
	addWindow(w, CUSTOMER_WINDOW);
    } else {
	bringToFront(subWindow);
    }
}

void MainWin::supplierManagement()
{
    QMdiSubWindow *subWindow =
	  (QMdiSubWindow *) windowFind(SUPPLIER_WINDOW);

    if(!subWindow) {
	SupplierManageWindow *w = new SupplierManageWindow(0, core);
	addWindow(w, SUPPLIER_WINDOW);
    } else {
	bringToFront(subWindow);
	SupplierManageWindow *w = static_cast<SupplierManageWindow *>(subWindow->widget());
	w->updateData();
    }
}

void MainWin::employedManagement()
{
    QMdiSubWindow *subWindow =
	  (QMdiSubWindow *) windowFind(EMPLOYED_WINDOW);

    if(!subWindow) {
	EmployedManageWindow *w = new EmployedManageWindow(0, core);
	addWindow(w, EMPLOYED_WINDOW);
    } else {
	bringToFront(subWindow);
	EmployedManageWindow *w = static_cast<EmployedManageWindow *>(subWindow->widget());
	w->updateData();
    }
}

void MainWin::buildingManagement()
{
    QMdiSubWindow *subWindow =
	  (QMdiSubWindow *) windowFind(BUILDING_WINDOW);

    if(!subWindow) {
	BuildingManageWindow *w = new BuildingManageWindow(0, core);
	addWindow(w, BUILDING_WINDOW);
    } else {
	bringToFront(subWindow);
	BuildingManageWindow *w = static_cast<BuildingManageWindow *>(subWindow->widget());
	w->updateData();
    }
}

void MainWin::receiptManagement()
{
    QMdiSubWindow *subWindow =
	  (QMdiSubWindow *) windowFind(RECEIPT_WINDOW);

    if(!subWindow) {
	ReceiptManageWindow *w = new ReceiptManageWindow(0, core);
	addWindow(w, RECEIPT_WINDOW);
    } else {
	bringToFront(subWindow);
	ReceiptManageWindow *w = static_cast<ReceiptManageWindow *>(subWindow->widget());
	w->updateData();
    }
}

QMdiSubWindow *MainWin::windowFind(int windowType)
{
    QHash<int, QMdiSubWindow *>::const_iterator i = windowList.constBegin();

    while(i != windowList.constEnd()) {
	if(i.key() == windowType) {
	    return i.value();
	}
	i++;
    }

    return 0;
}

QWidget *MainWin::pWindowFind(int windowType)
{
    QHash<int, QMdiSubWindow *>::const_iterator i = windowList.constBegin();

    while(i != windowList.constEnd()) {
	if(i.key() == windowType) {
	    return i.value();
	}
	i++;
    }

    return 0;
}

void MainWin::removeWindow(QWidget *w)
{
    QHash<int, QMdiSubWindow *>::const_iterator i = windowList.constBegin();

    while(i != windowList.constEnd()) {
	if(i.value()->widget()->winId() == w->winId()) {
	    mdiArea->closeActiveSubWindow();
	    windowList.remove(i.key());
	    return;
	}
	i++;
    }
}

void MainWin::addWindow(QWidget *w, int windowType)
{
    connect(w, SIGNAL(closeWindow(QWidget *)),
	    this, SLOT(removeWindow(QWidget *)));

    QMdiSubWindow *subWindow = new QMdiSubWindow();
    windowList.insert(windowType, subWindow);
    subWindow->setWidget(w);
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    mdiArea->addSubWindow(subWindow, WindowFlag);
    w->showMaximized();
    w->setFocus();
}

void MainWin::bringToFront(QMdiSubWindow *w)
{
    if(!w) {
	return;
    }

    mdiArea->setActiveSubWindow(w);
}
