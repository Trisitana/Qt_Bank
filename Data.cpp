#include "Data.h"
#include "Utils.h"
#include <QDebug>
#include <cmath>

QDataStream& operator<<(QDataStream& ds, const Record& record) { return ds << record.operationDateTime << record.amount << record.balance << record.type << record.employeeId; }
QDataStream& operator>>(QDataStream& ds, Record& record) { return ds >> record.operationDateTime >> record.amount >> record.balance >> record.type >> record.employeeId; }

QVariant RecordModel::data(const QModelIndex& index, int role) const {
    if (index.isValid()) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            const int row = index.row();
            switch (index.column()) {
            case 0:
                return modelData[row].operationDateTime.toString(DATETIME_FORMAT);
            case 1:
                return modelData[row].amount;
            case 2:
                return modelData[row].balance;
            case 3: {
                return modelData[row].type;
            }
            case 4:
                if (modelData[row].type.endsWith("存款"))
                    return modelData[row].getInterest();
                break;
            case 5:
                return modelData[row].employeeId;
            }
        }
    }
    return QVariant();
}

bool RecordModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid())
        return 0;
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const int row = index.row();
        switch (index.column()) {
        case 0:
            modelData[row].operationDateTime = QDateTime::fromString(value.toString(), DATETIME_FORMAT);
            break;
        case 1:
            modelData[row].amount = value.toInt();
            break;
        case 2:
            modelData[row].balance = value.toInt();
            break;
        case 3:
            modelData[row].type = value.toString();
            break;
        case 5:
            modelData[row].employeeId = value.toString();
        }
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

// Qt::ItemFlags RecordModel::flags(const QModelIndex& index) const {
//     if (!index.isValid())
//         return Qt::ItemIsEnabled;
//     if (modelData[index.row()].type != Deposit && index.column() == 4)
//         return Qt::NoItemFlags;

//    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
//}

std::vector<Record>& RecordModel::getModelData() { return modelData; }

const std::vector<Record>& RecordModel::getModelData() const { return modelData; }

RecordModel::RecordModel(QObject* parent) : TableModelBase(parent) {
    header << "操作日期"
           << "金额"
           << "余额"
           << "业务类型"
           << "利息"
           << "营业员工号";
}

User::User() {}

User::User(const User& user) { *this = user; }

User::User(User&& user) { *this = std::move(user); }

User& User::operator=(const User& user) {
    if (this != std::addressof(user)) {
        id = user.id, name = user.name, password = user.password;
        balance                = user.balance;
        address                = user.address;
        records.getModelData() = user.records.getModelData();
    }
    return *this;
}

User& User::operator=(User&& user) {
    if (this != std::addressof(user)) {
        id = user.id, name = user.name, password = user.password;
        balance                = user.balance;
        address                = user.address;
        records.getModelData() = std::move(user.records.getModelData());
    }
    return *this;
}

void User::setLost(bool value) {
    if (isLost == value)
        return;
    isLost = value;
    records.insertRow(records.rowCount());
    records.setData(records.index(records.rowCount() - 1, 0), QDateTime::currentDateTime().toString(DATETIME_FORMAT));
    records.setData(records.index(records.rowCount() - 1, 1), 0);
    records.setData(records.index(records.rowCount() - 1, 2), "");
    if (value) {
        records.setData(records.index(records.rowCount() - 1, 3), "挂失");
        lostDate = QDate::currentDate();
    }
    else {
        records.setData(records.index(records.rowCount() - 1, 3), "取消挂失");
        lostDate = QDate();
    }
}

void User::cancelFixedDeposit(int row) {
    int amount = fixedDeposit.data(fixedDeposit.index(row, 1), Qt::EditRole).toInt();
    balance += amount;
    makeRecord(amount, "取消");
    fixedDeposit.removeRow(row);
}

void User::addFixedDeposit(const Record& record) {
    fixedDeposit.insertRows(fixedDeposit.rowCount(), 1, QModelIndex());
    fixedDeposit.getModelData().back() = record;
}

int User::deposit(int amount, const QString& type) {
    if (isLost)
        return LOST;
    if (type == "活期存款") {
        balance += amount;
    }
    else {
        if (amount > balance)
            return INSUFFICIENT_BALANCE;
        balance -= amount;
    }
    makeRecord(amount, type);
    return SUCCESS;
}

