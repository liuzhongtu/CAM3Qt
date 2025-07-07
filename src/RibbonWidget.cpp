#include "RibbonWidget.h"
#include <QApplication>
#include <QListWidget>
#include <QStackedWidget>
#include <QSplitter>
#include "ThemeManager.h"

RibbonWidget::RibbonWidget(QWidget* parent)
	: QWidget(parent)
	, m_fileButton(nullptr)
	, m_tabWidget(nullptr)
	, m_backstageWidget(nullptr)
	, m_backstageVisible(false)
{
	setObjectName("RibbonWidget");
	setFixedHeight(120);
	setupUI();
}

QWidget* RibbonWidget::createThemeGroup()
{
	QFrame* groupFrame = new QFrame;
	groupFrame->setObjectName("RibbonGroup");
	groupFrame->setFrameStyle(QFrame::StyledPanel);

	QVBoxLayout* groupLayout = new QVBoxLayout(groupFrame);
	groupLayout->setContentsMargins(5, 5, 5, 2);
	groupLayout->setSpacing(2);

	// 创建按钮布局
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->setSpacing(2);

	// 主题切换按钮
	QToolButton* darkBtn = createRibbonButton("深色", "dark_theme");
	QToolButton* lightBtn = createRibbonButton("浅色", "light_theme");

	// 连接信号
/*	connect(darkBtn, &QToolButton::clicked, [this]() {
		ThemeManager::instance()->setTheme(Theme::Dark);
	});
	connect(lightBtn, &QToolButton::clicked, [this]() {
		ThemeManager::instance()->setTheme(Theme::Light);
	});*/

    connect(lightBtn, &QPushButton::clicked, this, &RibbonWidget::lightThemeClicked);
    connect(darkBtn, &QPushButton::clicked, this, &RibbonWidget::darkThemeClicked);
    

	buttonsLayout->addWidget(darkBtn);
	buttonsLayout->addWidget(lightBtn);

	groupLayout->addLayout(buttonsLayout);

	// 添加组标题
	QLabel* titleLabel = new QLabel("主题");
	titleLabel->setObjectName("RibbonGroupTitle");
	titleLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setMaximumHeight(18);
	groupLayout->addWidget(titleLabel);

	return groupFrame;
}

void RibbonWidget::setupUI()
{
	m_mainLayout = new QHBoxLayout(this);
	m_mainLayout->setContentsMargins(0, 0, 0, 0);
	m_mainLayout->setSpacing(0);

	setupFileButton();
	setupRibbonTabs();
	setupBackstage();
}

void RibbonWidget::setupFileButton()
{
	m_fileButton = new QPushButton("File");
	m_fileButton->setObjectName("FileButton");
	m_fileButton->setFixedSize(60, 120);

	connect(m_fileButton, &QPushButton::clicked, this, &RibbonWidget::onFileButtonClicked);

	m_mainLayout->addWidget(m_fileButton);
}

void RibbonWidget::setupRibbonTabs()
{
	m_tabWidget = new QTabWidget;
	m_tabWidget->setObjectName("RibbonTabs");
	m_tabWidget->setTabPosition(QTabWidget::North);

	// Home tab
	QWidget* homeTab = new QWidget;
	QHBoxLayout* homeLayout = new QHBoxLayout(homeTab);
	homeLayout->setContentsMargins(5, 5, 5, 5);

	homeLayout->addWidget(createRibbonGroup("Clipboard", { "Paste", "Copy", "Cut" }));
	homeLayout->addWidget(createRibbonGroup("Font", { "Bold", "Italic", "Underline" }));
	homeLayout->addWidget(createRibbonGroup("Paragraph", { "Left", "Center", "Right" }));
	homeLayout->addStretch();

	m_tabWidget->addTab(homeTab, "Home");

	// Insert tab
	QWidget* insertTab = new QWidget;
	QHBoxLayout* insertLayout = new QHBoxLayout(insertTab);
	insertLayout->setContentsMargins(5, 5, 5, 5);

	insertLayout->addWidget(createRibbonGroup("Tables", { "Table", "Chart", "Graph" }));
	insertLayout->addWidget(createRibbonGroup("Illustrations", { "Picture", "Shape", "Icon" }));
	insertLayout->addStretch();

	m_tabWidget->addTab(insertTab, "Insert");

	// View tab
	QWidget* viewTab = new QWidget;
	QHBoxLayout* viewLayout = new QHBoxLayout(viewTab);
	viewLayout->setContentsMargins(5, 5, 5, 5);

	viewLayout->addWidget(createRibbonGroup("Show", { "Ruler", "Grid", "Guides" }));
	viewLayout->addWidget(createRibbonGroup("Zoom", { "Zoom In", "Zoom Out", "Fit" }));
	viewLayout->addWidget(createThemeGroup());
	viewLayout->addStretch();

	m_tabWidget->addTab(viewTab, "View");

	m_mainLayout->addWidget(m_tabWidget);
}

QWidget* RibbonWidget::createRibbonGroup(const QString& title, const QStringList& actions)
{
	QFrame* groupFrame = new QFrame;
	groupFrame->setObjectName("RibbonGroup");
	groupFrame->setFrameStyle(QFrame::StyledPanel);

	QVBoxLayout* groupLayout = new QVBoxLayout(groupFrame);
	groupLayout->setContentsMargins(5, 5, 5, 2);
	groupLayout->setSpacing(2);

	// Create buttons layout
	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->setSpacing(2);

	for (const QString& action : actions) {
		QToolButton* button = createRibbonButton(action, action.toLower().replace(" ", "_"));
		buttonsLayout->addWidget(button);
	}

	groupLayout->addLayout(buttonsLayout);

	// Add group title
	QLabel* titleLabel = new QLabel(title);
	titleLabel->setObjectName("RibbonGroupTitle");
	titleLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setMaximumHeight(18);
	groupLayout->addWidget(titleLabel);

	return groupFrame;
}

