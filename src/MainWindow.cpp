#include "MainWindow.h"
#include "OpenGLWidget.h"
#include "RibbonWidget.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>
#include <QProgressBar>
#include <QSplitter>
#include <QLineEdit>
#include <QTextEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QTabWidget>
#include <QTextEdit>
#include <QScrollArea>
#include<QComboBox>
#include <QTabWidget>
#include <QActionGroup>
#include "ThemeManager.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, m_ribbonWidget(nullptr)
	, m_dockWidget(nullptr)
	, m_treeWidget(nullptr)
	, m_openGLWidget(nullptr)
	, m_rightToolBar(nullptr)
	, m_statusBar(nullptr)
	, m_centralWidget(nullptr)
{
	setWindowTitle("Fluent Qt Application");
	setWindowIcon(QIcon(":/icons/app_icon.png"));
	resize(1200, 800);

	setupUI();

	//	setupThemeMenu();
	connect(ThemeManager::instance(), &ThemeManager::themeChanged,
		this, &MainWindow::onThemeChanged);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupThemeMenu()
{
	// 在菜单栏添加主题选择
	QMenuBar* menuBar = this->menuBar();
	QMenu* viewMenu = menuBar->addMenu("视图");
	QMenu* themeMenu = viewMenu->addMenu("主题");

	QActionGroup* themeGroup = new QActionGroup(this);

	// 深色主题
	QAction* darkAction = new QAction("深色主题", this);
	darkAction->setCheckable(true);
	darkAction->setActionGroup(themeGroup);
	connect(darkAction, &QAction::triggered, [this]() {
		ThemeManager::instance()->setTheme(Theme::Dark);
	});
	themeMenu->addAction(darkAction);

	// 浅色主题
	QAction* lightAction = new QAction("浅色主题", this);
	lightAction->setCheckable(true);
	lightAction->setActionGroup(themeGroup);
	connect(lightAction, &QAction::triggered, [this]() {
		ThemeManager::instance()->setTheme(Theme::Light);
	});
	themeMenu->addAction(lightAction);

	// 设置当前主题
	Theme currentTheme = ThemeManager::instance()->currentTheme();
	if (currentTheme == Theme::Dark) {
		darkAction->setChecked(true);
	}
	else {
		lightAction->setChecked(true);
	}
}

void MainWindow::onThemeChanged()
{
	// 主题改变时的处理
	Theme currentTheme = ThemeManager::instance()->currentTheme();
	QString themeName = ThemeManager::instance()->getThemeName(currentTheme);
	statusBar()->showMessage(QString("已切换到%1").arg(themeName), 2000);

	// 可以在这里添加其他主题切换时需要更新的内容
}

void MainWindow::setupUI()
{
	setupRibbon();
	setupDockWidget();
	setupCentralTabWidget();        // 先创建tab widget
	setupBottomDockWidget();
	setupToolBar();
	setupStatusBar();

	// Create central widget with splitter
	m_centralWidget = new QWidget;
	setCentralWidget(m_centralWidget);

	QHBoxLayout* centralLayout = new QHBoxLayout(m_centralWidget);
	centralLayout->setContentsMargins(0, 0, 0, 0);
	centralLayout->setSpacing(0);

	QSplitter* splitter = new QSplitter(Qt::Horizontal);
	splitter->addWidget(m_centralTabWidget);
	splitter->setSizes({ 800, 200 });

	centralLayout->addWidget(splitter);
}

void MainWindow::setupRibbon()
{
	m_ribbonWidget = new RibbonWidget(this);

	connect(m_ribbonWidget, &RibbonWidget::lightThemeClicked, this, []() {
		ThemeManager::instance()->setTheme(Theme::Light);
	});

	connect(m_ribbonWidget, &RibbonWidget::darkThemeClicked, this, []() {
		ThemeManager::instance()->setTheme(Theme::Dark);
	});


	// Connect ribbon actions
	connect(m_ribbonWidget, &RibbonWidget::newFileClicked, this, &MainWindow::onNewFile);
	connect(m_ribbonWidget, &RibbonWidget::openFileClicked, this, &MainWindow::onOpenFile);
	connect(m_ribbonWidget, &RibbonWidget::saveFileClicked, this, &MainWindow::onSaveFile);
	connect(m_ribbonWidget, &RibbonWidget::exitClicked, this, &MainWindow::onExit);
	connect(m_ribbonWidget, &RibbonWidget::aboutClicked, this, &MainWindow::onAbout);

	// Set ribbon as menu bar replacement
	setMenuWidget(m_ribbonWidget);
}

void MainWindow::setupDockWidget()
{
	// 第一个dock - 项目浏览器
	m_dockWidget = new QDockWidget("项目浏览器", this);
	m_dockWidget->setObjectName("ProjectExplorerDock");
	m_dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	// 创建dock内容的主容器
	QWidget* dockContent = new QWidget;
	QVBoxLayout* dockLayout = new QVBoxLayout(dockContent);
	dockLayout->setContentsMargins(0, 0, 0, 0);
	dockLayout->setSpacing(0);

	// 创建顶部工具栏区域
	QWidget* toolbarWidget = new QWidget;
	toolbarWidget->setObjectName("ProjectToolbar");
	QHBoxLayout* toolbarLayout = new QHBoxLayout(toolbarWidget);
	toolbarLayout->setContentsMargins(8, 6, 8, 6);
	toolbarLayout->setSpacing(6);

	// 添加ComboBox
	QComboBox* projectCombo = new QComboBox;
	projectCombo->setObjectName("ProjectCombo");
	projectCombo->addItems({ "Project Structure", "File Explorer", "Class View", "Solution Explorer" });
	projectCombo->setCurrentText("Project Structure");

	// 添加按钮
	QPushButton* actionButton = new QPushButton("⚙");
	actionButton->setObjectName("ProjectActionButton");
	actionButton->setFixedSize(24, 24);
	actionButton->setToolTip("项目设置");

	toolbarLayout->addWidget(projectCombo, 1);  // 拉伸因子为1，占据大部分空间
	toolbarLayout->addWidget(actionButton, 0);  // 拉伸因子为0，保持固定大小

	// 设置树形控件
	setupTreeWidget();

	// 将工具栏和树形控件添加到主布局
	dockLayout->addWidget(toolbarWidget);
	dockLayout->addWidget(m_treeWidget, 1);  // 树形控件占据剩余空间

	m_dockWidget->setWidget(dockContent);

	// 其余dock设置保持不变...
	m_dialogDockWidget = new QDockWidget("对话框", this);
	m_dialogDockWidget->setObjectName("DialogDock");
	m_dialogDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	setupDialogWidget();
	m_dialogDockWidget->setWidget(m_dialogWidget);

	addDockWidget(Qt::LeftDockWidgetArea, m_dockWidget);
	addDockWidget(Qt::LeftDockWidgetArea, m_dialogDockWidget);

	tabifyDockWidget(m_dockWidget, m_dialogDockWidget);
	m_dockWidget->raise();
}

void MainWindow::setupTreeWidget()
{
	m_treeWidget = new QTreeWidget;
	//	m_treeWidget->setHeaderLabel("Project Structure");
	m_treeWidget->setObjectName("ProjectTree");

	// 添加这行来隐藏header
	m_treeWidget->setHeaderHidden(true);

	// Create sample tree structure with three levels
	QTreeWidgetItem* rootItem = new QTreeWidgetItem(m_treeWidget);
	rootItem->setText(0, "Project Root");
	rootItem->setIcon(0, QIcon(":/icons/folder.png"));
	rootItem->setExpanded(true);

	// Level 2 items
	QTreeWidgetItem* srcItem = new QTreeWidgetItem(rootItem);
	srcItem->setText(0, "Source Files");
	srcItem->setIcon(0, QIcon(":/icons/folder.png"));
	srcItem->setExpanded(true);

	QTreeWidgetItem* resItem = new QTreeWidgetItem(rootItem);
	resItem->setText(0, "Resources");
	resItem->setIcon(0, QIcon(":/icons/folder.png"));
	resItem->setExpanded(true);

	QTreeWidgetItem* docsItem = new QTreeWidgetItem(rootItem);
	docsItem->setText(0, "Documentation");
	docsItem->setIcon(0, QIcon(":/icons/folder.png"));

	// Level 3 items
	QTreeWidgetItem* mainCppItem = new QTreeWidgetItem(srcItem);
	mainCppItem->setText(0, "main.cpp");
	mainCppItem->setIcon(0, QIcon(":/icons/cpp.png"));

	QTreeWidgetItem* mainWindowItem = new QTreeWidgetItem(srcItem);
	mainWindowItem->setText(0, "MainWindow.cpp");
	mainWindowItem->setIcon(0, QIcon(":/icons/cpp.png"));

	QTreeWidgetItem* openglItem = new QTreeWidgetItem(srcItem);
	openglItem->setText(0, "OpenGLWidget.cpp");
	openglItem->setIcon(0, QIcon(":/icons/cpp.png"));

	QTreeWidgetItem* styleItem = new QTreeWidgetItem(resItem);
	styleItem->setText(0, "fluent.qss");
	styleItem->setIcon(0, QIcon(":/icons/css.png"));

	QTreeWidgetItem* iconsItem = new QTreeWidgetItem(resItem);
	iconsItem->setText(0, "Icons");
	iconsItem->setIcon(0, QIcon(":/icons/folder.png"));

	QTreeWidgetItem* readmeItem = new QTreeWidgetItem(docsItem);
	readmeItem->setText(0, "README.md");
	readmeItem->setIcon(0, QIcon(":/icons/markdown_file.png"));

	connect(m_treeWidget, &QTreeWidget::itemClicked,
		this, &MainWindow::onTreeItemClicked);
}

void MainWindow::setupDialogWidget()
{
	m_dialogWidget = new QWidget;
	m_dialogWidget->setObjectName("DialogWidget");

	QVBoxLayout* layout = new QVBoxLayout(m_dialogWidget);
	layout->setContentsMargins(12, 12, 12, 12);
	layout->setSpacing(8);

	// 添加标题
	QLabel* titleLabel = new QLabel("对话框");
	titleLabel->setObjectName("DialogTitle");
	titleLabel->setStyleSheet("font-size: 12pt; font-weight: bold; color: #00d4ff; margin-bottom: 10px;");
	layout->addWidget(titleLabel);

	// 添加一些示例控件
	QLabel* infoLabel = new QLabel("这里可以放置对话框内容");
	infoLabel->setStyleSheet("color: #e0e0e0; margin-bottom: 10px;");
	layout->addWidget(infoLabel);

	// 添加文本输入框
	QLineEdit* inputLine = new QLineEdit;
	inputLine->setPlaceholderText("输入内容...");
	inputLine->setStyleSheet(
		"QLineEdit {"
		"background: rgba(255, 255, 255, 0.1);"
		"border: 1px solid rgba(255, 255, 255, 0.2);"
		"border-radius: 6px;"
		"padding: 8px;"
		"color: #ffffff;"
		"font-size: 9pt;"
		"}"
		"QLineEdit:focus {"
		"border: 1px solid #00d4ff;"
		"background: rgba(0, 212, 255, 0.1);"
		"}"
	);
	layout->addWidget(inputLine);

	// 添加多行文本框
	QTextEdit* textEdit = new QTextEdit;
	textEdit->setPlaceholderText("多行文本输入...");
	textEdit->setMaximumHeight(120);
	textEdit->setStyleSheet(
		"QTextEdit {"
		"background: rgba(255, 255, 255, 0.08);"
		"border: 1px solid rgba(255, 255, 255, 0.2);"
		"border-radius: 6px;"
		"padding: 8px;"
		"color: #ffffff;"
		"font-size: 9pt;"
		"}"
		"QTextEdit:focus {"
		"border: 1px solid #00d4ff;"
		"background: rgba(0, 212, 255, 0.1);"
		"}"
	);
	layout->addWidget(textEdit);

	// 添加按钮
	QPushButton* submitBtn = new QPushButton("提交");
	submitBtn->setStyleSheet(
		"QPushButton {"
		"background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
		"stop:0 rgba(0, 212, 255, 0.2), stop:1 rgba(91, 99, 247, 0.2));"
		"border: 1px solid #00d4ff;"
		"border-radius: 6px;"
		"padding: 8px 16px;"
		"color: #00d4ff;"
		"font-weight: 500;"
		"}"
		"QPushButton:hover {"
		"background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
		"stop:0 rgba(0, 212, 255, 0.3), stop:1 rgba(91, 99, 247, 0.3));"
		"}"
	);
	layout->addWidget(submitBtn);

	layout->addStretch(); // 添加弹性空间
}

void MainWindow::setupBottomDockWidget()
{
	m_bottomDockWidget = new QDockWidget("数据表格", this);
	m_bottomDockWidget->setObjectName("TableDock");
	m_bottomDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);

	// 创建主容器
	QWidget* containerWidget = new QWidget;
	QHBoxLayout* containerLayout = new QHBoxLayout(containerWidget);
	containerLayout->setContentsMargins(8, 8, 8, 8);
	containerLayout->setSpacing(8);

	setupTableWidget();
	containerLayout->addWidget(m_tableWidget);

	// 创建右侧工具栏
	m_tableToolBar = new QToolBar;
	m_tableToolBar->setObjectName("TableToolBar");
	m_tableToolBar->setOrientation(Qt::Vertical);
	m_tableToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
	m_tableToolBar->setIconSize(QSize(16, 16));

	// 添加工具栏按钮
	QAction* addRowAction = m_tableToolBar->addAction(QIcon(), "添加行");  // 暂时移除图标引用
	QAction* deleteRowAction = m_tableToolBar->addAction(QIcon(), "删除行");
	m_tableToolBar->addSeparator();
	QAction* editAction = m_tableToolBar->addAction(QIcon(), "编辑");
	QAction* saveAction = m_tableToolBar->addAction(QIcon(), "保存");
	m_tableToolBar->addSeparator();
	QAction* refreshAction = m_tableToolBar->addAction(QIcon(), "刷新");
	/*
		// 设置表格样式
		m_tableWidget->setStyleSheet(
			"QTableWidget {"
			"background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
			"stop:0 rgba(255, 255, 255, 0.05), stop:1 rgba(255, 255, 255, 0.02));"
			"border: none;"
			"border-radius: 8px;"
			"color: #e0e0e0;"
			"font-size: 9pt;"
			"gridline-color: rgba(255, 255, 255, 0.08);"
			"selection-background-color: rgba(0, 212, 255, 0.2);"
			"alternate-background-color: rgba(255, 255, 255, 0.01);"
			"}"
			"QTableWidget::item {"
			"padding: 10px 8px;"
			"border: none;"
			"background: transparent;"
			"}"
			"QTableWidget::item:selected {"
			"background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
			"stop:0 rgba(0, 212, 255, 0.25), stop:1 rgba(91, 99, 247, 0.15));"
			"color: #ffffff;"
			"border: 1px solid rgba(0, 212, 255, 0.3);"
			"}"
			"QTableWidget::item:hover {"
			"background: rgba(0, 212, 255, 0.08);"
			"}"
			"QHeaderView::section {"
			"background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
			"stop:0 rgba(255, 255, 255, 0.08), stop:1 rgba(255, 255, 255, 0.04));"
			"color: #ffffff;"
			"padding: 12px 8px;"
			"border: none;"
			"border-right: 1px solid rgba(255, 255, 255, 0.06);"
			"border-bottom: 1px solid rgba(255, 255, 255, 0.08);"
			"font-weight: 600;"
			"font-size: 9pt;"
			"}"
			"QHeaderView::section:hover {"
			"background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
			"stop:0 rgba(0, 212, 255, 0.1), stop:1 rgba(91, 99, 247, 0.05));"
			"}"
		);

		// 设置工具栏样式
		m_tableToolBar->setStyleSheet(
			"QToolBar {"
			"background: transparent;"
			"border: none;"
			"spacing: 2px;"
			"}"
			"QToolBar QToolButton {"
			"background: rgba(255, 255, 255, 0.05);"
			"border: 1px solid rgba(255, 255, 255, 0.1);"
			"border-radius: 4px;"
			"padding: 4px;"
			"margin: 1px;"
			"color: #ffffff;"
			"}"
			"QToolBar QToolButton:hover {"
			"background: rgba(0, 212, 255, 0.2);"
			"border: 1px solid #00d4ff;"
			"color: #00d4ff;"
			"}"
		);
	*/
	containerLayout->addWidget(m_tableToolBar);

	m_bottomDockWidget->setWidget(containerWidget);
	addDockWidget(Qt::BottomDockWidgetArea, m_bottomDockWidget);

	// 关键修改：设置dock widget的布局限制
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}

