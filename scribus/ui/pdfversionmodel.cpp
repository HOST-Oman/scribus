/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "pdfversionmodel.h"

#include "commonstrings.h"
#include "pdfoptions.h"

PdfVersionModel::PdfVersionModel(QObject *parent)
	          : QAbstractItemModel(parent)
{
	
	m_enabledVec << true << true << true << false << false << false;
}

void PdfVersionModel::clear()
{
	qDebug() << "PdfVersionModel: this model cannot be cleared";
}

int PdfVersionModel::columnCount(const QModelIndex &/*parent*/) const
{
	return 1;
}

QVariant PdfVersionModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	bool* pEnabled = static_cast<bool*>(index.internalPointer());
	if (!pEnabled)
		return QVariant();

	if (role == Qt::DisplayRole)
	{
		int row = index.row();
		if (row == ItemPDF_13)
			return tr("PDF 1.3 (Acrobat 4)");
		if (row == ItemPDF_14)
			return tr("PDF 1.4 (Acrobat 5)");
		if (row == ItemPDF_15)
			return tr("PDF 1.5 (Acrobat 6)");
		if (row == ItemPDFX_1a)
			return tr("PDF/X-1a");
		if (row == ItemPDFX_3)
			return tr("PDF/X-3");
		if (row == ItemPDFX_4)
			return tr("PDF/X-4");
		return QVariant();
	}

	return QVariant();
}

Qt::ItemFlags PdfVersionModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return nullptr;

	Qt::ItemFlags flags = nullptr;
	if (m_enabledVec[index.row()])
		flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	return flags;
}

QModelIndex PdfVersionModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	bool* pEnabled = static_cast<bool*>(parent.internalPointer());
	if (pEnabled)
		return QModelIndex();

	if (row < 0 || (row >= rowCount()) || (column != 0))
		return QModelIndex();

	const bool& modeEnabled = m_enabledVec.at(row);
	return createIndex(row, column, const_cast<bool*>(&modeEnabled));
}

QModelIndex PdfVersionModel::parent(const QModelIndex &/*child*/) const
{
	return QModelIndex();
}

bool PdfVersionModel::removeRow(int row, const QModelIndex& parent)
{
	qDebug() << "PdfVersionModel: this model cannot have rows removed";
	return false;
}

bool PdfVersionModel::removeRows(int row, int count, const QModelIndex& parent)
{
	qDebug() << "PdfVersionModel: this model cannot have rows removed";
	return false;
}

int PdfVersionModel::rowCount(const QModelIndex &parent) const
{
	if (m_enabledVec.count() == 0)
		return 0;

	bool* pEnabled = static_cast<bool*>(parent.internalPointer());
	if (pEnabled)
		return 0;

	return m_enabledVec.count();
}

void PdfVersionModel::setPdfXEnabled(bool enabled)
{
	//beginResetModel();
	m_enabledVec[ItemPDFX_1a] = enabled;
	m_enabledVec[ItemPDFX_3]  = enabled;
	m_enabledVec[ItemPDFX_4]  = enabled;
	//endResetModel();
}
