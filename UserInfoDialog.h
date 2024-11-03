#ifndef USERINFODIALOG_H
#define USERINFODIALOG_H

#include "Data.h"
#include "ShadowWidget.h"
#include <QDialog>

namespace Ui {
    class UserInfoDialog;
}  // namespace Ui

class UserInfoDialog : public ShadowWidget<QDialog> {
    Q_OBJECT

public:
    explicit UserInfoDialog(UserModel* model, QWidget* parent = nullptr);
    UserInfoDialog(const QModelIndex& index, QWidget* parent = nullptr);
    ~UserInfoDialog();

    void     setTitle(const QString& title);
    QLayout* getContentLayout() const;

Q_SIGNALS:
    void confirmData(const User&);

private:
    Ui::UserInfoDialog* ui;
};

#endif  // USERINFODIALOG_H
