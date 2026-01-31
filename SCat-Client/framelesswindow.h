#ifndef FRAMELESSWINDOW_H  // 宏定义也顺便改了，更规范
#define FRAMELESSWINDOW_H

#include <QObject>
#include <QMainWindow>
#include <QList>
#include <QMargins>
#include <QRect>

#ifdef Q_OS_WIN
#include <QWidget>

class FramelessWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit FramelessWindow(QWidget* parent = 0);

public:
    // 设置是否可以通过鼠标调整窗口大小
    void setResizeable(bool resizeable = true);
    bool isResizeable() { return m_bResizeable; }

    // 设置可调整大小区域的宽度
    void setResizeableAreaWidth(int width = 5);

protected:
    // 设置标题栏 widget
    void setTitleBar(QWidget* titlebar);

    // 添加白名单 widget (比如标题栏上的按钮)，避免拖拽冲突
    void addIgnoreWidget(QWidget* widget);

    // nativeEvent 用于处理 Windows 消息
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

private slots:
    void onTitleBarDestroyed();

public:
    void setContentsMargins(const QMargins& margins);
    void setContentsMargins(int left, int top, int right, int bottom);
    QMargins contentsMargins() const;
    QRect contentsRect() const;
    void getContentsMargins(int* left, int* top, int* right, int* bottom) const;

public slots:
    void showFullScreen();

private:
    QWidget* m_titlebar;
    QList<QWidget*> m_whiteList;
    int m_borderWidth;

    QMargins m_margins;
    QMargins m_frames;
    bool m_bJustMaximized;

    bool m_bResizeable;
};

#endif // Q_OS_WIN
#endif // FRAMELESSWINDOW_H