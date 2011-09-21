/*
 *
 *
 */

#include <QApplication>
#include <QPlastiqueStyle>
#include <QMessageBox>
#include <QDir>

#include "core.h"
#include "applicationinfo.h"
#include "log.h"

static bool verifyHome()
{
    QDir path(ApplicationInfo::homePath());
    if(!path.exists()) {
	if(!path.mkdir(path.absolutePath())) {
	    qCritical("mkdir %s", qPrintable(path.absolutePath()));
	    return false;
	}
    }

    QDir log(ApplicationInfo::logDirPath());
    if(!log.exists()) {
	if(!log.mkdir(log.absolutePath())) {
	    qCritical("mkdir %s", qPrintable(log.absolutePath()));
	    return false;
	}
    }

    return true;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if(!verifyHome()) {
	QMessageBox::critical(0,
	  QObject::trUtf8("Error"),
	  QObject::trUtf8("No se puedo crear el directorio del programa."));
	return -1;
    }

    if(!Log::init()) {
	fprintf(stderr, "error: init()\n");
	return -1;
    }

#if defined(QT_DEBUG)
    qDebug("Starting QZette...");
#endif


    Core *core = new Core();

    if(!core->init()) {
	Log::error("initializing");
	delete core;
	QCoreApplication::quit();
	return -1;
    }

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    int returnValue = app.exec();
    delete core;

    return returnValue;
}
