/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef SCTEXTBROWSER_H
#define SCTEXTBROWSER_H

#include "scribusapi.h"

#include <QString>
#include <QTextBrowser>
#include <QUrl>


class SCRIBUS_API ScTextBrowser : public QTextBrowser
{
	Q_OBJECT

	QUrl homeUrl;

	public:
		ScTextBrowser( QWidget * parent = 0 );

		void clear();
		void find(const QString& txt, const int& options = 0);

	public slots:
		void home();

	private slots:
		void catchHome(const QUrl& url);
		void externalLinkClick(const QUrl& url);

	protected:
		QString m_baseDir;

};

#endif
