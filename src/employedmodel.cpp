/*
 *
 *
 *
 */

#include "employedmodel.h"

#include <QSqlRelationalTableModel>
#include <QPolygonF>
#include <QPainter>
#include <math.h>

EmployedModel::EmployedModel(QObject *parent, QSqlDatabase db)
    : QSqlRelationalTableModel(parent, db)
{
}

QVariant EmployedModel::data(const QModelIndex &idx, int role) const
{
    if(idx.column() == RatingPix) {
	if(role == Qt::DecorationRole) {
	    int rating = data(index(idx.row(), Rating), Qt::DisplayRole).toInt();
	    QPixmap pixmap((16 * MaxRating), 16);
	    pixmap.fill(Qt::transparent);
	    QPainter painter(&pixmap);
	    painter.setRenderHint(QPainter::Antialiasing, true);
	    painter.setBrush(QColor("#BBCCEE"));
	    painter.setPen(QColor("#BBCCEE"));
	    int i;
	    for(i = 0; i < MaxRating - rating; ++i) {
		QPolygonF polygon;
		polygon << QPointF((i * 16) + 8, 0) << QPointF(16 * i, 8) << QPointF((i * 16) + 8, 16) << QPointF((16 * i) + 16, 8);
		painter.drawPolygon(polygon);
	    }
	    painter.setBrush(Qt::NoBrush);
	    for(int i = MaxRating - rating; i < MaxRating; ++i) {
		QPolygonF polygon;
		polygon << QPointF((i * 16) + 8, 0) << QPointF(16 * i, 8) << QPointF((i * 16) + 8, 16) << QPointF((16 * i) + 16, 8);
		painter.drawPolygon(polygon);
	    }
	    return pixmap;
	}
    }

    return QSqlRelationalTableModel::data(idx, role);
}
