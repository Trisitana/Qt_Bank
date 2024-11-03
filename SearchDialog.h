#pragma once
#include "ShadowWidget.h"
#include <QAction>
#include <QDialog>
#include <QHeaderView>
#include <QMouseEvent>
#include <QSortFilterProxyModel>
#include <QTableWidget>
namespace Ui {
    class SearchDialog;

}  // namespace Ui

class SearchDialog : public ShadowWidget<QDialog> {
    Q_OBJECT
public:
    SearchDialog(QSortFilterProxyModel* model, QWidget* parent = Q_NULLPTR);

private:
    Ui::SearchDialog* ui;

    QSortFilterProxyModel* model;
};
