/*
 *
 *
 */


#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QMdiArea>
#include <QHash>

#include "ui_mainwin.h"
#include "core.h"

class QAction;
class QString;
class QLabel;

class MainWin : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWin(QWidget *parent = 0, Core *core = 0);
    ~MainWin();

    QWidget *pWindowFind(int windowType);

private slots:
    void about();
    void removeWindow(QWidget *w);
    void print();
    void configurationManagement();
    void customerManagement();
    void supplierManagement();
    void employedManagement();
    void buildingManagement();
    void receiptManagement();

private:
    QMdiSubWindow *windowFind(int windowType);
    void bringToFront(QMdiSubWindow *window);
    void addWindow(QWidget *window, int windowType);
    void closeAllSubWindows();

    QHash <int, QMdiSubWindow *> windowList;
    int activeWindow;

    Core *core;
};

#endif // MAINWIN_H
