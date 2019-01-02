/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "scxmlstreamreader.h"

#include "scclocale.h"

ScXmlStreamAttributes::ScXmlStreamAttributes()
{

}

ScXmlStreamAttributes::ScXmlStreamAttributes(const QXmlStreamAttributes& attrs)
					 : QXmlStreamAttributes(attrs)
{

}

bool ScXmlStreamAttributes::valueAsBool (const char* attrName, bool def) const
{
	bool retValue = def;
	QStringRef att = value(QLatin1String(attrName));
	if (!att.isEmpty())
	{
		bool success = false;
		QString strVal = QString::fromRawData(att.constData(), att.length());
		int intVal = strVal.toInt(&success);
		if (success)
			retValue = static_cast<bool>(intVal);
	}
	return retValue;
}

bool ScXmlStreamAttributes::valueAsBool (const QString& attrName, bool def) const
{
	bool retValue = def;
	QStringRef att = value(attrName);
	if (!att.isEmpty())
	{
		bool success = false;
		QString strVal = QString::fromRawData(att.constData(), att.length());
		int intVal = strVal.toInt(&success);
		if (success)
			retValue = static_cast<bool>(intVal);
	}
	return retValue;
}

int ScXmlStreamAttributes::valueAsInt (const char* attrName, int def) const
{
	int retValue = def;
	QStringRef att = value(QLatin1String(attrName));
	if (!att.isEmpty())
	{
		bool success = false;
		QString strVal = QString::fromRawData(att.constData(), att.length());
		int intVal = strVal.toInt(&success);
		if (success)
			retValue = intVal;
	}
	return retValue;
}

int ScXmlStreamAttributes::valueAsInt (const QString& attrName, int def) const
{
	int retValue = def;
	QStringRef att = value(attrName);
	if (!att.isEmpty())
	{
		bool success = false;
		QString strVal = QString::fromRawData(att.constData(), att.length());
		int intVal = strVal.toInt(&success);
		if (success)
			retValue = intVal;
	}
	return retValue;
}

int ScXmlStreamAttributes::valueAsInt (const char* attrName, int min, int max, int def) const
{
	int value = valueAsInt(attrName, def);
	return qMin(max, qMax(value, min));
}

int ScXmlStreamAttributes::valueAsInt (const QString& attrName, int min, int max, int def) const
{
	int value = valueAsInt(attrName, def);
	return qMin(max, qMax(value, min));
}

uint ScXmlStreamAttributes::valueAsUInt  (const char*    attrName, uint def) const
{
	uint retValue = def;
	QStringRef att = value(QLatin1String(attrName));
	if (!att.isEmpty())
	{
		bool success = false;
		QString strVal = QString::fromRawData(att.constData(), att.length());
		uint intVal = strVal.toUInt(&success);
		if (success)
			retValue = intVal;
	}
	return retValue;
}

uint ScXmlStreamAttributes::valueAsUInt  (const QString& attrName, uint def) const
{
	uint retValue = def;
	QStringRef att = value(attrName);
	if (!att.isEmpty())
	{
		bool success = false;
		QString strVal = QString::fromRawData(att.constData(), att.length());
		uint intVal = strVal.toUInt(&success);
		if (success)
			retValue = intVal;
	}
	return retValue;
}

double ScXmlStreamAttributes::valueAsDouble (const char* attrName, double def) const
{
	double retValue = def;
	QStringRef att = value(QLatin1String(attrName));
	if (!att.isEmpty())
	{
		QString strVal = QString::fromRawData(att.constData(), att.length());
		retValue = ScCLocale::toDoubleC(strVal, def);
	}
	return retValue;
}

double ScXmlStreamAttributes::valueAsDouble (const QString& attrName, double def) const
{
	double retValue = def;
	QStringRef att = value(attrName);
	if (!att.isEmpty())
	{
		QString strVal = QString::fromRawData(att.constData(), att.length());
		retValue = ScCLocale::toDoubleC(strVal, def);
	}
	return retValue;
}

QString ScXmlStreamAttributes::valueAsString (const char*    attrName, const QString& def) const
{
	QString retValue = def;
	QStringRef att = value(QLatin1String(attrName));
	if (!att.isEmpty() || hasAttribute(attrName))
		retValue = att.toString();
	return retValue;
}

QString ScXmlStreamAttributes::valueAsString (const QString& attrName, const QString& def) const
{
	QString retValue = def;
	QStringRef att = value(attrName);
	if (!att.isEmpty() || hasAttribute(attrName))
		retValue = att.toString();
	return retValue;
}

ScXmlStreamAttributes ScXmlStreamReader::scAttributes() const
{
	ScXmlStreamAttributes attrs(attributes());
	return attrs;
}

void ScXmlStreamReader::readToElementEnd()
{
	if (!isStartElement())
		return;
	int count = 1;
	QStringRef tagName = name();
	while (!atEnd() && !hasError())
	{
		readNext();
		if (isStartElement() && (name() == tagName))
			++count;
		if (isEndElement() && (name() == tagName))
			--count;
		if (count == 0)
			break;
	}
}

