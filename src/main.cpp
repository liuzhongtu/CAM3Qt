#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QFile>
#include "MainWindow.h"
#include "ThemeManager.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	// Set application properties
	app.setApplicationName("FluentQtApp");
	app.setApplicationVersion("1.0.0");
	app.setOrganizationName("FluentDesign");

	// Use Fusion style as base
	app.setStyle(QStyleFactory::create("Fusion"));

	// Load and combine stylesheets
	QString combinedStyleSheet;

	// Load main fluent stylesheet
	QFile fluentFile(":/styles/fluent.qss");
	if (fluentFile.open(QFile::ReadOnly)) {
		combinedStyleSheet += QLatin1String(fluentFile.readAll());
	}

	// Load table stylesheet
	QFile tableFile(":/styles/table.qss");
	if (tableFile.open(QFile::ReadOnly)) {
		combinedStyleSheet += QLatin1String(tableFile.readAll());
	}

	app.setStyleSheet(combinedStyleSheet);

	ThemeManager::instance();

	MainWindow window;
	window.show();

	return app.exec();
}