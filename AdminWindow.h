#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui { class AdminWindow; }
QT_END_NAMESPACE
class LoginDialog;
class UserModel;

class AdminWindow : public QMainWindow
{
  Q_OBJECT

public:
  AdminWindow(QWidget *parent = nullptr);
  void setSlot(LoginDialog* dialog);
  ~AdminWindow();

private:
  Ui::AdminWindow *ui;

  std::map<QString, QString> users;
  UserModel* userModel;

  // QWidget interface
protected:
  void closeEvent(QCloseEvent *event) override;
};
#endif // ADMINWINDOW_H