void MainWindow::setupTableWidget()
{
	m_tableWidget = new QTableWidget;
	m_tableWidget->setObjectName("DataTable");

	// 设置表格拉伸模式，自动填充宽度
	m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	// 或者如果想要部分列固定宽度，可以这样设置：
	// m_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);  // ID列固定
	// m_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // 名称列拉伸
	// m_tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);   // 类型列固定
	// m_tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);   // 状态列固定
	// m_tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch); // 时间列拉伸

	// 设置行高自动调整
	m_tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	// 设置列数和标题
	m_tableWidget->setColumnCount(5);
	QStringList headers;
	headers << "ID" << "名称" << "类型" << "状态" << "创建时间";
	m_tableWidget->setHorizontalHeaderLabels(headers);

	// 设置选择模式为整行选择
	m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	// 添加示例数据
	m_tableWidget->setRowCount(8);

	// 修正的示例数据
	QString sampleData[8][5] = {
		{"001", "立方体模型", "3D对象", "活动", "2024-01-15"},
		{"002", "纹理贴图", "材质", "活动", "2024-01-16"},
		{"003", "光照设置", "环境", "禁用", "2024-01-17"},
		{"004", "相机视角", "视图", "活动", "2024-01-18"},
		{"005", "动画序列", "动作", "活动", "2024-01-19"},
		{"006", "粒子系统", "特效", "测试", "2024-01-20"},
		{"007", "声音文件", "音频", "活动", "2024-01-21"},
		{"008", "场景配置", "设置", "活动", "2024-01-22"}
	};

	for (int row = 0; row < 8; ++row) {
		for (int col = 0; col < 5; ++col) {
			QTableWidgetItem* item = new QTableWidgetItem(sampleData[row][col]);

			// 根据状态列设置不同颜色
			if (col == 3) { // 状态列
				if (sampleData[row][col] == "活动") {
					item->setData(Qt::ForegroundRole, QColor("#00ff88"));
				}
				else if (sampleData[row][col] == "禁用") {
					item->setData(Qt::ForegroundRole, QColor("#ff6b6b"));
				}
				else {
					item->setData(Qt::ForegroundRole, QColor("#ffd700"));
				}
			}

			m_tableWidget->setItem(row, col, item);
		}
	}

	// 其余代码保持不变...
}

