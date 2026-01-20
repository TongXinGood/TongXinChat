#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv); // 1. 初始化 Qt 应用

    QWidget window;
    window.setWindowTitle("TX-Messenger Client");
    window.resize(400, 300);

    QVBoxLayout* layout = new QVBoxLayout(&window);

    QPushButton* btn = new QPushButton("点击测试 Qt 环境", &window);
    layout->addWidget(btn);

    QObject::connect(btn, &QPushButton::clicked, [&]() {
        QMessageBox::information(&window, "成功", "VS2022 + CMake + Qt6 环境配置满分通过！");
        });

    window.show(); // 2. 显示窗口

    return app.exec(); // 3. 进入事件循环
}