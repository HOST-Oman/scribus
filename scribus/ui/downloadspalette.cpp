#include "downloadspalette.h"

DownloadsPalette::DownloadsPalette(QWidget* parent) : ScrPaletteBase(parent, "DownloadsPalette", false, Qt::WindowFlags())
{
	setupUi(this);
	languageChange();
}

void DownloadsPalette::languageChange()
{
#if defined(Q_OS_MACOS)
	showInButton->setText( tr( "Show In Finder" ) );
#endif
#if defined(Q_OS_WIN32)
	showInButton->setText( tr( "Show In Explorer" ) );
#endif
	removeButton->setText( tr( "Remove" ) );
	removeAllButton->setText( tr( "Remove All" ) );
}

void DownloadsPalette::addDownload(const QString& name, const QString& source, const QString& destination)
{

}

