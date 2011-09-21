/*
 *
 *
 */

#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "database.h"
#include "databasequery.h"
#include "common.h"

class MainWin;

class Core : public QObject
{
    Q_OBJECT

public:
    Core();
    ~Core();

    bool init();
    bool login();

    MainWin *mainwin() const;
    Database *database() const;
    DatabaseQuery *databaseQuery() const;

private:
    bool initializeDatabase();

    class Private;
    Private *d;
};

#endif // CORE_H
