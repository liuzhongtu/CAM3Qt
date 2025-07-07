#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QApplication>
#include <QSettings>

enum class Theme {
    Dark,
    Light
};

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    static ThemeManager* instance();
    
    void setTheme(Theme theme);
    Theme currentTheme() const;
    
    void saveTheme();
    void loadTheme();
    
    QString getThemeName(Theme theme) const;
    QStringList getAvailableThemes() const;

signals:
    void themeChanged(Theme theme);

private:
    explicit ThemeManager(QObject *parent = nullptr);
    void applyTheme(Theme theme);
    QString loadStyleSheet(const QString& fileName);
    
    static ThemeManager* m_instance;
    Theme m_currentTheme;
    QSettings* m_settings;
};

#endif // THEMEMANAGER_H