/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
 *   Riku Leino, tsoots@gmail.com                                          *
 ***************************************************************************/
#include "nftdialog.h"
#include <QPushButton>

#include "scconfig.h"
#include "scribusapi.h"
#include "iconmanager.h"


nftdialog::nftdialog(QWidget* parent, const QString& lang) : QDialog(parent)
{
	setupUi(this);
	setModal(true);
	setWindowIcon(IconManager::instance()->loadIcon("AppIcon.png"));
	nftGui->setupSettings(lang);
	buttonBox->button(QDialogButtonBox::Cancel)->setDefault(true);
	buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
	connect(nftGui, SIGNAL(leaveOK()), this, SLOT(accept()));
	connect(nftGui, SIGNAL(ButtonBoxEnable(bool)), this, SLOT(enableButtonBox(bool)));
}

bool nftdialog::isTemplateSelected()
{
	return nftGui->currentDocumentTemplate != nullptr;
}

nfttemplate* nftdialog::currentTemplate()
{
	return (nftGui->currentDocumentTemplate);
}

void nftdialog::enableButtonBox(bool setter)
{
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(setter);
	buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
}
