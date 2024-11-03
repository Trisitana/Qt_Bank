#include "TransactionDialog.h"
#include "Utils.h"
#include "qmessagebox.h"
#include "ui_TransactionDialog.h"
#include <QValidator>

TransactionDialog::TransactionDialog(QWidget* parent) : ShadowWidget(parent), ui(new Ui::TransactionDialog) {
    ui->setupUi(this);
    title()->setButtons(TitleBar::Close);
    init(ui->gridLayout);
    title()->setIconColor(QColor(0x2, 0x2, 0x2), TitleBar::Close, ":/xicons/close.svg");
    title()->setButtonSize(30, 30);
    title()->setIconSize(18, 18);
    title()->setTitle("业务操作框");
    title()->setButtons(TitleBar::Close);
    ui->gridLayout->setSpacing(10);
    ui->gridLayout->setContentsMargins(10, 0, 10, 10);

    auto* validator = new QIntValidator();
    validator->setRange(0, 100000);
    ui->withdrawLineEdit->setValidator(validator);
    ui->depositLineEdit->setValidator(validator);
    ui->transferLineEdit->setValidator(validator);

    connect(ui->withdrawButton, &QPushButton::clicked, this, [=] {
        QString val;
        CHECK_STR_EMPTY(withdrawLineEdit->text(), val)
        emit withdraw(val.toInt());
    });
    connect(ui->depositButton, &QPushButton::clicked, this, [=] {
        QString val;
        CHECK_STR_EMPTY(depositLineEdit->text(), val)
        emit deposit(val.toInt(), ui->depositTypeComboBox->currentText());
    });
    connect(ui->transferButton, &QPushButton::clicked, this, [=] {
        QString val, toId;
        CHECK_STR_EMPTY(transferLineEdit->text(), val)
        CHECK_STR_EMPTY(toIdLineEdit->text(), toId)
        emit transfer(val.toInt(), toId);
    });
}

TransactionDialog::~TransactionDialog() { delete ui; }
