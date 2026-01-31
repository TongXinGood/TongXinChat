#include "framelesswindow.h"
#include <QApplication>
#include <QPoint>
#include <QSize>

#ifdef Q_OS_WIN

#include <windows.h>
#include <WinUser.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <objidl.h> 
#include <gdiplus.h>
#include <GdiPlusColor.h>

#pragma comment (lib,"Dwmapi.lib") 
#pragma comment (lib,"user32.lib")

FramelessWindow::FramelessWindow(QWidget* parent)
    : QMainWindow(parent),
    m_titlebar(Q_NULLPTR),
    m_borderWidth(5),
    m_bJustMaximized(false),
    m_bResizeable(true)
{
    setWindowFlags(windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setResizeable(m_bResizeable);
}

void FramelessWindow::setResizeable(bool resizeable)
{
    bool visible = isVisible();
    m_bResizeable = resizeable;
    if (m_bResizeable) {
        setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
        HWND hwnd = (HWND)this->winId();
        DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
        ::SetWindowLong(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);
    }
    else {
        setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
        HWND hwnd = (HWND)this->winId();
        DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
        ::SetWindowLong(hwnd, GWL_STYLE, style & ~WS_MAXIMIZEBOX & ~WS_CAPTION);
    }

    const MARGINS shadow = { 1, 1, 1, 1 };
    DwmExtendFrameIntoClientArea(HWND(winId()), &shadow);

    setVisible(visible);
}

void FramelessWindow::setResizeableAreaWidth(int width)
{
    if (1 > width) width = 1;
    m_borderWidth = width;
}

void FramelessWindow::setTitleBar(QWidget* titlebar)
{
    m_titlebar = titlebar;
    if (!titlebar) return;
    connect(titlebar, SIGNAL(destroyed(QObject*)), this, SLOT(onTitleBarDestroyed()));
}

void FramelessWindow::onTitleBarDestroyed()
{
    if (m_titlebar == QObject::sender())
    {
        m_titlebar = Q_NULLPTR;
    }
}

void FramelessWindow::addIgnoreWidget(QWidget* widget)
{
    if (!widget) return;
    if (m_whiteList.contains(widget)) return;
    m_whiteList.append(widget);
}

bool FramelessWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);

    switch (msg->message)
    {
    case WM_NCCALCSIZE:
    {
        NCCALCSIZE_PARAMS& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
        if (params.rgrc[0].top != 0)
            params.rgrc[0].top -= 1;
        *result = WVR_REDRAW;
        return true;
    }
    case WM_NCHITTEST:
    {
        *result = 0;
        const LONG border_width = m_borderWidth;
        RECT winrect;
        GetWindowRect(HWND(winId()), &winrect);

        long x = GET_X_LPARAM(msg->lParam);
        long y = GET_Y_LPARAM(msg->lParam);

        if (m_bResizeable)
        {
            bool resizeWidth = minimumWidth() != maximumWidth();
            bool resizeHeight = minimumHeight() != maximumHeight();

            if (resizeWidth)
            {
                if (x >= winrect.left && x < winrect.left + border_width) *result = HTLEFT;
                if (x < winrect.right && x >= winrect.right - border_width) *result = HTRIGHT;
            }
            if (resizeHeight)
            {
                if (y < winrect.bottom && y >= winrect.bottom - border_width) *result = HTBOTTOM;
                if (y >= winrect.top && y < winrect.top + border_width) *result = HTTOP;
            }
            if (resizeWidth && resizeHeight)
            {
                if (x >= winrect.left && x < winrect.left + border_width && y < winrect.bottom && y >= winrect.bottom - border_width) *result = HTBOTTOMLEFT;
                if (x < winrect.right && x >= winrect.right - border_width && y < winrect.bottom && y >= winrect.bottom - border_width) *result = HTBOTTOMRIGHT;
                if (x >= winrect.left && x < winrect.left + border_width && y >= winrect.top && y < winrect.top + border_width) *result = HTTOPLEFT;
                if (x < winrect.right && x >= winrect.right - border_width && y >= winrect.top && y < winrect.top + border_width) *result = HTTOPRIGHT;
            }
        }
        if (0 != *result) return true;

        if (!m_titlebar) return false;

        double dpr = this->devicePixelRatioF();
        QPoint pos = m_titlebar->mapFromGlobal(QPoint(x / dpr, y / dpr));

        if (!m_titlebar->rect().contains(pos)) return false;
        QWidget* child = m_titlebar->childAt(pos);
        if (!child)
        {
            *result = HTCAPTION;
            return true;
        }
        else {
            if (m_whiteList.contains(child))
            {
                *result = HTCAPTION;
                return true;
            }
        }
        return false;
    }
    case WM_GETMINMAXINFO:
    {
        if (::IsZoomed(msg->hwnd)) {
            RECT frame = { 0, 0, 0, 0 };
            AdjustWindowRectEx(&frame, WS_OVERLAPPEDWINDOW, FALSE, 0);

            double dpr = this->devicePixelRatioF();
            m_frames.setLeft(abs(frame.left) / dpr + 0.5);
            m_frames.setTop(abs(frame.bottom) / dpr + 0.5);
            m_frames.setRight(abs(frame.right) / dpr + 0.5);
            m_frames.setBottom(abs(frame.bottom) / dpr + 0.5);

            QMainWindow::setContentsMargins(m_frames.left() + m_margins.left(), \
                m_frames.top() + m_margins.top(), \
                m_frames.right() + m_margins.right(), \
                m_frames.bottom() + m_margins.bottom());
            m_bJustMaximized = true;
        }
        else {
            if (m_bJustMaximized)
            {
                QMainWindow::setContentsMargins(m_margins);
                m_frames = QMargins();
                m_bJustMaximized = false;
            }
        }
        return false;
    }
    default:
        return QMainWindow::nativeEvent(eventType, message, result);
    }
}

