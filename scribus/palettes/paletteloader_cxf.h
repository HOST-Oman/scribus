/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef PALETTELOADER_CXF_H
#define PALETTELOADER_CXF_H

#include <QString>
#include <QList>

class CxfObject;

#include "cxfdocument.h"
#include "paletteloader.h"
#include "scribusapi.h"
#include "vgradient.h"

#include "colormgmt/sccolormgmtstructs.h"
#include "colormgmt/scspectralvaluesconvertor.h"

class PaletteLoader_CxF : public PaletteLoader
{
public:
	PaletteLoader_CxF();
	
	// Examine the passed file and test to see whether it appears to be
	// loadable with this loader. This test must be quick and simple.
	// It need not verify a file, just confirm that it looks like a 
	// supported file type
	virtual bool isFileSupported(const QString & fileName) const;
	
	// Import colors from specified file
	virtual bool importFile(const QString& fileName, bool merge);
	
protected:
	CxfDocument m_cxfDoc;
	ScSpectralValuesConvertor m_spectrumConvertor;

	bool canImportObjectAsRgb(const CxfObject* object) const;
	bool canImportObjectAsCmyk(const CxfObject* object) const;
	bool canImportObjectAsLab(const CxfObject* object) const;

	typedef bool (PaletteLoader_CxF::*ColorImportFunction)(const CxfObject*);

	bool importObjectAsRgbColor(const CxfObject* object);
	bool importObjectAsCmykColor(const CxfObject* object);
	bool importObjectAsLabColor(const CxfObject* object);

	QList<eColorSpaceType> getAvailableColorspaces() const;
};

#endif