void MainWindow::setupCentralTabWidget()
{
	// 创建tab widget
	m_centralTabWidget = new QTabWidget;
	m_centralTabWidget->setObjectName("CentralTabs");

	// 创建OpenGL widget
	m_openGLWidget = new OpenGLWidget;
	m_openGLWidget->setMinimumSize(400, 300);

	// 创建报告widget
	setupReportWidget();

	// 添加tabs
	m_centralTabWidget->addTab(m_openGLWidget, "3D视图");
	m_centralTabWidget->addTab(m_reportWidget, "报告");

	// 设置默认选中第一个tab
	m_centralTabWidget->setCurrentIndex(0);

	// 确保tab widget可见
	m_centralTabWidget->setVisible(true);
}

void MainWindow::setupOpenGLWidget()
{
	m_openGLWidget = new OpenGLWidget;
	m_openGLWidget->setMinimumSize(400, 300);
}

void MainWindow::setupReportWidget()
{
	m_reportWidget = new QWidget;
	m_reportWidget->setObjectName("ReportWidget");

	QVBoxLayout* reportLayout = new QVBoxLayout(m_reportWidget);
	reportLayout->setContentsMargins(16, 16, 16, 16);
	reportLayout->setSpacing(12);

	// 添加标题
	QLabel* titleLabel = new QLabel("项目分析报告");
	titleLabel->setObjectName("ReportTitle");
	titleLabel->setStyleSheet(
		"font-size: 18pt; font-weight: bold; color: #ffffff; margin-bottom: 16px;"
	);
	reportLayout->addWidget(titleLabel);

	// 添加统计信息区域
	QWidget* statsWidget = new QWidget;
	statsWidget->setObjectName("StatsWidget");
	QGridLayout* statsLayout = new QGridLayout(statsWidget);
	statsLayout->setSpacing(12);

	// 创建统计卡片
	auto createStatCard = [](const QString& title, const QString& value, const QString& color) {
		QWidget* card = new QWidget;
		card->setObjectName("StatCard");
		QVBoxLayout* cardLayout = new QVBoxLayout(card);
		cardLayout->setContentsMargins(12, 12, 12, 12);

		QLabel* valueLabel = new QLabel(value);
		valueLabel->setStyleSheet(QString("font-size: 24pt; font-weight: bold; color: %1;").arg(color));
		valueLabel->setAlignment(Qt::AlignCenter);

		QLabel* titleLabel = new QLabel(title);
		titleLabel->setStyleSheet("font-size: 10pt; color: #b0b0b0;");
		titleLabel->setAlignment(Qt::AlignCenter);

		cardLayout->addWidget(valueLabel);
		cardLayout->addWidget(titleLabel);

		card->setStyleSheet(
			"QWidget#StatCard {"
			"background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
			"stop:0 rgba(255, 255, 255, 0.1), stop:1 rgba(255, 255, 255, 0.05));"
			"border: 1px solid rgba(255, 255, 255, 0.15);"
			"border-radius: 8px;"
			"}"
		);

		return card;
	};

	statsLayout->addWidget(createStatCard("文件总数", "127", "#00d4ff"), 0, 0);
	statsLayout->addWidget(createStatCard("代码行数", "3,254", "#00ff88"), 0, 1);
	statsLayout->addWidget(createStatCard("函数数量", "89", "#ffd700"), 0, 2);
	statsLayout->addWidget(createStatCard("类数量", "23", "#ff6b6b"), 0, 3);

	reportLayout->addWidget(statsWidget);

	// 添加详细报告文本区域
	QTextEdit* reportText = new QTextEdit;
	reportText->setObjectName("ReportText");
	reportText->setReadOnly(true);
	reportText->setHtml(
		"<h3 style='color: #00d4ff;'>项目概览</h3>"
		"<p style='color: #e0e0e0;'>本项目是一个基于Qt6的现代化桌面应用程序，采用Fluent Design设计风格。</p>"
		"<br>"
		"<h3 style='color: #00d4ff;'>技术栈</h3>"
		"<ul style='color: #e0e0e0;'>"
		"<li>Qt 6.9.1 - 跨平台GUI框架</li>"
		"<li>OpenGL - 3D图形渲染</li>"
		"<li>CMake - 构建系统</li>"
		"<li>C++17 - 编程语言</li>"
		"</ul>"
		"<br>"
		"<h3 style='color: #00d4ff;'>架构分析</h3>"
		"<p style='color: #e0e0e0;'>应用程序采用模块化设计，包含以下主要组件：</p>"
		"<ul style='color: #e0e0e0;'>"
		"<li><strong>主窗口模块：</strong>负责整体布局和窗口管理</li>"
		"<li><strong>OpenGL模块：</strong>处理3D图形渲染</li>"
		"<li><strong>Ribbon界面：</strong>现代化的工具栏设计</li>"
		"<li><strong>数据表格：</strong>结构化数据展示</li>"
		"</ul>"
		"<br>"
		"<h3 style='color: #00d4ff;'>性能指标</h3>"
		"<p style='color: #e0e0e0;'>• 渲染帧率：60 FPS<br>"
		"• 内存占用：约 45MB<br>"
		"• 启动时间：1.2秒<br>"
		"• CPU占用：2-5%</p>"
	);

	reportText->setStyleSheet(
		"QTextEdit#ReportText {"
		"background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
		"stop:0 rgba(255, 255, 255, 0.05), stop:1 rgba(255, 255, 255, 0.02));"
		"border: 1px solid rgba(255, 255, 255, 0.1);"
		"border-radius: 8px;"
		"color: #e0e0e0;"
		"font-size: 9pt;"
		"padding: 12px;"
		"}"
	);

	reportLayout->addWidget(reportText, 1);  // 让文本区域占据剩余空间
}

