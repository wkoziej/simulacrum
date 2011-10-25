#include "StyleSettingsWidget.h"

#include "Application.h"

#include <QSettings>
#include <QFileDialog>

namespace QtOgre
{
	StyleSettingsWidget::StyleSettingsWidget(QWidget *parent)
	:AbstractSettingsWidget(parent)
	{
		setupUi(this);
		
		connect(browseButton, SIGNAL(clicked(bool)), this, SLOT(chooseStyleFile(bool)));
	}

	void StyleSettingsWidget::disableFirstTimeOnlySettings(void)
	{
		//None
	}

	void StyleSettingsWidget::readFromSettings(void)
	{
		fileLineEdit->setText(mSettings->value("UI/StyleFile").toString());
	}

	void StyleSettingsWidget::writeToSettings(void)
	{
		mSettings->setValue("UI/StyleFile", fileLineEdit->text());
		QFile styleFile(fileLineEdit->text());
		styleFile.open(QIODevice::ReadOnly | QIODevice::Text);
		qApp->setStyleSheet(QString(styleFile.readAll()));
		styleFile.close();
	}
	
	void StyleSettingsWidget::chooseStyleFile(bool checked)
	{
		//QFileDialog fileDialog(this, "Style file");
		//fileDialog.setNameFilter("Qt Style Files (*.qss)");
		fileLineEdit->setText(QFileDialog::getOpenFileName(this, "Style file", "", "Qt Style Files (*.qss)"));
	}
}
