#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include "../../NoFrame.h"
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QString>


class RegisterWindow : public NoFrame
{
	Q_OBJECT
public:
	explicit RegisterWindow(QWidget* parent = nullptr);
	~RegisterWindow();
signals:
	void sendRegisterClicked(const QString& account, const QString& pwd);
	void sendReturnClicked();
private slots:
	void onRegisterClicked();
	void onReturnClicked();
private:
	void initUi();
	void initconnect();
private:
	QLabel* Logo;
	QLabel* LogoText;

	QLabel* texteg1;
	QLabel* texteg2;
	
	QLineEdit* account;
	QLineEdit* pwd;

	QPushButton* btnregister;
	QPushButton* btnreturn;

	QVBoxLayout* mainLayout;
};


#endif // REGISTERWINDOW_H
