#ifndef NOTESSTYLESEDITOR_H
#define NOTESSTYLESEDITOR_H

#include "notesstyles.h"
#include "ui/scrpalettebase.h"
#include "ui/stylecombos.h"
#include "ui_notesstyleseditor.h"

class ScribusDoc;
class ScribusMainWindow;

class SCRIBUS_API NotesStylesEditor : public ScrPaletteBase, private Ui::NotesStylesEditor
{
	Q_OBJECT

public:
	explicit NotesStylesEditor(QWidget* parent = nullptr, const char *name = "notesStylesEditor");
	~NotesStylesEditor();

	void updateNSList();

protected:
	void changeEvent(QEvent *e) override;

private:
	ScribusDoc         *m_Doc { nullptr };
	PrefsContext       *m_prefs { nullptr };
	bool                m_addNewNsMode { false };
	QMap<QString, NotesStyle> changesMap; //<NSname to change, NSet new values>

	void readNotesStyle(const QString& nsName);
	void changeNotesStyle();
	void setBlockSignals(bool block);

public slots:
	void setDoc(ScribusDoc *doc);
	void handleUpdateRequest(int updateFlags);
	void languageChange();
	void setNotesStyle(NotesStyle* NS);

private slots:
	void on_NSlistBox_currentTextChanged(const QString &arg1);
	void on_ApplyButton_clicked();
	void on_DeleteButton_clicked();
	void on_OKButton_clicked();
	void on_NewNameEdit_textChanged(const QString &arg1);
	void on_FootRadio_toggled(bool checked);
	void on_EndRadio_toggled(bool checked);
	void on_NumberingBox_currentIndexChanged(int index);
	void on_RangeBox_currentIndexChanged(int index);
	void on_StartSpinBox_valueChanged(int arg1);
	void on_PrefixEdit_textChanged(const QString &arg1);
	void on_SuffixEdit_textChanged(const QString &arg1);
	void on_SuperMasterCheck_toggled(bool checked);
	void on_SuperNoteCheck_toggled(bool checked);
	void on_AutoH_toggled(bool checked);
	void on_AutoW_toggled(bool checked);
	void on_AutoWeld_toggled(bool checked);
	void on_AutoRemove_toggled(bool checked);
	void on_NewButton_clicked();
	void on_paraStyleCombo_currentIndexChanged(const int &arg1);
	void on_charStyleCombo_currentIndexChanged(const int &arg1);
};
#endif // NOTESSTYLESEDITOR_H
