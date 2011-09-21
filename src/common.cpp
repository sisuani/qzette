/*
 *
 *
 *
 */

#include "common.h"

#include <QMessageBox>
#include <QObject>

#include "log.h"

Options option;

bool errorCommon(QWidget *parent, int WINDOW, int error)
{
    Log::error("errorHandling() : WINDOW -> " +
	        QString::number(WINDOW) + " | error -> " +
	        QString::number(error));

    QString msg;
    bool handle = false;
    switch(error) {
	case ER_DUP_ENTRY:
	    msg = QObject::trUtf8("Error, dato duplicado.");
	    handle = true;
	    break;
	case ER_ROW_IS_REFERENCED_2:
	    msg = QObject::trUtf8("Error, el dato ");
	    msg += QObject::trUtf8("no se puede eliminar.\n");
	    msg += QObject::trUtf8("Existe una referencia cruzada.");
	    handle = true;
	    break;
	default:
	    msg = QObject::trUtf8("DB: Error desconocido!\n");
	    msg += QObject::trUtf8("Ventana: ");
	    msg += QString::number(WINDOW);
	    msg += QObject::trUtf8(" - Error: ");
	    msg += QString::number(error);
	    break;
    }

    QMessageBox::critical(parent, ("Error"), msg);
    return handle;
}