void MainWindow::setupToolBar()
{
	m_rightToolBar = new QToolBar("Tools", this);
	m_rightToolBar->setObjectName("RightToolBar");
	m_rightToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
	m_rightToolBar->setOrientation(Qt::Vertical);

	// Add some tool actions
	QAction* rotateAction = m_rightToolBar->addAction(QIcon(":/icons/rotate.png"), "Rotate");
	QAction* zoomAction = m_rightToolBar->addAction(QIcon(":/icons/zoom.png"), "Zoom");
	QAction* resetAction = m_rightToolBar->addAction(QIcon(":/icons/reset.png"), "Reset");
	m_rightToolBar->addSeparator();
	QAction* wireframeAction = m_rightToolBar->addAction(QIcon(":/icons/wireframe.png"), "Wireframe");
	QAction* solidAction = m_rightToolBar->addAction(QIcon(":/icons/solid.png"), "Solid");

	// Connect actions to OpenGL widget
	connect(rotateAction, &QAction::triggered, m_openGLWidget, &OpenGLWidget::toggleRotation);
	connect(resetAction, &QAction::triggered, m_openGLWidget, &OpenGLWidget::resetCamera);
	connect(wireframeAction, &QAction::triggered, m_openGLWidget, &OpenGLWidget::setWireframeMode);
	connect(solidAction, &QAction::triggered, m_openGLWidget, &OpenGLWidget::setSolidMode);

	addToolBar(Qt::RightToolBarArea, m_rightToolBar);
}