int User::withdraw(int amount) {
    if (isLost)
        return LOST;
    if (amount > balance)
        return INSUFFICIENT_BALANCE;
    balance -= amount;
    makeRecord(amount, "取款");
    return SUCCESS;
}

void User::makeRecord(int amount, const QString& type) {
    int row = records.rowCount();
    records.insertRow(row);

    records.setData(records.index(row, 0), QDateTime::currentDateTime().toString(DATETIME_FORMAT));
    records.setData(records.index(row, 1), amount);
    records.setData(records.index(row, 2), balance);
    records.setData(records.index(row, 3), ::employeeId);
    records.setData(records.index(row, 4), type);
}

QDataStream& operator<<(QDataStream& ds, const User& user) {
    ds << user.id << user.name << user.password << user.address << user.isLost << user.lostDate << user.balance;
    user.records.writeData(ds);
    user.fixedDeposit.writeData(ds);
    return ds;
}
QDataStream& operator>>(QDataStream& ds, User& user) {
    ds >> user.id >> user.name >> user.password >> user.address >> user.isLost >> user.lostDate >> user.balance;
    user.records.readData(ds);
    user.fixedDeposit.readData(ds);
    return ds;
}

QVariant UserModel::data(const QModelIndex& index, int role) const {
    if (index.isValid())
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            const int row = index.row();
            switch (index.column()) {
            case 0:
                return modelData[row].id;
            case 1:
                return modelData[row].name;
            case 2:
                return modelData[row].password;
            case 3:
                return modelData[row].address;
            case 4:
                return modelData[row].balance;
            case 5:
                return modelData[row].isLost ? "是" : "否";
            case 6:
                return modelData[row].lostDate.toString("yyyy/M/d");
            }
        }
    return QVariant();
}

bool UserModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid())
        return 0;
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const int row = index.row();
        switch (index.column()) {
        case 0:
            modelData[row].id = value.toString();
            break;
        case 1:
            modelData[row].name = value.toString();
            break;
        case 2:
            modelData[row].password = value.toString();
            break;
        case 3:
            modelData[row].address = value.toString();
            break;
        case 4:
            modelData[row].balance = value.toInt();
            break;
        case 5:
            modelData[row].isLost = value.toBool();
            break;
        case 6:
            modelData[row].lostDate = QDate::fromString(value.toString(), "yyyy/M/d");
            break;
        }
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

int UserModel::deposit(int row, int amount, const QString& type) { return modelData[row].deposit(amount, type); }

int UserModel::withdraw(int row, int amount) { return modelData[row].withdraw(amount); }

int UserModel::transfer(int row, int amount, const QString& to) {
    auto  res  = std::find_if(modelData.begin(), modelData.end(), [&](const User& user) { return user.id == to; });
    auto& curr = modelData[row];
    if (res == modelData.end())
        return TO_ID_INVALID;
    if (curr.isLost || res->isLost)
        return LOST;
    if (amount > curr.balance)
        return INSUFFICIENT_BALANCE;

    curr.balance -= amount;
    curr.makeRecord(amount, "转账到" + to);

    res->balance += amount;
    curr.makeRecord(amount, "从" + curr.id + "转入");

    return SUCCESS;
}

UserModel::UserModel(QObject* parent) : TableModelBase(parent) {
    header << "账号"
           << "姓名"
           << "密码"
           << "地址"
           << "余额"
           << "是否挂失"
           << "挂失日期";
}

qreal Record::getInterest() const {
    if (type == "活期存款")
        return this->amount * demandRate / 365 * operationDateTime.date().daysTo(QDate::currentDate());
    else if (type == "1年期存款")
        return this->amount * year1Rate;
    else if (type == "3年期存款")
        return this->amount * year3Rate * 3;
    else if (type == "5年期存款")
        return this->amount * year5Rate * 5;

    return -1;
}

FixedDepositModel::FixedDepositModel(QObject* parent) : RecordModel(parent) {
    header.clear();
    header << "操作日期"
           << "金额"
           << "业务类型"
           << "利息";
}

QVariant FixedDepositModel::data(const QModelIndex& index, int role) const { return RecordModel::data(this->index(index.row(), index.column() < 2 ? index.column() : index.column() + 1), role); }

bool FixedDepositModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    return RecordModel::setData(this->index(index.row(), index.column() < 2 ? index.column() : index.column() + 1), value, role);
}
