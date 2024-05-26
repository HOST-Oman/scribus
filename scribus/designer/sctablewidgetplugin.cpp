/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
For general Scribus copyright and licensing information please refer
to the COPYING file provided with the program.
*/


#include <QtPlugin>
#include "sctablewidgetplugin.h"
#include "../ui/sctablewidget.h"

ScTableWidgetPlugin::ScTableWidgetPlugin(QObject* parent)
	: QObject(parent), initialized(false)
{
}

void ScTableWidgetPlugin::initialize(QDesignerFormEditorInterface* core)
{
	Q_UNUSED(core);
	if (!initialized)
		initialized = true;
}

bool ScTableWidgetPlugin::isInitialized() const
{
	return initialized;
}

QWidget* ScTableWidgetPlugin::createWidget(QWidget* parent)
{
	return new ScTableWidget(parent);
}

QString ScTableWidgetPlugin::name() const
{
	return QLatin1String("ScTableWidget");
}

QString ScTableWidgetPlugin::group() const
{
	return QLatin1String("Scribus");
}

QIcon ScTableWidgetPlugin::icon() const
{
	return QIcon();
}

QString ScTableWidgetPlugin::toolTip() const
{
	return QString();
}

QString ScTableWidgetPlugin::whatsThis() const
{
	return QString();
}

bool ScTableWidgetPlugin::isContainer() const
{
	return false;
}

QString ScTableWidgetPlugin::domXml() const
{
	return QLatin1String("<widget class=\"ScTableWidget\" name=\"ScTableWidget1\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>180</width>\n"
		"   <height>80</height>\n"
		"  </rect>\n"
		" </property>\n"
		"</widget>\n");
}

QString ScTableWidgetPlugin::includeFile() const
{
	return QLatin1String("sctablewidget.h");
}

QString ScTableWidgetPlugin::codeTemplate() const
{
	return QString();
}

 Q_EXPORT_PLUGIN2(scribus_sctablewidget, ScTableWidgetPlugin)

