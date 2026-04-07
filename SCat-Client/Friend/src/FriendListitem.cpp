#include "../include/FriendListitem.h"
#include <QPixmap>

FriendListItem::FriendListItem(const QString& id, const QString& avatarPath, const QString& name, const QString& lastMsg, QWidget* parent)
    : QWidget(parent), friendId(id), friendName(name)
{
    initUI(avatarPath, name, lastMsg);
}

FriendListItem::~FriendListItem()
{
}

void FriendListItem::initUI(const QString& avatarPath, const QString& name, const QString& lastMsg)
{
    // ======== 核心修复 2：将控件自身的背景彻底设为透明 ========
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("QWidget { background-color: transparent; }");

    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(15, 10, 15, 10);
    mainLayout->setSpacing(15);

    lbAvatar = new QLabel(this);
    lbAvatar->setFixedSize(45, 45);
    QPixmap pixmap(avatarPath);
    lbAvatar->setPixmap(pixmap.scaled(45, 45, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    lbAvatar->setStyleSheet("border-radius: 22px; background-color: #E0E0E0;");

    textLayout = new QVBoxLayout();
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(4);

    lbName = new QLabel(name, this);
    // 追加 background-color: transparent 防止文本控件遮挡父级选中颜色
    lbName->setStyleSheet("font-weight: bold; font-size: 15px; color: #1A1A1A; background-color: transparent;");

    lbLastMsg = new QLabel(lastMsg, this);
    // 追加 background-color: transparent
    lbLastMsg->setStyleSheet("font-size: 13px; color: #757575; background-color: transparent;");

    textLayout->addWidget(lbName);
    textLayout->addWidget(lbLastMsg);
    textLayout->addStretch(1);

    mainLayout->addWidget(lbAvatar);
    mainLayout->addLayout(textLayout);
}

QString FriendListItem::getFriendId() const
{
    return friendId;
}

QString FriendListItem::getFriendName() const
{
    return friendName;
}

void FriendListItem::setLastMessage(const QString& msg)
{
    lbLastMsg->setText(msg);
}