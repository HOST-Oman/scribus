#include "markvariabletext.h"
#include "marks.h"
#include <QComboBox>

MarkVariableText::MarkVariableText(const QList<Mark*>& marks, QWidget *parent) : MarkInsert(marks, parent), m_widgetType(ComboBox), m_mark(0)
{
	//for editing mark entry in text - user can change mark pointer inserted into text or create new mark entry
	setupUi(this);

	labelEditWidget = (QWidget*) new QComboBox(parent);
	((QComboBox*) labelEditWidget)->addItem(tr("New Mark"), QVariant::fromValue((void*) NULL));
	for (int i = 0; i < marks.size(); ++i)
	{
		if (marks[i]->isType(MARKVariableTextType))
			((QComboBox*) labelEditWidget)->addItem(marks[i]->label, QVariant::fromValue((void*) marks[i]));
	}
	((QComboBox*) labelEditWidget)->setEditable(true);
	((QComboBox*) labelEditWidget)->lineEdit()->setToolTip(tr("Edit selected Mark's label. To create new Mark select \"New Mark\" in drop list and input new label."));
	connect(((QComboBox*) labelEditWidget), SIGNAL(currentIndexChanged(int)),this,SLOT(onLabelList_currentIndexChanged(int)));
	labelLayout->addWidget(labelEditWidget);
	textEdit->setToolTip(tr("Edit variable text"));
	setWindowTitle(tr("Mark with Variable Text"));
}

MarkVariableText::MarkVariableText(const Mark* mark, QWidget *parent) : MarkInsert(mark, parent), m_widgetType(LineEdit), m_mark(mark)
{
	//for editing by marks Manager - user can change label and variable text
	setupUi(this);

	labelEditWidget = (QWidget*) new QLineEdit(parent);
	((QLineEdit*) labelEditWidget)->setToolTip(tr("Edit selected Mark's label"));

	labelLayout->addWidget(labelEditWidget);
	textEdit->setToolTip(tr("Edit variable text"));
	setWindowTitle(tr("Mark with Variable Text"));
}

MarkVariableText::~MarkVariableText()
{
	//delete ui;
	delete labelEditWidget;
}

Mark* MarkVariableText::values(QString& label, QString& text)
{
	text = textEdit->text();
	//hack to read if it is QComboBox or QLineEdit
	if (m_widgetType == ComboBox)
	{
		QComboBox* comboBox = (QComboBox*) labelEditWidget;
		int labelID = comboBox->currentIndex();
		label = comboBox->currentText();
		if (label == tr("New Mark"))
			label = "VariableMark";
		return (Mark*) (comboBox->itemData(labelID).value<void*>());
	}
	label = ((QLineEdit*) labelEditWidget)->text();
	if (label == tr("New Mark"))
		label = "VariableMark";
	return const_cast<Mark*> (m_mark);
}

void MarkVariableText::setValues(QString label, QString text)
{
	if (m_widgetType == LineEdit)
		((QLineEdit*) labelEditWidget)->setText(label);
	else
	{
		QComboBox* comboBox = (QComboBox*) labelEditWidget;
		comboBox->setCurrentIndex(comboBox->findText(label));
	}
	textEdit->setText(text);
}

void MarkVariableText::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			retranslateUi(this);
			break;
		default:
			break;
	}
}

void MarkVariableText::onLabelList_currentIndexChanged(int index)
{
	if (index > 0)
	{
		Mark* mrk = (Mark*) ((QComboBox*) labelEditWidget)->itemData(index).value<void*>();
		textEdit->setText(mrk->getString());
	}
	else
	{
		((QComboBox*) labelEditWidget)->setEditText("");
		textEdit->clear();
	}
}
