#ifndef GOODSDELEGATE_H
#define GOODSDELEGATE_H

#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSpinBox>
#include <QStyledItemDelegate>

class SpinBoxDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    SpinBoxDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class DoubleSpinBoxDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    DoubleSpinBoxDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class DateEditDelegate : public QStyledItemDelegate {
    Q_OBJECT

    // QAbstractItemDelegate interface
public:
    DateEditDelegate(QObject* parent = nullptr);
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void     setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void     setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void     updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class ComboBoxDelegate : public QStyledItemDelegate {
    Q_OBJECT

    // QAbstractItemDelegate interface
public:
    ComboBoxDelegate(QStringList items, QObject* parent = nullptr);
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void     setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void     setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void     updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    QStringList items;
};

#endif  // GOODSDELEGATE_H
