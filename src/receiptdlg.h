/*
 *
 *
 *
 */

#ifndef RECEIPTDLG_H
#define RECEIPTDLG_H

#include <QDialog>
#include <QDate>

#include "ui_receipt.h"

class QString;

class ReceiptDlg : public QDialog, public Ui::Receipt
{
    Q_OBJECT

public:
    ReceiptDlg(QWidget *parent = 0, const QString &receiptNum = 0, const QString &ownerName = 0,
      const QDate &date = QDate(), const QString &employed = 0, double amount = 0,
      const QString &buildingName = 0, const QString &buildingLocation = 0,
      const QString &concept = 0);

private slots:
    void print();
};

#endif // RECEIPTDLG_H
