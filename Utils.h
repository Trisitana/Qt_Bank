#ifndef UTILS_H
#define UTILS_H
#include <QDataStream>
#include <QFile>
#include <QMessageBox>
#define DATE_FORMAT "yyyy/M/d"
#define DATETIME_FORMAT "yyyy/M/d HH:mm:ss"
#define CHECK_EMPTY(TYPE, NAME, MEMBER)                                         \
    do {                                                                        \
        TYPE tmp = ui->NAME;                                                    \
        if (tmp.isEmpty()) {                                                    \
            QMessageBox::critical(this, windowTitle(), "数据不能为空，请重试"); \
            return;                                                             \
        }                                                                       \
        else                                                                    \
            MEMBER = tmp;                                                       \
    } while (0);
#define CHECK_STR_EMPTY(NAME, MEMBER) CHECK_EMPTY(QString, NAME, MEMBER)

#define LOST 1
#define ID_INVALID 2
#define TO_ID_INVALID 3
#define INSUFFICIENT_BALANCE 4
#define SUCCESS 0

template <class = void>
bool checkOpenFile(QFile& file, QFile::OpenModeFlag flag, const QString& errMsg, QWidget* parent) {
    while (true) {
        if (!file.open(flag)) {
            auto res = QMessageBox::critical(parent, parent->windowTitle(), errMsg, QMessageBox::Retry, QMessageBox::Abort, QMessageBox::Cancel);
            if (res == QMessageBox::Abort)
                exit(0);
            if (res == QMessageBox::Cancel)
                return false;
        }
        else
            return true;
    }
}

template <class = void>
void checkDepositResult(int res, QWidget* parent) {
    switch (res) {
    case LOST:
        QMessageBox::information(parent, "存款业务", "存款失败，此卡已挂失");
        break;
    case SUCCESS:
        QMessageBox::information(parent, "存款业务", "存款成功");
        break;
    case INSUFFICIENT_BALANCE:
        QMessageBox::information(parent, "存款业务", "定期存款失败，余额不足");
        break;
    }
}

template <class = void>
void checkWithdrawResult(int res, QWidget* parent) {
    switch (res) {
    case LOST:
        QMessageBox::information(parent, "取款业务", "取款失败，此卡已挂失");
        break;
    case SUCCESS:
        QMessageBox::information(parent, "取款业务", "取款成功");
        break;
    case INSUFFICIENT_BALANCE:
        QMessageBox::information(parent, "取款业务", "取款失败，余额不足");
        break;
    }
}

template <class = void>
void checkTransferResult(int res, QWidget* parent) {
    switch (res) {
    case LOST:
        QMessageBox::information(parent, "转账业务", "转账失败，此卡或转入账户已挂失");
        break;
    case SUCCESS:
        QMessageBox::information(parent, "转账业务", "转账成功");
        break;
    case INSUFFICIENT_BALANCE:
        QMessageBox::information(parent, "转账业务", "转账失败，余额不足");
        break;
    case TO_ID_INVALID:
        QMessageBox::information(parent, "转账业务", "转账失败，转入账户不存在");
        break;
    }
}

inline double  year1Rate = 0.0035, year3Rate = 0.0198, year5Rate = 0.0225, demandRate = 0.0350;
inline QString employeeId;

#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
template <typename T>
typename std::enable_if<std::is_enum<T>::value, QDataStream&>::type& operator>>(QDataStream& s, T& t) {
    return s >> reinterpret_cast<typename std::underlying_type<T>::type&>(t);
}
#endif
#endif  // UTILS_H
