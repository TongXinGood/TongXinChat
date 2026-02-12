#include "NoFrame.h"

NoFrame::Frameconfig::Frameconfig()
	:background(255,255,255)
	,borderRadius(10)
	,defaultsize(800,600)
	,titlebarheight(30)
	,titlebarcolor(255,255,255)
	,showlogo(false)
	,showclose(true)
	,showmin(true)
	,showmax(false)
	,showtext(false)
	,logoimage(":/Resource/icon/logo.png")
	,minicon(":/Resource/icon/min.png")
	,maxicon(":/Resource/icon/max.png")
	,closeicon(":/Resource/icon/close.png")
	,logosize(20,20)
	,btnsize(16,16)
{}

NoFrame::NoFrame(QWidget* parent) :FramelessWindow(parent) ,ismax(false){
	//防止野指针
	centralContainer = nullptr;
	mainlayout = nullptr;
	titleBar = nullptr;
	titleLayout = nullptr;
	lbLogo = nullptr;
	lbTitle = nullptr;
	btnMin = nullptr;
	btnMax = nullptr;
	btnClose = nullptr;
	contentArea = nullptr;
	//初始化`
	initUi();
	setFrameconfig(config);
}

NoFrame::~NoFrame(){
}

void NoFrame::initUi() {
	centralContainer = new QWidget(this);
	centralContainer->setObjectName("centralContainer");
	this->setCentralWidget(centralContainer);
	mainlayout = new QVBoxLayout(centralContainer);
	mainlayout->setContentsMargins(0, 0, 0, 0);
	mainlayout->setSpacing(0);
	titleBar = new QWidget(this);
	titleBar->setObjectName("titleBar");
	titleLayout = new QHBoxLayout(titleBar);
	titleLayout->setContentsMargins(10, 0, 0, 0);
	titleLayout->setSpacing(10);
	lbLogo = new QLabel(titleBar);
	lbLogo->setObjectName("lbLogo");
	lbTitle = new QLabel(titleBar);
	lbTitle->setObjectName("lbTitle");
	btnMin = new QPushButton(titleBar);
	btnMin->setObjectName("btnMin");
	btnMax = new QPushButton(titleBar);
	btnMax->setObjectName("btnMax");
	btnClose = new QPushButton(titleBar);
	btnClose->setObjectName("btnClose");

	titleLayout->addWidget(lbLogo);
	titleLayout->addWidget(lbTitle);
	titleLayout->addStretch(1);
	titleLayout->addWidget(btnMin);
	titleLayout->addWidget(btnMax);
	titleLayout->addWidget(btnClose);

	contentArea = new QWidget(this);
	contentArea->setObjectName("contentArea");

	mainlayout->addWidget(titleBar);
	mainlayout->addWidget(contentArea, 1);

	this->setTitleBar(titleBar);

	connect(btnMin, &QPushButton::clicked, this, &NoFrame::onbtnminClick);
	connect(btnMax, &QPushButton::clicked, this, &NoFrame::onbtnmaxClick);
	connect(btnClose, &QPushButton::clicked, this, &NoFrame::onbtncloseClick);
}

void NoFrame::setFrameconfig(const NoFrame::Frameconfig& config)
{
	this->config = config;
	this->resize(config.defaultsize);
	titleBar->setFixedHeight(config.titlebarheight);
	lbLogo->setVisible(config.showlogo);
	lbTitle->setVisible(config.showtext);
	btnMin->setVisible(config.showmin);    
	btnMax->setVisible(config.showmax);     
	btnClose->setVisible(config.showclose);
	updateStyle();    
	changeIconsize();
}

void NoFrame::updateStyle()
{
	QString titleColorStr = QString("rgb(%1, %2, %3)")
		.arg(config.titlebarcolor.red())
		.arg(config.titlebarcolor.green())
		.arg(config.titlebarcolor.blue());
	QString bgColorStr = QString("rgb(%1, %2, %3)")
		.arg(config.background.red())
		.arg(config.background.green())  
		.arg(config.background.blue());


	QString qss = QString(
		"#titleBar { "
		"   background-color: %1; "
		"   border-top-left-radius: %3px; "   
		"   border-top-right-radius: %3px; " 
		"} "

		// 窗口中心背景样式 
		"#centralContainer { "
		"   background-color: %2; "
		"   border-radius: %3px; "            // 四周圆角
		"} "

		//按钮基础样式
		"QPushButton { "
		"   border: none; "
		"   background: transparent; "
		"} "


		"#btnMin:hover, #btnMax:hover { "
		"   background-color: rgba(0, 0, 0, 30); "
		"} "

		"#btnClose:hover { "
		"   background-color: rgb(232, 17, 35); " 
		"} "
	)
		.arg(titleColorStr)  
		.arg(bgColorStr)
		.arg(config.borderRadius); 

	this->setStyleSheet(qss);
}

void NoFrame::changeIconsize()
{
	btnMin->setIconSize(config.btnsize);       // 设置显示大小
	btnMin->setIcon(QIcon(config.minicon));    // 加载图片
	btnMax->setIconSize(config.btnsize);
	btnMax->setIcon(QIcon(config.maxicon));
	btnClose->setIconSize(config.btnsize);
	btnClose->setIcon(QIcon(config.closeicon));
	lbLogo->setFixedSize(config.logosize);
	QPixmap logoMap(config.logoimage);
	lbLogo->setPixmap(logoMap.scaled(config.logosize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

}

void NoFrame::setMainWindow(QWidget *widget)
{
	QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(contentArea->layout());
	if (layout == nullptr) {
		layout = new QVBoxLayout(contentArea);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(0);
	}
	layout->addWidget(widget);
}

void NoFrame::setWindowTitle(const QString& title)
{
	lbTitle->setText(title);
	FramelessWindow::setWindowTitle(title);
}

void NoFrame::onbtncloseClick()
{
	this->close();
}

void NoFrame::onbtnminClick() 
{
	this->showMinimized();
}

void NoFrame::onbtnmaxClick()
{
	if (this->isMaximized()) {
		this->showNormal();
		btnMax->setIcon(QIcon(config.maxicon));

	}
	else {
		this->showMaximized();
	}
}