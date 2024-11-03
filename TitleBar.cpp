#include "TitleBar.h"
#include "ui_TitleBar.h"
#include <QBitmap>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QGraphicsColorizeEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#undef GLOBAL_POS
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
#define GLOBAL_POS(e) e->globalPosition().toPoint()
#else
#define GLOBAL_POS(e) e->globalPos()
#endif

TitleBar::TitleBar(QWidget* parentWidget, TitleButtons button, QWidget* parent) : QWidget(parent), ui(new Ui::TitleBar), flag(button), parentWidget(parentWidget) {
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint, true);
    installEventFilter(this);
    setButtons(button);
    if (parentWidget && parentWidget->layout()) {
        auto contentMargins = parentWidget->layout()->contentsMargins();
        parentWidget->layout()->setContentsMargins(QMargins(0, 0, 0, contentMargins.bottom()));
    }
    connect(ui->btnClose, &QPushButton::clicked, this, [=] { parentWidget->close(); });
    connect(ui->btnMinimize, &QPushButton::clicked, this, [=] { parentWidget->showMinimized(); });
    connect(ui->btnMaximize, &QPushButton::clicked, this, [=] {
        parentWidget->showMaximized();
        ui->btnRestore->show();
        ui->btnMaximize->hide();
    });
    connect(ui->btnRestore, &QPushButton::clicked, this, [=] {
        parentWidget->showNormal();
        ui->btnRestore->hide();
        ui->btnMaximize->show();
    });
}

TitleBar::~TitleBar() { delete ui; }

void TitleBar::setTitle(const QString& title) { ui->title->setText(title); }

void TitleBar::setTitleFont(const QFont& font) { ui->title->setFont(font); }

void TitleBar::setButtons(TitleButtons button) {
    if (button.testFlag(Restore)) {
        flag.setFlag(Maximize, true);
        flag.setFlag(Restore, false);
    }
    operation(
        button, [](QPushButton* button) { button->show(); }, [](QPushButton* button) { button->hide(); });
}

void TitleBar::setIconSize(QSize size, TitleButtons button) {
    operation(button, [&size](QPushButton* button) { button->setIconSize(size); });
}

QSize TitleBar::iconSize(TitleButton button) const {
    QSize res;
    if (button != Full)
        const_cast<TitleBar*>(this)->operation(button, [&res](QPushButton* button) { res = button->iconSize(); });
    return res;
}

void TitleBar::setIconSize(int w, int h, TitleButtons button) { setIconSize(QSize(w, h), button); }

void TitleBar::setButtonIcon(TitleButton button, QIcon icon) {
    if (button != Full)
        operation(button, [&icon](QPushButton* button) { button->setIcon(icon); });
}

void TitleBar::setButtonSize(QSize size, TitleButtons button) {
    operation(button, [&size](QPushButton* button) { button->setFixedSize(size); });
}

QSize TitleBar::buttonSize(TitleButton button) const {
    QSize res;
    if (button != Full)
        const_cast<TitleBar*>(this)->operation(button, [&res](QPushButton* button) { res = button->size(); });
    return res;
}

void TitleBar::setButtonSize(int w, int h, TitleButtons button) { setButtonSize(QSize(w, h), button); }

void TitleBar::setIconColor(const QColor& color, TitleButton button, const QString& iconPath) {
    if (button == Full)
        return;
    QString path = iconPath;
    if (path.isEmpty()) {
        if (button == Close)
            path = ":/xicons/close.svg";
        else if (button == Maximize)
            path = ":/xicons/maximize.svg";
        else if (button == Minimize)
            path = ":/xicons/minimize.svg";
        else if (button == Restore)
            path = ":/xicons/restore.svg";
        else {
            throw std::invalid_argument("must select the iconPath");
        }
    }

    operation(button, [&](QPushButton* button) {
        QPixmap  pixmap = QPixmap(path);
        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.setBrush(color);
        painter.setPen(color);
        painter.drawRect(pixmap.rect());
        button->setIcon(QIcon(pixmap));
    });
}

void TitleBar::setStyle(QStyle* style, TitleButtons button) {
    operation(button, [=](QPushButton* button) { button->setStyle(style); });
}

void TitleBar::setStyleSheet(const QString& style, TitleButtons button) {
    operation(button, [=](QPushButton* button) { button->setStyleSheet(style); });
}

void TitleBar::addLayout(QLayout* layout) { ui->layout->addLayout(layout, ui->layout->rowCount(), 0, 1, ui->layout->columnCount()); }

QWidget* TitleBar::getParentWidget() const { return parentWidget; }

void TitleBar::setParentWidget(QWidget* newParentWidget) { parentWidget = newParentWidget; }

bool TitleBar::eventFilter(QObject* watched, QEvent* event) {
    auto e = static_cast<QMouseEvent*>(event);

    switch (event->type()) {
    case QEvent::MouseButtonPress:
        if (e->button() == Qt::LeftButton) {
            isDragged = true;
            offset    = GLOBAL_POS(e) - parentWidget->frameGeometry().topLeft();
        }
        return true;
    case QEvent::MouseMove:
        if (isDragged)
            parentWidget->move(GLOBAL_POS(e) - offset);
        return true;
    case QEvent::MouseButtonRelease:
        if (e->button() == Qt::LeftButton)
            isDragged = false;
        return true;
    case QEvent::MouseButtonDblClick:
        if (flag.testFlag(Maximize)) {
            if (parentWidget->isMaximized())
                emit ui->btnRestore->clicked();
            else
                emit ui->btnMaximize->clicked();
        }
        return true;
    case QEvent::Paint: {
        QStyleOption opt;

        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
    default:
        break;
    }
    return QWidget::eventFilter(watched, event);
}

template <class Fn, class Fn2>
void TitleBar::operation(TitleButtons button, Fn op, Fn2 op2) {
    if (button.testFlag(Close))
        op(ui->btnClose);
    else
        op2(ui->btnClose);
    if (button.testFlag(Maximize))
        op(ui->btnMaximize);
    else
        op2(ui->btnMaximize);
    if (button.testFlag(Minimize))
        op(ui->btnMinimize);
    else
        op2(ui->btnMinimize);
    if (button.testFlag(Restore))
        op(ui->btnRestore);
    else
        op2(ui->btnRestore);
}
