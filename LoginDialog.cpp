#include "LoginDialog.h"
#include "TitleBar.h"
#include "Utils.h"
#include "qgraphicseffect.h"
#include "ui_LoginDialog.h"
#include <QFile>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QTextStream>

LoginDialog::LoginDialog(QWidget* parent) : ShadowWidget(parent), ui(new Ui::LoginDialog) {
    ui->setupUi(this);
    setBackgroundColor(Qt::white);
    title()->setButtonSize(30, 30);
    title()->setIconSize(18, 18);
    title()->setButtons(TitleBar::Close | TitleBar::Minimize);
    setResizable(false);
    init(ui->mainLayout);
    QFile user("users.txt"), admin(":/admin.txt");
    openFile(user, "储户数据文件打开失败", users);
    openFile(admin, "管理员数据文件打开失败", admins);
    auto* shadow = new QGraphicsDropShadowEffect();
    shadow->setOffset(0, 0);
    shadow->setBlurRadius(6);
    shadow->setColor(QColor(0x0C, 0xBF, 0xFD));
    ui->loginButton->setGraphicsEffect(shadow);
    connect(ui->loginButton, &QPushButton::clicked, this, [=] {
        if (ui->radioUser->isChecked()) {
            if (checkPassword(users, ui->userLineEdit->text(), ui->passwordLineEdit->text())) {
                setResult(1);
                emit userLoggedIn(ui->userLineEdit->text());
                accept();
            }
        }
        else {
            if (checkPassword(admins, ui->userLineEdit->text(), ui->passwordLineEdit->text())) {
                setResult(2);
                emit adminLoggedIn(ui->userLineEdit->text());
                accept();
            }
        }
    });
}

LoginDialog::~LoginDialog() { delete ui; }

void LoginDialog::paintEvent(QPaintEvent* event) {
    ShadowWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QRect rect = this->rect();
    rect.setHeight(125);
    QLinearGradient grad(0, 30, 0, 200);
    grad.setColorAt(0.0, QColor(0x0f, 0xc3, 0xff));
    grad.setColorAt(0.5, QColor(0x07, 0xbc, 0xfc));
    grad.setColorAt(1, QColor(0x00, 0xb5, 0xf9));
    painter.fillRect(rect, QBrush(grad));
}

void LoginDialog::openFile(QFile& file, const QString& errMsg, std::map<QString, QString>& des) {
    if (!file.exists()) {
        file.open(QIODevice::WriteOnly);
    }
    else {
        while (true) {
            if (!file.open(QIODevice::ReadOnly)) {
                auto res = QMessageBox::critical(this, windowTitle(), errMsg, QMessageBox::Retry, QMessageBox::Abort);
                if (res == QMessageBox::Abort)
                    exit(0);
            }
            else
                break;
        }
        QTextStream ts(&file);
        ts.skipWhiteSpace();
        while (!ts.atEnd()) {
            QString acc, pwd;
            ts >> acc >> pwd;
            if (!acc.isEmpty() && !pwd.isEmpty())
                des[acc] = pwd;
        }
    }
    file.close();
}

bool LoginDialog::checkPassword(const std::map<QString, QString>& dataSet, const QString& acc, const QString& pwd) {
    auto res = dataSet.find(acc);
    if (res == dataSet.end() || res->second != pwd) {
        QMessageBox::critical(this, windowTitle(), "用户密码错误或账户不存在");
        return false;
    }
    return true;
}
