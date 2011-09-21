/*
 *
 *
 */

#include <QtGui>
#include <QPrintDialog>

#include "receiptdlg.h"
#include "numbertoword.h"

ReceiptDlg::ReceiptDlg(QWidget *parent, const QString &receiptNum, const QString &ownerName,
  const QDate &date, const QString &employed, double amount,
  const QString &buildingName, const QString &buildingLocation,
  const QString &concept)
    : QDialog(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
//    layout()->setSizeConstraint(QLayout::SetFixedSize);

    lb_number->setText(receiptNum);
    lb_ownername->setText(ownerName.isEmpty() ? "ZETTE CONSTRUCCIONES" : ownerName);
    lb_date->setText(date.toString());
    lb_amount->setText(QString::number(amount, 'f', 2));
    QString numWord = NumberToWord::instance()->numberToWord(QString::number(amount).toStdString()).c_str();
    lb_amountText->setText(numWord);
    lb_buildingName->setText(buildingName);
    lb_buildingLocation->setText(buildingLocation);
    lb_concept->setText(concept);
    lb_name->setText(employed);

    connect(pb_close, SIGNAL(clicked()),
	    this, SLOT(close()));
    connect(pb_print, SIGNAL(clicked()),
	    this, SLOT(print()));
}

void ReceiptDlg::print()
{
    QPrinter printer;
    QPrintDialog printerDlg(&printer, this);
    if(printerDlg.exec()) {
	QFont defaultFont(font());
	QFont boldFont(font());
	boldFont.setBold(true);

	QPainter painter(&printer);
	QFontMetrics fm(font());
	int x = receiptFrame->x() + 40;
	int y = receiptFrame->y() + 45;
	int xRect = x;
	int yRect = receiptFrame->y() + 40;

	//
	x += 10;
	y += fm.height();
	QString str;
	str = lb_numberSymbol->text();
	painter.setFont(boldFont);
	fm = QFontMetrics(boldFont);
	painter.drawText(x, y, str);
	painter.setFont(defaultFont);
	painter.drawText(x + fm.width("  " + str), y, lb_number->text());
	painter.drawText(receiptFrame->width() + x - fm.width(lb_date->text() + "          "), y, lb_date->text());

	//
	fm = QFontMetrics(defaultFont);
	y += fm.height();
	str = lb_receive->text();
	str += lb_ownername->text();
	str += lb_receive_2->text();
	painter.setFont(boldFont);
	fm = QFontMetrics(boldFont);
	painter.drawText(x, y + 5, str);

	//
	y += fm.height() * 2;
	painter.setFont(defaultFont);
	painter.drawText(x + fm.width("   "), y + 5, lb_amountText->text());

	//
	fm = QFontMetrics(defaultFont);
	y += fm.height() * 2;
	str = lb_conceptText->text();
	painter.setFont(boldFont);
	fm = QFontMetrics(boldFont);
	painter.drawText(x, y, lb_concept->width(), fm.height() * 2, Qt::TextWordWrap, str);
	painter.setFont(defaultFont);
	painter.drawText(x + fm.width("  " + str), y , lb_concept->width(), fm.height() * 2, Qt::TextWordWrap, lb_concept->text());
	//
	fm = QFontMetrics(defaultFont);
	y += fm.height() * 3;
	str = lb_buildingNameText->text();
	painter.setFont(boldFont);
	fm = QFontMetrics(boldFont);
	painter.drawText(x, y, str);
	painter.setFont(defaultFont);
	painter.drawText(x + fm.width("  " + str), y, lb_buildingName->text());
	//
	fm = QFontMetrics(defaultFont);
	y += fm.height() * 2;
	str = lb_buildingLocationText->text();
	painter.setFont(boldFont);
	fm = QFontMetrics(boldFont);
	painter.drawText(x, y, str);
	painter.setFont(defaultFont);
	painter.drawText(x + fm.width("  " + str), y, lb_buildingLocation->text());
	//
	fm = QFontMetrics(defaultFont);
	y += fm.height() * 2;
	str = lb_cashSymbol->text();
	painter.setFont(boldFont);
	fm = QFontMetrics(boldFont);
	painter.drawText(x, y, str);
	painter.setFont(defaultFont);
	painter.drawText(x + fm.width("  " + str), y, lb_amount->text());
	//
	fm = QFontMetrics(defaultFont);
	y += fm.height() * 2;
	painter.drawLine(line->x() + x, y, line->x() + x + line->width() - 10, y);
	painter.drawText(lb_name->x() + x, y, lb_name->width(), lb_name->height(), Qt::AlignHCenter, lb_name->text());
	//
	painter.drawRect(xRect, yRect, receiptFrame->width() - 20, y - fm.height() * 2); // ?? y - fm.height() * 2
	//
	y += fm.height() * 3;
	painter.setPen(Qt::DashDotDotLine);
	painter.drawLine(xRect, y, receiptFrame->width() + xRect, y);
    }
}
