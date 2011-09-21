/*
 *
 *
 *
 */

#ifndef EMPLOYEDMODEL_H
#define EMPLOYEDMODEL_H

#include <QSqlRelationalTableModel>
#include <QPixmap>

class EmployedModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    EmployedModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    QVariant data(const QModelIndex &index, int role) const;

    enum {
	id = 0,
	LastName,
	Name,
	idCategory,
	Dni,
	Address,
	Location,
	CellPhone,
	Phone,
	Rating,
	RatingPix
    };

    enum {
	MaxRating = 5
    };
};

#endif // EMPLOYEDMODEL_H
