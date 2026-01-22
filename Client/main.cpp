#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // 1. 主窗口
    QWidget* window = new QWidget();
    window->setWindowTitle("TongXinChat");
    window->setFixedSize(400, 500);
    window->setStyleSheet("background-color: #EBF7FF;border-radius: 15px;");


    // 2. 登录按钮 (蓝色那个)
    QPushButton* bt1 = new QPushButton("登录");
    bt1->setFixedHeight(45); 
    bt1->setStyleSheet("QPushButton { background-color: #0099FF; color: white; border-radius: 5px; font-weight: bold; font-size: 16px; }");

    // 3. 注册按钮 
    QPushButton* bt2 = new QPushButton("注册账号");
    bt2->setFixedHeight(30); 
    bt2->setFlat(true);
    bt2->setStyleSheet("color: #0099FF; font-size: 14px;");

    // 4. 布局设计
    QVBoxLayout* v1 = new QVBoxLayout();


    v1->addStretch();

    v1->addWidget(bt1);

    v1->addSpacing(10);

    v1->addWidget(bt2);


    v1->setContentsMargins(40, 0, 40, 30);
 

    window->setLayout(v1);
    window->show();

    return app.exec();
}