/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
 *   Craig Bradney, cbradney@zip.com.au                                    *
 ***************************************************************************/

#ifndef SCRSPINBOX
#define SCRSPINBOX

#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QWidget>

#include "scribusapi.h"

//!\brief ScrSpinBox is a Qt4 replacement for our old ScrSpinBox using Qt3
class SCRIBUS_API ScrSpinBox : public QDoubleSpinBox
{
	Q_OBJECT
	public:
		ScrSpinBox(QWidget *parent, int unitIndex=0);
		ScrSpinBox(double minValue, double maxValue, QWidget *pa, int unitIndex=0);
		~ScrSpinBox() override;
		
		//overridden members
		double valueFromText ( const QString & text ) const override;
		QString textFromValue ( double value ) const override;
		QValidator::State validate ( QString & input, int & pos ) const override;
		void fixup ( QString & input ) const override;

		// call QDoubleSpinBox::setValue() without emitting valueChanged() signal
		void showValue(double val);
		
		//custom
		void init(int unitIndex);
		void setConstants(const QMap<QString, double>* constants);
		void setNewUnit(int unitIndex);
		double getValue(int unitIndex=0) const;

		uint unitIndex() const { return m_unitIndex; }
		double unitRatio() const;

		// Reimplement QDoubleSpinBox::stepBy() for angle wrapping
		void stepBy(int steps) override;
		
	public slots:
		void getValues(double *min, double *max, int *deci, double *val) const;
		void setValue(int val);
		void setValue(double val);
		void setValues(double min, double max, int deci, double val);
	
	protected:
		uint m_unitIndex;
		const QMap<QString, double>* m_constants;

		void setParameters(int s);
		bool eventFilter ( QObject * watched, QEvent * event ) override;
		
	protected slots:
		void textChanged();

};
#endif
