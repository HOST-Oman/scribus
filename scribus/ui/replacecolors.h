/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
*   Copyright (C) 2008 by Franz Schmid                                   *
*   franz.schmid@altmuehlnet.de                                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
***************************************************************************/

#ifndef REPLACECOLORSDIALOG_H
#define REPLACECOLORSDIALOG_H

#include <QDialog>
#include <QPixmap>
#include "ui_replacecolors.h"
#include "scribusapi.h"
#include "sccolor.h"

class SCRIBUS_API replaceColorsDialog : public QDialog, Ui::replaceColorsDialog
{
	Q_OBJECT

public:
	replaceColorsDialog(QWidget* parent, ColorList &colorList, ColorList &colorListUsed);
	~replaceColorsDialog() {};
	ColorList EditColors;
	ColorList UsedColors;
	QMap<QString,QString> replaceMap;

private slots:
	void addColor();
	void selReplacement(int sel);
	void delReplacement();
	void editReplacement();

private:
	void updateReplacementTable();
	QPixmap getColorIcon(QString color);
	int selectedRow;
	QPixmap alertIcon;
	QPixmap cmykIcon;
	QPixmap rgbIcon;
	QPixmap labIcon;
	QPixmap spotIcon;
	QPixmap regIcon;
};

#endif
