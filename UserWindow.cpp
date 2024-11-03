#include "UserWindow.h"
#include "LoginDialog.h"
#include "SearchDialog.h"
#include "TitleBar.h"
#include "Utils.h"
#include "qmenu.h"
#include "qtoolbar.h"
#include "ui_UserWindow.h"
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QTextStream>
#include <map>

UserWindow::UserWindow(QWidget* parent) : ShadowWidget(parent), ui(new Ui::UserWindow), userModel(new UserModel()) {
    ui->setupUi(this);

    title()->setButtonSize(30, 30);
    title()->setIconSize(18, 18);
    title()->setTitle("  " + windowTitle());
    title()->setStyleSheet("font:bold 20px;color:white;background:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(38, 205, 255, 255), stop:0.516908 rgba(7, 188, 252, 255), stop:1 "
                           "rgba(1, 181, 249, 255));");
    title()->setMinimumHeight(50);
    init(ui->gridLayout);
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, [=](int index) {
        if (index == 3)
            close();
        else {
            ui->stackedWidget->setCurrentIndex(index);
        }
    });
    ui->listWidget->setCurrentRow(0);

    // page 1
    connect(ui->lostButton, &QPushButton::clicked, this, [=] {
        user->setLost(true);
        ui->lostButton->setEnabled(!user->isLost);
        ui->cancelLostButton->setEnabled(user->isLost);
        QMessageBox::information(this, windowTitle(), "挂失成功");
      });
    connect(ui->cancelLostButton, &QPushButton::clicked, this, [=] {
        user->setLost(false);
        ui->lostButton->setEnabled(!user->isLost);
        ui->cancelLostButton->setEnabled(user->isLost);
        QMessageBox::information(this, windowTitle(), "挂失失败 ");
      });
    // page 2
    ui->withdrawLineEdit->setValidator(new QIntValidator());
    ui->depositLineEdit->setValidator(new QIntValidator());

    ui->fixedDepositTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->fixedDepositTable->horizontalHeader()->setFixedHeight(40);

    connect(ui->fixedDepositTable, &QTableView::customContextMenuRequested, this, [=](const QPoint&) {
        QMenu menu;
        menu.addAction("取消定期存款", this, [=] {
            if (ui->fixedDepositTable->currentIndex().isValid()) {
                user->cancelFixedDeposit(ui->fixedDepositTable->currentIndex().row());
                ui->balanceLabel->setText(QString::number(user->balance));
            }
        });
        menu.exec(QCursor::pos());
    });
    connect(ui->depositButton, &QPushButton::clicked, this, [=] {
        int     amount = ui->depositLineEdit->text().toInt();
        QString type   = ui->depositTypeComboBox->currentText();
        int     res    = user->deposit(amount, type);
        checkDepositResult(res, this);
        if (res == SUCCESS) {
            ui->depositLineEdit->clear();
            ui->balanceLabel->setText(QString::number(user->balance));
            ui->depositTypeComboBox->setCurrentIndex(0);
        }
    });
    connect(ui->withdrawButton, &QPushButton::clicked, this, [=] {
        int amount = ui->withdrawLineEdit->text().toInt();
        int res    = user->withdraw(amount);
        checkWithdrawResult(res, this);
        if (res == SUCCESS) {
            ui->balanceLabel->setText(QString::number(user->balance));
            ui->withdrawLineEdit->clear();
        }
    });
    connect(ui->transferButton, &QPushButton::clicked, this, [=] {
        int amount = ui->transferLineEdit->text().toInt();
        int currIndex =
            std::distance(userModel->getModelData().begin(), std::find_if(userModel->getModelData().begin(), userModel->getModelData().end(), [=](const User& u) { return user->id == u.id; }));
        int res = userModel->transfer(currIndex, amount, ui->toIdLineEdit->text());
        checkTransferResult(res, this);
        if (res == SUCCESS) {
            ui->balanceLabel->setText(QString::number(user->balance));
            ui->transferLineEdit->clear();
            ui->toIdLineEdit->clear();
        }
    });


}

UserWindow::~UserWindow() { delete ui; }

void UserWindow::setSlot(LoginDialog* dialog) {
    connect(dialog, &LoginDialog::userLoggedIn, this, [=](const QString& id) {
        QFile file("user_data.dat");
        if (!checkOpenFile(file, QIODevice::ReadOnly, "读取数据文件失败", this))
            exit(0);
        QDataStream ds(&file);
        quint32     size;
        ds >> size;
        userModel->insertRows(0, size, QModelIndex());
        for (quint32 i = 0; i < size; ++i) {
            ds >> userModel->getModelData()[i];
        }
        file.close();
        user = std::addressof(*std::find_if(userModel->getModelData().begin(), userModel->getModelData().end(), [=](const User& u) { return u.id == id; }));
        // page 1
        ui->idLabel->setText(user->id);
        ui->nameLineEdit->setText(user->name);
        ui->passwordLineEdit->setText(user->password);
        ui->addressPlainTextEdit->setPlainText(user->address);
        ui->lostButton->setEnabled(!user->isLost);
        ui->cancelLostButton->setEnabled(user->isLost);

        // page 2

        ui->balanceLabel->setText(QString::number(user->balance));
        ui->fixedDepositTable->setModel(&user->fixedDeposit);
        connect(&user->records, &QAbstractItemModel::dataChanged, &user->fixedDeposit, [=](const QModelIndex& index, const QModelIndex&, const QVector<int>&) {
            if (user->records.data(user->records.index(index.row(), 3), Qt::EditRole).toString().endsWith("年期存款")) {
                user->addFixedDeposit(user->records.getModelData()[index.row()]);
                ui->balanceLabel->setText(QString::number(user->balance));
            }
        });
        // page 3
        auto* proxy = new QSortFilterProxyModel();
        proxy->setSourceModel(&user->records);
        ui->recordTable->setModel(proxy);
        ui->recordTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        ui->recordTable->horizontalHeader()->setFixedHeight(40);
        auto* toolBar = new QToolBar();
        ui->verticalLayout->insertWidget(0, toolBar);
        toolBar->addAction(QIcon(":/icons/ashbin.png"), "删除记录", [=] {
            if (!user->isLost && ui->recordTable->currentIndex().isValid()) {
                user->records.removeRows(ui->recordTable->currentIndex().row(), 1, QModelIndex());
            }
        });
        toolBar->addAction(QIcon(":/icons/search.svg"), "搜索", [=] { SearchDialog(proxy, this).exec(); });
        show();
    });
}

void UserWindow::closeEvent(QCloseEvent* event) {
    user->name     = ui->nameLineEdit->text();
    user->password = ui->passwordLineEdit->text();
    user->address  = ui->addressPlainTextEdit->toPlainText();
    user->balance  = ui->balanceLabel->text().toInt();
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
