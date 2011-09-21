/*
 *
 *
 */

#include "core.h"

#include <QString>
#include <QApplication>
#include <QMessageBox>

#include "settings.h"
#include "mainwin.h"
#include "log.h"

class Core::Private
{
public:
      Private(Core *parent)
      {
	  core = parent;
      }

      ~Private()
      {
      }

      Core *core;
      DatabaseQuery *databaseQuery;
      Database *database;
      MainWin *mainwin;
};

Core::Core()
  : QObject(0)
{
    d = new Private(this);
}

Core::~Core()
{
    if(d->mainwin) {
	delete d->mainwin;
    }

    if(d->databaseQuery) {
	delete d->databaseQuery;
    }

    if(d->database) {
	delete d->database;
    }

    delete d;
}

bool Core::init()
{
    d->database = 0;
    d->databaseQuery = 0;
    d->mainwin = 0;

    if(!Settings::instance()->isValid()) {
	Log::error("config is not valid.");
	QString msg = trUtf8("El archivo de configuración es inválido.");
	QMessageBox::critical(0,
	  trUtf8("Error"),
	  msg);
	return false;
    }

    /* set log level */
    Log::setLevel(Settings::instance()->getLogLevel());

    /* Options */
    option.minimumPasswordLength = Settings::instance()->getMinimumPasswordLength();

    if(option.minimumPasswordLength < 3 || option.minimumPasswordLength > 30) {
	Log::warning("option minimumPasswordLength is not valid.");
	option.minimumPasswordLength = 4;
    }

    option.toolBarToolsVisible = Settings::instance()->toolBarToolsVisible();
    option.toolBarActionsVisible = Settings::instance()->toolBarActionsVisible();

    //
    if(!initializeDatabase()) {
	Log::error("connecting to db...");
	QString msg = trUtf8("Compruebe la configuración de la base de datos");
	QMessageBox::critical(d->mainwin,
			      trUtf8("Error"),
			      msg);
	return false;
    }

    Log::print(4, "Core up ... OK");

    d->mainwin = new MainWin(0, this);
#if defined(Q_OS_WIN)
    d->mainwin->showMaximized();
#else
    d->mainwin->show();
#endif

    Log::print(4, "Main UI up ... OK");

    Settings::cleanup();
    return true;
}

bool Core::initializeDatabase()
{
    bool r;
    d->database = new Database();
    r = d->database->addDatabase(Settings::instance()->getHostName(),
			Settings::instance()->getDatabaseName(),
			Settings::instance()->getUserName(),
			Settings::instance()->getPassword());
    if(!r) {
	return false;
    }

    d->databaseQuery = new DatabaseQuery(0, this);
    return true;
}

MainWin *Core::mainwin() const
{
    return d->mainwin;
}

Database *Core::database() const
{
    return d->database;
}

DatabaseQuery *Core::databaseQuery() const
{
    return d->databaseQuery;
}