void MainWindow::setupStatusBar()
{
	m_statusBar = statusBar();

	// Add status widgets
	QLabel* readyLabel = new QLabel("Ready");
	m_statusBar->addWidget(readyLabel);

	m_statusBar->addPermanentWidget(new QLabel("Vertices: 8"));
	m_statusBar->addPermanentWidget(new QLabel("Faces: 6"));

	QProgressBar* progressBar = new QProgressBar;
	progressBar->setVisible(false);
	progressBar->setMaximumWidth(200);
	m_statusBar->addPermanentWidget(progressBar);
}

void MainWindow::onTreeItemClicked(QTreeWidgetItem* item, int column)
{
	Q_UNUSED(column)
		QString itemText = item->text(0);
	statusBar()->showMessage(QString("Selected: %1").arg(itemText), 2000);
}

void MainWindow::onNewFile()
{
	statusBar()->showMessage("New file created", 2000);
}

void MainWindow::onOpenFile()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		"Open File", "", "All Files (*.*)");
	if (!fileName.isEmpty()) {
		statusBar()->showMessage(QString("Opened: %1").arg(fileName), 2000);
	}
}

void MainWindow::onSaveFile()
{
	statusBar()->showMessage("File saved", 2000);
}

void MainWindow::onExit()
{
	close();
}

void MainWindow::onAbout()
{
	QMessageBox::about(this, "About CAM3",
		"CAM3 v1.0.0\n\n"
		"A modern Qt application with Fluent Design style\n"
		"Features:\n"
		"• Ribbon interface with Backstage\n"
		"• 3D OpenGL rendering\n"
		"• Dockable panels\n"
		"• Modern styling");
}