QToolButton* RibbonWidget::createRibbonButton(const QString& text, const QString& iconName, const QString& tooltip)
{
	QToolButton* button = new QToolButton;
	button->setText(text);
	button->setIcon(QIcon(QString(":/icons/%1.png").arg(iconName)));
	button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	button->setObjectName("RibbonButton");
	button->setFixedSize(60, 60);

	if (!tooltip.isEmpty()) {
		button->setToolTip(tooltip);
	}

	return button;
}

void RibbonWidget::setupBackstage()
{
	m_backstageWidget = new QWidget;
	m_backstageWidget->setObjectName("BackstageWidget");
	m_backstageWidget->setVisible(false);
	m_backstageWidget->setFixedHeight(400);

	QHBoxLayout* backstageLayout = new QHBoxLayout(m_backstageWidget);
	backstageLayout->setContentsMargins(0, 0, 0, 0);

	// Left panel with options
	QListWidget* optionsList = new QListWidget;
	optionsList->setObjectName("BackstageList");
	optionsList->setFixedWidth(200);
	optionsList->addItem("New");
	optionsList->addItem("Open");
	optionsList->addItem("Save");
	optionsList->addItem("Save As");
	optionsList->addItem("Print");
	optionsList->addItem("Export");
	optionsList->addItem("Options");
	optionsList->addItem("Exit");

	// Right panel with content
	QStackedWidget* contentStack = new QStackedWidget;
	contentStack->setObjectName("BackstageContent");

	// Create content pages
	QWidget* newPage = new QWidget;
	QVBoxLayout* newLayout = new QVBoxLayout(newPage);
	newLayout->addWidget(new QLabel("Create a new document"));

	QPushButton* newDocButton = new QPushButton("Blank Document");
	connect(newDocButton, &QPushButton::clicked, this, &RibbonWidget::newFileClicked);
	connect(newDocButton, &QPushButton::clicked, this, &RibbonWidget::hideBackstage);
	newLayout->addWidget(newDocButton);
	newLayout->addStretch();

	QWidget* openPage = new QWidget;
	QVBoxLayout* openLayout = new QVBoxLayout(openPage);
	openLayout->addWidget(new QLabel("Open a document"));

	QPushButton* openDocButton = new QPushButton("Browse Files");
	connect(openDocButton, &QPushButton::clicked, this, &RibbonWidget::openFileClicked);
	connect(openDocButton, &QPushButton::clicked, this, &RibbonWidget::hideBackstage);
	openLayout->addWidget(openDocButton);
	openLayout->addStretch();

	QWidget* savePage = new QWidget;
	QVBoxLayout* saveLayout = new QVBoxLayout(savePage);
	saveLayout->addWidget(new QLabel("Save the current document"));

	QPushButton* saveDocButton = new QPushButton("Save Document");
	connect(saveDocButton, &QPushButton::clicked, this, &RibbonWidget::saveFileClicked);
	connect(saveDocButton, &QPushButton::clicked, this, &RibbonWidget::hideBackstage);
	saveLayout->addWidget(saveDocButton);
	saveLayout->addStretch();

	QWidget* exitPage = new QWidget;
	QVBoxLayout* exitLayout = new QVBoxLayout(exitPage);
	exitLayout->addWidget(new QLabel("Exit the application"));

	QPushButton* exitButton = new QPushButton("Exit Application");
	connect(exitButton, &QPushButton::clicked, this, &RibbonWidget::exitClicked);
	exitLayout->addWidget(exitButton);
	exitLayout->addStretch();

	contentStack->addWidget(newPage);    // 0
	contentStack->addWidget(openPage);   // 1
	contentStack->addWidget(savePage);   // 2
	contentStack->addWidget(new QWidget); // 3 - Save As
	contentStack->addWidget(new QWidget); // 4 - Print
	contentStack->addWidget(new QWidget); // 5 - Export
	contentStack->addWidget(new QWidget); // 6 - Options
	contentStack->addWidget(exitPage);   // 7 - Exit

	// Connect list selection to stack
	connect(optionsList, &QListWidget::currentRowChanged,
		contentStack, &QStackedWidget::setCurrentIndex);

	backstageLayout->addWidget(optionsList);
	backstageLayout->addWidget(contentStack);

	// Initially select "New"
	optionsList->setCurrentRow(0);
}

void RibbonWidget::onFileButtonClicked()
{
	if (m_backstageVisible) {
		hideBackstage();
	}
	else {
		showBackstage();
	}
}

void RibbonWidget::showBackstage()
{
	if (!m_backstageVisible) {
		m_backstageVisible = true;
		m_tabWidget->setVisible(false);
		m_backstageWidget->setVisible(true);
		m_mainLayout->addWidget(m_backstageWidget);

		// Resize parent window if needed
		if (QWidget* topLevel = window()) {
			topLevel->resize(topLevel->width(), topLevel->height() + 400);
		}
	}
}

void RibbonWidget::hideBackstage()
{
	if (m_backstageVisible) {
		m_backstageVisible = false;
		m_backstageWidget->setVisible(false);
		m_mainLayout->removeWidget(m_backstageWidget);
		m_tabWidget->setVisible(true);

		// Resize parent window back
		if (QWidget* topLevel = window()) {
			topLevel->resize(topLevel->width(), topLevel->height() - 400);
		}
	}
}