#ifndef RIBBONWIDGET_H
#define RIBBONWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QTabWidget>
#include <QMenu>
#include <QFrame>
#include <QButtonGroup>

class RibbonWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RibbonWidget(QWidget *parent = nullptr);

signals:
    void lightThemeClicked();
    void darkThemeClicked();
    void newFileClicked();
    void openFileClicked();
    void saveFileClicked();
    void exitClicked();
    void aboutClicked();

private slots:
    void onFileButtonClicked();
    void showBackstage();
    void hideBackstage();

private:
    void setupUI();
    void setupFileButton();
    void setupRibbonTabs();
    void setupBackstage();
    QWidget* createThemeGroup();        // 添加这行
    QWidget* createRibbonGroup(const QString &title, const QStringList &actions);
    QToolButton* createRibbonButton(const QString &text, const QString &iconName, const QString &tooltip = QString());
    
    QPushButton *m_fileButton;
    QTabWidget *m_tabWidget;
    QWidget *m_backstageWidget;
    QHBoxLayout *m_mainLayout;
    bool m_backstageVisible;
};

#endif // RIBBONWIDGET_H