/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef PAGEPALETTE_WIDGETS_H
#define PAGEPALETTE_WIDGETS_H

#include <QCheckBox>
#include <QDialog>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QToolTip>
#include <QVBoxLayout>

class QEvent;

#include "scribusapi.h"
#include "scdockpalette.h"

class ScribusView;
class ScribusMainWindow;
class PageLayouts;

class SCRIBUS_API SeItem : public QTableWidgetItem
{
	friend class PagePalette;
	friend class PagePalette_Pages;
	friend class SeView;

public:
	SeItem(const QString& text, uint pgnr, const QPixmap& Pix);
	~SeItem() {};

	const QString& getPageName();
	uint pageNumber;
	
protected:
	QString pageName;
};

class SCRIBUS_API SeList : public QListWidget
{
	friend class PagePalette;
	friend class PagePalette_Pages;

	Q_OBJECT

public:
	SeList(QWidget* parent);
	~SeList() {};

private slots:
	void toggleThumbnail();

signals:
	//! Emitted when user requests/disables the thumbnais for master pages.
	void thumbnailChanged();
	void DelMaster(QString);

protected:
	void mouseReleaseEvent(QMouseEvent *m);
	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	virtual void keyPressEvent(QKeyEvent* e);	
	
	QPoint Mpos;
	QListWidgetItem *CurItem;
	bool Mpressed;
	bool Thumb;

};

class SCRIBUS_API SeView : public QTableWidget
{
	Q_OBJECT
	
	friend class PagePalette;
	friend class PagePalette_Pages;

public:
	SeView(QWidget* parent);
	~SeView() {};

	void ClearPix();
	int GetPage(int r, int c, bool *last);
	SeItem* GetPageItem(int pageIndex);

public slots:
	void ToggleNam();

signals:
	void UseTemp(QString, int);
	void NewPage(int, QString);
	void movePage(int, int);
	void Click(int, int, int);
	void DelPage(int);

protected:
	virtual void dropEvent(QDropEvent * e);
	virtual void dragEnterEvent(QDragEnterEvent *e);
	virtual void dragLeaveEvent(QDragLeaveEvent *e);
	virtual void dragMoveEvent(QDragMoveEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *m);
	virtual void mousePressEvent(QMouseEvent* e);
	virtual void mouseMoveEvent(QMouseEvent* e);
	virtual void keyPressEvent(QKeyEvent* e);
	
	QPoint Mpos;
	bool Mpressed;
	bool Namen;
	int MaxC;
	int colmult;
	int rowmult;
	int coladd;
	int rowadd;
	int cols;
	int firstP;
};

class SCRIBUS_API TrashBin : public QLabel
{
	Q_OBJECT

public:
	TrashBin( QWidget * parent );
	~TrashBin() {};
	void dragEnterEvent( QDragEnterEvent *e );
	void dragLeaveEvent( QDragLeaveEvent * );
	void dropEvent( QDropEvent * e );

protected:
	QPixmap Normal;
	QPixmap Offen;
	
signals:
	void DelPage(int);
	void DelMaster(QString);
};

#endif

