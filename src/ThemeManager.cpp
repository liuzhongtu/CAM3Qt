#include "ThemeManager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QApplication>

ThemeManager* ThemeManager::m_instance = nullptr;

ThemeManager* ThemeManager::instance()
{
	if (!m_instance) {
		m_instance = new ThemeManager();
	}
	return m_instance;
}

ThemeManager::ThemeManager(QObject* parent)
	: QObject(parent)
	, m_currentTheme(Theme::Dark)
	, m_settings(new QSettings("CAM3", "Theme", this))
{
	loadTheme();
}

void ThemeManager::setTheme(Theme theme)
{
	if (m_currentTheme != theme) {
		m_currentTheme = theme;
		applyTheme(theme);
		saveTheme();
		emit themeChanged(theme);
	}
}

Theme ThemeManager::currentTheme() const
{
	return m_currentTheme;
}

void ThemeManager::saveTheme()
{
	m_settings->setValue("theme", static_cast<int>(m_currentTheme));
	m_settings->sync();
}

void ThemeManager::loadTheme()
{
	int themeValue = m_settings->value("theme", static_cast<int>(Theme::Dark)).toInt();
	m_currentTheme = static_cast<Theme>(themeValue);
	applyTheme(m_currentTheme);
}

QString ThemeManager::getThemeName(Theme theme) const
{
	switch (theme) {
	case Theme::Dark:
		return "深色主题";
	case Theme::Light:
		return "浅色主题";
	default:
		return "未知主题";
	}
}

QStringList ThemeManager::getAvailableThemes() const
{
	return { getThemeName(Theme::Dark), getThemeName(Theme::Light) };
}

void ThemeManager::applyTheme(Theme theme)
{
	QString combinedStyleSheet;

	// 加载主题样式
	switch (theme) {
	case Theme::Dark:
		combinedStyleSheet += loadStyleSheet(":/styles/fluent.qss");
		break;
	case Theme::Light:
		combinedStyleSheet += loadStyleSheet(":/styles/light.qss");
		break;
	}

	// 加载表格样式
	combinedStyleSheet += loadStyleSheet(":/styles/table.qss");

	// 应用样式
	if (QApplication::instance()) {
		static_cast<QApplication*>(QApplication::instance())->setStyleSheet(combinedStyleSheet);
	}

	qDebug() << "Applied theme:" << getThemeName(theme);
}

QString ThemeManager::loadStyleSheet(const QString& fileName)
{
	QFile file(fileName);
	if (file.open(QFile::ReadOnly)) {
		QTextStream stream(&file);
		return stream.readAll();
	}
	else {
		qWarning() << "Could not load stylesheet:" << fileName;
		return QString();
	}
}