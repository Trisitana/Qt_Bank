#ifndef DATA_H
#define DATA_H
#include <QAbstractItemModel>
#include <QDataStream>
#include <QDate>
#include <vector>

struct Record {
    QDateTime operationDateTime;
    qint32    amount  = 0;
    qint32    balance = 0;
    QString   type;
    QString   employeeId;
    qreal     getInterest() const;
};

QDataStream& operator<<(QDataStream& ds, const Record& record);
QDataStream& operator>>(QDataStream& ds, Record& record);

template <class T>
class TableModelBase : public QAbstractTableModel {
public:
    TableModelBase(QObject* parent = nullptr) : QAbstractTableModel(parent) {}
    std::vector<T>&       getModelData() { return modelData; }
    const std::vector<T>& getModelData() const { return modelData; }

    int      rowCount(const QModelIndex& = QModelIndex()) const override { return modelData.size(); }
    int      columnCount(const QModelIndex& = QModelIndex()) const override { return header.size(); }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            if (section < header.size()) {
                return header[section];
            }
        }

        return QAbstractItemModel::headerData(section, orientation, role);
    }

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override {
        if (row < 0 || count < 1 || row > rowCount(parent))
            return false;

        beginInsertRows(parent, row, row + count - 1);
        modelData.insert(modelData.cbegin() + row, count, T());

        endInsertRows();
        return true;
    }

    bool removeRows(int row, int count, const QModelIndex& parent) override {
        if (row < 0 || count < 1 || row + count > rowCount(parent))
            return false;
        beginRemoveRows(parent, row, row + count - 1);

        modelData.erase(modelData.cbegin() + row, modelData.cbegin() + row + count);

        endRemoveRows();

        return true;
    }

    bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild) override {
        if (sourceRow < 0 || count < 1 || sourceRow + count > rowCount(sourceParent))
            return false;
        if (sourceRow < destinationChild && destinationChild < sourceRow + count)
            return false;
        if (sourceRow + 1 == destinationChild)
            std::swap(sourceRow, destinationChild);
        if (!beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild))
            return false;
        std::vector<T> tmp(modelData.begin() + sourceRow, modelData.begin() + sourceRow + count);
        modelData.erase(modelData.begin() + sourceRow, modelData.begin() + sourceRow + count);
        if (destinationChild > sourceRow + count)
            destinationChild -= count;
        modelData.insert(modelData.begin() + destinationChild, tmp.begin(), tmp.end());

        endMoveRows();
        return true;
    }

    void readData(QDataStream& ds) {
        quint32 size;
        ds >> size;
        insertRows(0, size);
        for (quint32 i = 0; i < size; ++i) {
            T item;
            ds >> item;
            modelData[i] = std::move(item);
        }
    }
    void writeData(QDataStream& ds) const {
        ds << static_cast<quint32>(modelData.size());
        for (const auto& item : modelData)
            ds << item;
    }

protected:
    QStringList    header;
    std::vector<T> modelData;
};

class RecordModel : public TableModelBase<Record> {  // read-only model, becuase sales records shouldn't be modified manually
    Q_OBJECT
public:
    RecordModel(QObject* parent = nullptr);

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex& index, int role) const override;
    bool     setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    // Qt::ItemFlags              flags(const QModelIndex& index) const override;
    std::vector<Record>&       getModelData();        // for simplicity...
    const std::vector<Record>& getModelData() const;  // underlying data should never be exposed in such an unsafe way
};

class FixedDepositModel : public RecordModel {
public:
    FixedDepositModel(QObject* parent = nullptr);

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex& index, int role) const override;
    bool     setData(const QModelIndex& index, const QVariant& value, int role) override;
};

struct User {
    friend class UserModel;
    User();
    User(const User& user);
    User(User&& user);
    User& operator=(const User& user);
    User& operator=(User&& user);

    void setLost(bool value);

    void cancelFixedDeposit(int row);
    void addFixedDeposit(const Record& record);
    int  deposit(int amount, const QString& type);
    int  withdraw(int amount);

    QString           id, name, password;
    QString           address;
    bool              isLost = false;
    QDate             lostDate;
    qint32            balance = 0;
    RecordModel       records;
    FixedDepositModel fixedDeposit;

private:
    void makeRecord(int amount, const QString& type);
};

QDataStream& operator<<(QDataStream& ds, const User& user);
QDataStream& operator>>(QDataStream& ds, User& user);

class UserModel : public TableModelBase<User> {
    Q_OBJECT
    // QAbstractItemModel interface
public:
    UserModel(QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role) const override;
    bool     setData(const QModelIndex& index, const QVariant& value, int role) override;

    int deposit(int row, int amount, const QString& type);
    int withdraw(int row, int amount);
    int transfer(int row, int amount, const QString& to);
};

#endif  // DATA_H
