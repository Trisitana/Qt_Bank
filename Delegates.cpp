#include "delegates.h"
#include <QBitmap>
#include <QDebug>
#include <QEvent>
#include <QPainter>

DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

QWidget* DoubleSpinBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /* option */, const QModelIndex& /* index */) const {
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(0);
    editor->setMaximum(1000);
    return editor;
}
void DoubleSpinBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    int value = index.model()->data(index, Qt::EditRole).toDouble();

    QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}
void DoubleSpinBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}
void DoubleSpinBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /* index */) const { editor->setGeometry(option.rect); }

SpinBoxDelegate::SpinBoxDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

QWidget* SpinBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /* option */, const QModelIndex& /* index */) const {
    QSpinBox* editor = new QSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(0);
    editor->setMaximum(1000);
    return editor;
}
void SpinBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    int value = index.model()->data(index, Qt::EditRole).toInt();

    QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}
void SpinBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();

    model->setData(index, spinBox->value(), Qt::EditRole);
}
void SpinBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /* index */) const { editor->setGeometry(option.rect); }

QWidget* DateEditDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex&) const {
    QDateEdit* editor = new QDateEdit(QDate::currentDate(), parent);
    editor->setFrame(false);
    editor->setCalendarPopup(true);
    return editor;
}

void DateEditDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QDateEdit* dateEdit = static_cast<QDateEdit*>(editor);
    dateEdit->setDate(QDate::fromString(index.model()->data(index, Qt::EditRole).toString(), "yyyy/M/d"));
}

void DateEditDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QDateEdit* dateEdit = static_cast<QDateEdit*>(editor);
    model->setData(index, dateEdit->date().toString("yyyy/M/d"), Qt::EditRole);
}

void DateEditDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const { editor->setGeometry(option.rect); }

DateEditDelegate::DateEditDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

ComboBoxDelegate::ComboBoxDelegate(QStringList items, QObject* parent) : QStyledItemDelegate(parent), items(items) {}

QWidget* ComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QComboBox* comboBox = new QComboBox(parent);
    comboBox->addItems(items);
    comboBox->setFrame(false);
    return comboBox;
}

void ComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    comboBox->setCurrentText(index.model()->data(index, Qt::EditRole).toString());
}

void ComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const { model->setData(index, static_cast<QComboBox*>(editor)->currentText(), Qt::EditRole); }

void ComboBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const { editor->setGeometry(option.rect); }
