/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
						  sclistwidgetdelegate.cpp  -  description
							 -------------------
	begin                : Sat June 18 2011
	copyright            : (C) 2011 by Franz Schmid
	email                : Franz.Schmid@altmuehlnet.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QPainter>
#include "sclistwidgetdelegate.h"

ScListWidgetDelegate::ScListWidgetDelegate(QListWidget *view, QWidget *parent) : QItemDelegate(parent), m_view(view)
{
	m_dispIconOnly = false;
}

void ScListWidgetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (m_dispIconOnly)
	{
		painter->save();
		QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
		painter->drawPixmap(option.rect, icon.pixmap(m_view->iconSize()));
		QPalette::ColorGroup cg = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
		if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
			cg = QPalette::Inactive;
		if (option.state & QStyle::State_Selected)
		{
			QColor sele = option.palette.brush(cg, QPalette::Highlight).color();
			sele.setAlphaF(0.3);
			painter->fillRect(option.rect, sele);
		}
		painter->restore();
	}
	else
		QItemDelegate::paint(painter, option, index);
}

QSize ScListWidgetDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
	if (m_dispIconOnly)
	{
		QSize sz = m_view->iconSize();
		return sz;
	}
	return QItemDelegate::sizeHint(opt, index);
}

void ScListWidgetDelegate::setIconOnly(bool setter)
{
	m_dispIconOnly = setter;
}

bool ScListWidgetDelegate::iconOnly()
{
	return m_dispIconOnly;
}
