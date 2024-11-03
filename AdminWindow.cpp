#include "AdminWindow.h"
#include "Data.h"
#include "Delegates.h"
#include "LoginDialog.h"
#include "SearchDialog.h"
#include "TransactionDialog.h"
#include "UserInfoDialog.h"
#include "Utils.h"
#include "qdebug.h"
#include "ui_AdminWindow.h"
#include <QDataStream>
#include <QFile>
#include <QHeaderView>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QTableView>
#include <QTextStream>
#include <QToolBar>
#include <QToolButton>

AdminWindow::AdminWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::AdminWindow) {
    ui->setupUi(this);

    userModel   = new UserModel();
    auto* proxy = new QSortFilterProxyModel();
    proxy->setSourceModel(userModel);

    connect(ui->userTable, &QTableView::clicked, this, [=] { ui->infoTable->setModel(&userModel->getModelData()[ui->userTable->currentIndex().row()].records); });
    ui->userTable->setModel(proxy);
    ui->splitter->setStretchFactor(0, 5);
    ui->splitter->setStretchFactor(1, 3);

    // actions

    ui->addUserAction->setIcon(QIcon(":/icons/add.png"));
    ui->removeUserAction->setIcon(QIcon(":/icons/ashbin.png"));
    ui->inquiryAction->setIcon(QIcon(":/icons/search.svg"));
    connect(ui->infoAction, &QAction::triggered, this, [=] {
        QMessageBox msgBox;
        msgBox.setText("营业员工号为 " + employeeId);
        msgBox.exec();
    });
    connect(ui->addUserAction, &QAction::triggered, this, [=] {
        UserInfoDialog dialog(userModel);
        dialog.setTitle("开户注册");
        connect(&dialog, &UserInfoDialog::confirmData, this, [=](const User& user) {
            userModel->insertRow(userModel->rowCount());
            userModel->getModelData().back() = user;
        });
        dialog.exec();
    });
    connect(ui->addUserAction, &QAction::triggered, this, [=] {});
    connect(ui->removeUserAction, &QAction::triggered, this, [=] {
        if (ui->userTable->currentIndex().isValid()) {
            if (QMessageBox::question(this, windowTitle(), "您确认要注销此账户吗") == QMessageBox::Yes) {
                if (std::addressof(userModel->getModelData()[ui->userTable->currentIndex().row()].records) == ui->infoTable->model())
                    ui->infoTable->setModel(nullptr);
                userModel->removeRows(ui->userTable->currentIndex().row(), 1, QModelIndex());
            }
        }
    });
    connect(ui->inquiryAction, &QAction::triggered, this, [=] { SearchDialog(proxy, this).exec(); });
    connect(ui->exitAction, &QAction::triggered, this, [=] { close(); });
    connect(ui->transactionAction, &QAction::triggered, this, [=] {
        if (auto index = ui->userTable->currentIndex(); index.isValid()) {
            auto* dialog = new TransactionDialog(this);
            connect(dialog, &TransactionDialog::deposit, this, [=](int amount, const QString& type) {
                auto index = ui->userTable->currentIndex();
                checkDepositResult(userModel->deposit(index.row(), amount, type), this);
            });
            connect(dialog, &TransactionDialog::withdraw, this, [=](int amount) {
                auto index = ui->userTable->currentIndex();
                checkWithdrawResult(userModel->withdraw(index.row(), amount), this);
            });
            connect(dialog, &TransactionDialog::transfer, this, [=](int amount, const QString& to) {
                auto index = ui->userTable->currentIndex();
                checkWithdrawResult(userModel->transfer(index.row(), amount, to), this);
            });
            dialog->exec();
        }
        else
            QMessageBox::information(this, windowTitle(), "请先选中一个用户");
    });
    connect(ui->demandRateAction, &QAction::triggered, this, [=] {
        bool   ok;
        double res = QInputDialog::getDouble(this, "利率调整", "输入新的活期利率", ::demandRate, 0, 1, 4, &ok);
        if (ok)
            ::demandRate = res;
    });
    connect(ui->year1RateAction, &QAction::triggered, this, [=] {
        bool   ok;
        double res = QInputDialog::getDouble(this, "利率调整", "输入新的活期利率", ::demandRate, 0, 1, 4, &ok);
        if (ok)
            ::year1Rate = res;
    });
    connect(ui->year3RateAction, &QAction::triggered, this, [=] {
        bool   ok;
        double res = QInputDialog::getDouble(this, "利率调整", "输入新的活期利率", ::demandRate, 0, 1, 4, &ok);
        if (ok)
            ::year3Rate = res;
    });
    connect(ui->year5RateAction, &QAction::triggered, this, [=] {
        bool   ok;
        double res = QInputDialog::getDouble(this, "利率调整", "输入新的活期利率", ::demandRate, 0, 1, 4, &ok);
        if (ok)
            ::year5Rate = res;
    });
    // tool bar
    ui->toolBar->addActions({ ui->addUserAction, ui->removeUserAction, ui->inquiryAction });
    ui->toolBar->addSeparator();
    ui->toolBar->addActions({ ui->transactionAction });
}

void AdminWindow::setSlot(LoginDialog* dialog) {

    connect(dialog, &LoginDialog::adminLoggedIn, this, [=](const QString& employeeId) {
        ::employeeId = employeeId;
        QFile file("user_data.dat");
        if (!file.exists()) {
            file.open(QIODevice::WriteOnly);
        }
        else {
            if (!checkOpenFile(file, QIODevice::ReadOnly, "读取数据文件失败", this))
                exit(0);
            QDataStream ds(&file);
            quint32     size;
            ds >> size;
            userModel->insertRows(0, size, QModelIndex());
            for (quint32 i = 0; i < size; ++i) {
                ds >> userModel->getModelData()[i];
            }
        }
        file.close();
        show();
    });
}

AdminWindow::~AdminWindow() { delete ui; }

void AdminWindow::closeEvent(QCloseEvent* event) {
    QFile userFile("users.txt");
    if (!checkOpenFile(userFile, QIODevice::WriteOnly, "写入数据文件失败", this)) {
        event->ignore();
        return;
    }
    QTextStream ts(&userFile);

    QFile file("user_data.dat");
    if (!checkOpenFile(file, QIODevice::WriteOnly, "写入数据文件失败", this)) {
        event->ignore();
        return;
    }
    QDataStream ds(&file);
    ds << (quint32)userModel->rowCount();
    for (int i = 0; i < userModel->rowCount(); ++i) {
        auto& user = userModel->getModelData()[i];
        ts << user.id << " " << user.password << '\n';
        ds << user;
    }
    file.close();
    userFile.close();
}
