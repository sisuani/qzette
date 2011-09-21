/*
 *
 *
 */

#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QWidget>

struct Options
{
    /* Options */
    int minimumPasswordLength;

    /* db */
    QString hostName;
    QString databaseName;
    QString userName;
    QString password;
    QString pwdDB;

    /* UI */
    bool toolBarToolsVisible;
    bool toolBarActionsVisible;
};

extern
Options option;

bool errorCommon(QWidget *parent, int WINDOW, int error);

enum Errors {
    DATABASE_ERROR
};

enum Mysql_Errors {
    ER_DUP_ENTRY = 1062,
    ER_ROW_IS_REFERENCED_2 = 1451
};

enum Windows {
    CUSTOMER_WINDOW = 0,
    SUPPLIER_WINDOW,
    EMPLOYED_WINDOW,
    BUILDING_WINDOW,
    RECEIPT_WINDOW,
    CREATERECEIPT_WINDOW,
    CONFIGURATION_DIALOG
};

#endif // COMMON_H
