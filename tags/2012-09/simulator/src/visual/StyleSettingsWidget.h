#ifndef QTOGRE_STYLESETTINGSWIDGET_H_
#define QTOGRE_STYLESETTINGSWIDGET_H_

#include "ui_StyleSettingsWidget.h"

#include <AbstractSettingsWidget.h>

namespace QtOgre
{

	class StyleSettingsWidget : public AbstractSettingsWidget, private Ui::StyleSettingsWidget
	{
		Q_OBJECT

	public:
		StyleSettingsWidget(QWidget *parent = 0);

		void disableFirstTimeOnlySettings(void);

	public slots:
		void readFromSettings(void);
		void writeToSettings(void);
		
	private slots:
		void chooseStyleFile(bool);
	};
}

#endif /*QTOGRE_STYLESETTINGSWIDGET_H_*/
