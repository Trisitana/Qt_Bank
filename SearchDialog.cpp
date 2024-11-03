#include "SearchDialog.h"
#include "ui_SearchDialog.h"

SearchDialog::SearchDialog(QSortFilterProxyModel* _model, QWidget* parent) : ShadowWidget<QDialog>(parent), ui(new Ui::SearchDialog()), model(_model) {
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setResizable(false);
    init(ui->gridLayout);
    title()->hide();
    for (int i = 0; i < model->columnCount(); ++i)
        ui->cmb_list->addItem(model->headerData(i, Qt::Horizontal).toString());

    connect(ui->tbtn_search, &QToolButton::clicked, this, [=] {
        model->setFilterKeyColumn(ui->cmb_list->currentIndex());
        model->setFilterFixedString(ui->line_text->text());
    });

    connect(ui->tbtn_cancel, &QToolButton::clicked, this, [=] {
        model->setFilterFixedString("");
        close();
    });
}
