#include "UserInfoDialog.h"
#include "TitleBar.h"
#include "qdebug.h"
#include "ui_UserInfoDialog.h"
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QValidator>
#include "Utils.h"

UserInfoDialog::UserInfoDialog(UserModel* model, QWidget* parent) : ShadowWidget(parent), ui(new Ui::UserInfoDialog) {
    ui->setupUi(this);
    setResizable(false);
    setMovable(false);
    title()->setIconColor(QColor(0x2, 0x2, 0x2), TitleBar::Close, ":/xicons/close.svg");
    title()->setButtonSize(30, 30);
    title()->setIconSize(18, 18);
    title()->setButtons(TitleBar::Close);
    init(ui->frameLayout);
    ui->frameLayout->setSpacing(10);
    ui->frameLayout->setContentsMargins(10, 0, 10, 10);
    ui->idLineEdit->setValidator(new QIntValidator());

    connect(ui->okButton, &QPushButton::clicked, this, [=] {
        User user;

        CHECK_STR_EMPTY(idLineEdit->text(), user.id);
        for (auto& it : model->getModelData()) {
            if (it.id == user.id) {
                QMessageBox::critical(this, windowTitle(), "注册账号重复");
                return;
            }
        }
        CHECK_STR_EMPTY(nameLineEdit->text(), user.name);
        CHECK_STR_EMPTY(passwordLineEdit->text(), user.password);
        CHECK_STR_EMPTY(addressPlainTextEdit->toPlainText(), user.address);
        emit confirmData(user);
        accept();
    });
}

UserInfoDialog::~UserInfoDialog() { delete ui; }

void UserInfoDialog::setTitle(const QString& title) { ui->titleLabel->setText(title); }

QLayout* UserInfoDialog::getContentLayout() const { return ui->layout; }
