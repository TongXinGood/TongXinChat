#ifndef FRIENDLISTITEM_H
#define FRIENDLISTITEM_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>

class FriendListItem : public QWidget
{
    Q_OBJECT
public:
    explicit FriendListItem(const QString& id, const QString& avatarPath, const QString& name, const QString& lastMsg, QWidget* parent = nullptr);
    ~FriendListItem();

    QString getFriendId() const;
    QString getFriendName() const;
    void setLastMessage(const QString& msg);

private:
    void initUI(const QString& avatarPath, const QString& name, const QString& lastMsg);

private:
    QString friendId;
    QString friendName;

    QLabel* lbAvatar;
    QLabel* lbName;
    QLabel* lbLastMsg;

    QHBoxLayout* mainLayout;
    QVBoxLayout* textLayout;
};

#endif // FRIENDLISTITEM_H