#ifndef USERWINDOW_H
#define USERWINDOW_H

#include "Data.h"
#include "ShadowWidget.h"
#include <QMainWindow>
#include <map>
class LoginDialog;
namespace Ui {
    class UserWindow;
}

class UserWindow : public ShadowWidget<QMainWindow> {
    Q_OBJECT

public:
    explicit UserWindow(QWidget* parent = nullptr);
    ~UserWindow();

    void setSlot(LoginDialog* dialog);

    void closeEvent(QCloseEvent* event) override;

private:
    Ui::UserWindow* ui;
    UserModel*      userModel;
    User*           user = nullptr;
};

#endif  // USERWINDOW_H
