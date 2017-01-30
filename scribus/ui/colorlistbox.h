/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef COLORLISTBOX_H
#define COLORLISTBOX_H

#include <QListWidget>
#include <QColor>
#include <QPointer>

#include "colorsetmanager.h"
#include "commonstrings.h"
#include "scribusapi.h"
#include "sclistboxpixmap.h"
#include "scguardedptr.h"
#include "sccolor.h"

struct SCRIBUS_API ColorPixmapValue
{
	ScColor m_color;
	ScGuardedPtr<ScribusDoc> m_doc;
	QString m_name;

	ColorPixmapValue();
	ColorPixmapValue( const ScColor& col, ScribusDoc* doc, const QString colName );
	ColorPixmapValue(const ColorPixmapValue& other);
	ColorPixmapValue& operator= (const ColorPixmapValue& other);
};


Q_DECLARE_METATYPE(ColorPixmapValue)

class ColorPixmapItem : public QListWidgetItem
{
	enum usrType {
		ColorPixmapUserType = UserType + 1
	};
public:	
	ColorPixmapItem( const ScColor& col, ScribusDoc* doc, const QString colName) : QListWidgetItem(NULL, ColorPixmapUserType) { 
		setText(colName);
		setData(Qt::UserRole, QVariant::fromValue(ColorPixmapValue(col, doc, colName))); 
	};
	ColorPixmapItem( const ColorPixmapValue& col) : QListWidgetItem(NULL, ColorPixmapUserType) { 
		setText(col.m_name);
		setData(Qt::UserRole, QVariant::fromValue(col));
	};
	ColorPixmapItem( ) : QListWidgetItem(NULL, ColorPixmapUserType) { 
		setText(CommonStrings::tr_NoneColor);
		setData(Qt::UserRole, QVariant::fromValue(ColorPixmapValue(ScColor(0,0,0,0), NULL, CommonStrings::tr_NoneColor))); 
	};
	QListWidgetItem * clone () const { return new ColorPixmapItem(*this); }
	QString colorName() const { return data(Qt::UserRole).value<ColorPixmapValue>().m_name; }
};



/*! \brief Very nice list box with color names and samples.
It's inherited from QListBox with all its methods and properties.
I create it as separate class because it's used now in ColorManager
and ColorWheel too. You can see it in Extras/Color Wheel or in
Edit/Colors dialogs in action.
\author Petr Vanek <petr@yarpen.cz>
*/
class SCRIBUS_API ColorListBox : public QListWidget
{
	Q_OBJECT

	public:

		enum PixmapType
		{
			smallPixmap,
			widePixmap,
			fancyPixmap
		};

		/*! \brief Standard QListBox like constructor.
		Just there are initialized pixmaps for icon drawing. */
		ColorListBox(QWidget * parent = 0);
		ColorListBox(ColorListBox::PixmapType type, QWidget * parent = 0);
		~ColorListBox();

		virtual void changeEvent(QEvent *e);

		QString currentColor() const;

		/*! \brief Fill the list box with values taken from list.
		The list is cleared itself. Then is rendered an icon with
		color attributes (RGB/CMYK/Spot etc.).
		\param list a ColorList to present. */
		void updateBox(ColorList& list);

		/*! \brief Fill the list box with values taken from list.
		The list is not cleared before items insertion.
		\param list a ColorList to present. */
		void insertItems(ColorList& list);

		void addItem(ColorPixmapItem* item);
		void addItem(QString text);

		/*! \brief Retrieve the pixmap type used by this listbox */
		ColorListBox::PixmapType pixmapType() const { return m_type; }

		/*! \brief Set the pixmap type used by this listbox */
		void setPixmapType(ColorListBox::PixmapType type);
				
		/*! \brief Pointer to the color list displayed by this box */
		ColorList *cList;
	private slots:
		void slotRightClick();
	protected slots:
		virtual void languageChange();
	signals:
		void showContextMenue();
	protected:
		bool viewportEvent(QEvent *event);
		static int initialized;
		static int sortRule;
		ColorListBox::PixmapType m_type;
};

#endif
