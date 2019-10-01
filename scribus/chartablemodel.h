/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef CHARTABLEMODEL_H
#define CHARTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QMimeData>

#include "scribusapi.h"

class ScribusDoc;
class ScFace;
class QItemSelectionModel;


//! \brief A special type for character classes
typedef QList<uint> CharClassDef;


/*! \brief A model (MVC) to handle unicode characters map.
It's a backend for CharTableView - its GUI representation.
\warning: CharTableModel and CharTableView are designed for 1:1 relations!
\author Petr Vanek <petr@scribus.info>
*/
class SCRIBUS_API CharTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	CharTableModel(QObject* parent = nullptr, int cols = 4, ScribusDoc* doc = nullptr, const QString & font = nullptr);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	//! \brief Get a graphics representation/pixmap of the glyph
	QVariant data(const QModelIndex& index,
				  int role = Qt::DisplayRole) const override;

	void setFontInUse(const QString& font);

	//! \brief Font in use. It's used in model's view.
	ScFace fontFace();

	void setCharacters(const CharClassDef& ch);
	void setCharactersAndFonts(const CharClassDef& ch, const QStringList& fonts);
	void addCharacter(const QString& ch);
	CharClassDef characters() { return m_characters; }
	QStringList fonts() { return m_fonts; }

	//! \brief called to erase glyph at index from table.
	bool removeCharacter(int index);

	void setDoc(ScribusDoc *doc);

	void setViewWidth(int w) {
		m_viewWidth = w;
	};

public slots:
	/*! \brief appends an unicode char into m_characters list.
	\param s a QString with numerical representation of the character.
	\param base an optional parameter containing base of the numerical converion. See QString::toInt() documentation.
	The base parameter is used mainly in normal code - not in slot calls.
	If user adds an already existing glyph it's rejected and the original
	one is selected (see selectionChanged()).
	*/
	void appendUnicode(const QString & s, uint base = 16);

signals:
	/*! \brief Inform its view about internal selection changes.
	It's emitted every time a user adds an existing glyph to the
	CharClassDef list. */
	void selectionChanged(QItemSelectionModel * model);
	//! \brief Emitted when there is a new row
	void rowAppended();

private:

	enum DataRole
	{
		CharTextRole = Qt::UserRole + 1,
		CharTextAndFontRole = Qt::UserRole + 2
	};

	ScribusDoc *m_doc;
	//! \brief Number of the columns for model
	int m_cols;
	//! \brief View's width to compute pixmap sizes.
	int m_viewWidth {200};

	QString m_fontInUse;
	CharClassDef m_characters;
	QStringList m_fonts;

	//! \brief Internal selection handling. See selectionChanged().
	QItemSelectionModel * m_selectionModel;

	/*! \brief All drag'n'drop actions are handled in this model only
	See Qt4 docs "Using Drag and Drop with Item Views" for more info.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	Qt::DropActions supportedDropActions() const override;
	QStringList mimeTypes() const override;
	QMimeData* mimeData(const QModelIndexList& indexes) const override;
	bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) override;
};

#endif
