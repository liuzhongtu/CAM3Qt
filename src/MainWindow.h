#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTableWidget>

class OpenGLWidget;
class RibbonWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:
	void onTreeItemClicked(QTreeWidgetItem* item, int column);
	void onThemeChanged();                              // 添加这行
	void onNewFile();
	void onOpenFile();
	void onSaveFile();
	void onExit();
	void onAbout();

private:
	void setupUI();
	void setupRibbon();
	void setupDockWidget();
	void setupBottomDockWidget();
    void setupCentralTabWidget();        // 添加这行
    void setupOpenGLWidget();
    void setupReportWidget();            // 添加这行
    void setupToolBar();
	void setupStatusBar();
	void setupTreeWidget();
	void setupDialogWidget();
	void setupTableWidget();
	void setupThemeMenu(); 

	RibbonWidget* m_ribbonWidget;
	QDockWidget* m_dockWidget;
	QDockWidget* m_dialogDockWidget;
	QDockWidget* m_bottomDockWidget;

	QTreeWidget* m_treeWidget;
	QWidget* m_dialogWidget;
	QTableWidget* m_tableWidget;
	QToolBar* m_tableToolBar;

	OpenGLWidget* m_openGLWidget;
    QTabWidget *m_centralTabWidget;      // 添加这行
    QWidget *m_reportWidget;             // 添加这行
	QToolBar* m_rightToolBar;
	QStatusBar* m_statusBar;
	QWidget* m_centralWidget;
};

#endif