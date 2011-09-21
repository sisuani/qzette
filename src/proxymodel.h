/*
 *
 *
 *
 *
 *
 */

#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QHash>


class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ProxyModel(QObject *parent = 0);

    void clearFilter();
    void setFilterColumn(int column, const QString &filter);
    void removeFilterColumn(int column);
    void filterBetweenColumns(int first, int last);
    void invalidateFilter();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QHash<int, QString> columnsFilterHash;

    int m_first;
    int m_last;
};

#endif //PROXYMODEL_H