void FramelessWindow::setContentsMargins(const QMargins& margins)
{
    QMainWindow::setContentsMargins(margins + m_frames);
    m_margins = margins;
}
void FramelessWindow::setContentsMargins(int left, int top, int right, int bottom)
{
    QMainWindow::setContentsMargins(left + m_frames.left(), \
        top + m_frames.top(), \
        right + m_frames.right(), \
        bottom + m_frames.bottom());
    m_margins.setLeft(left);
    m_margins.setTop(top);
    m_margins.setRight(right);
    m_margins.setBottom(bottom);
}
QMargins FramelessWindow::contentsMargins() const
{
    QMargins margins = QMainWindow::contentsMargins();
    margins -= m_frames;
    return margins;
}

void FramelessWindow::getContentsMargins(int* left, int* top, int* right, int* bottom) const
{
    QMargins m = QMainWindow::contentsMargins();
    if (left) *left = m.left();
    if (top) *top = m.top();
    if (right) *right = m.right();
    if (bottom) *bottom = m.bottom();

    if (!(left && top && right && bottom)) return;
    if (isMaximized())
    {
        *left -= m_frames.left();
        *top -= m_frames.top();
        *right -= m_frames.right();
        *bottom -= m_frames.bottom();
    }
}

QRect FramelessWindow::contentsRect() const
{
    QRect rect = QMainWindow::contentsRect();
    int width = rect.width();
    int height = rect.height();
    rect.setLeft(rect.left() - m_frames.left());
    rect.setTop(rect.top() - m_frames.top());
    rect.setWidth(width);
    rect.setHeight(height);
    return rect;
}
void FramelessWindow::showFullScreen()
{
    if (isMaximized())
    {
        QMainWindow::setContentsMargins(m_margins);
        m_frames = QMargins();
    }
    QMainWindow::showFullScreen();
}

#endif //Q_OS_WIN