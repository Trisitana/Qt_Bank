#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "ShadowWidget.h"
#include <QDialog>
#include <map>

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public ShadowWidget<QDialog> {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog();
Q_SIGNALS:
  void userLoggedIn(const QString&);
  void adminLoggedIn(const QString&);
protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::LoginDialog*                     ui;
    std::map<QString, QString> users, admins;

    void openFile(QFile& file, const QString& errMsg, std::map<QString, QString>& des);
    bool checkPassword(const std::map<QString, QString>& dataSet, const QString& acc, const QString& pwd);
};

#endif  // LOGINDIALOG_H
