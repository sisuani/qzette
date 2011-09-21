/*
 *
 *
 *
 */

#include "proxymodel.h"

ProxyModel::ProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void ProxyModel::clearFilter()
{
    columnsFilterHash.clear();
}

void ProxyModel::setFilterColumn(int column, const QString &filter)
{
    columnsFilterHash[column] = filter;
}

void ProxyModel::removeFilterColumn(int column)
{
    columnsFilterHash.remove(column);
}

void ProxyModel::filterBetweenColumns(int first, int last)
{
    if(first > last)
	last = first;
    m_first = first;
    m_last = last;
}

bool ProxyModel::filterAcceptsRow(int sourceRow,
	const QModelIndex &sourceParent) const
{
    QHash<int, QString>::const_iterator i = columnsFilterHash.constBegin();

    while(i != columnsFilterHash.end()) {
	QModelIndex index = sourceModel()->index(sourceRow, i.key(), sourceParent);
	if(sourceModel()->data(index).toString() != i.value())
	    return false;
	++i;
    }

    if(!filterRegExp().isEmpty()) {
	for(int j = m_first; j < m_last; j++) {
	    QModelIndex index = sourceModel()->index(sourceRow, j, sourceParent);
	    if(sourceModel()->data(index).toString().contains(filterRegExp()))
		return true;
	}
    } else {
	return true;
    }

    return false;
}

void ProxyModel::invalidateFilter()
{
    QSortFilterProxyModel::invalidateFilter();
}
