#ifndef SHADOWWIDGET_H
#define SHADOWWIDGET_H

#include "TitleBar.h"
#include <QDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <cmath>

#undef GLOBAL_POS
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
#define GLOBAL_POS(e) e->globalPosition().toPoint()
#else
#define GLOBAL_POS(e) e->globalPos()
#endif
class QMainWindow;
template <class Base>
class ShadowWidget : public Base {
    enum Edge { NoEdge = 0, Left = 0x1, Right = 0x2, Top = 0x4, Bottom = 0x8, TopLeft = Left | Top, TopRight = Right | Top, BottomLeft = Bottom | Left, BottomRight = Bottom | Right };

public:
    explicit ShadowWidget(QWidget* parent = nullptr) : Base(parent), _title(new TitleBar(this)), _layout(new QVBoxLayout()), _ptr(this) {
        _ptr->setAttribute(Qt::WA_TranslucentBackground);
        _ptr->setWindowFlags(Qt::FramelessWindowHint);
        _ptr->setContentsMargins(_shadowWidth, _shadowWidth, _shadowWidth, _shadowWidth);
        _ptr->installEventFilter(this);
        _layout->addWidget(_title);
        _layout->setSpacing(0);
        _layout->setContentsMargins(0, 0, 0, 0);
        if constexpr (std::is_same_v<Base, QMainWindow>) {
            QWidget* window = new QWidget(parent);
            window->setLayout(_layout);
            _ptr->setCentralWidget(window);
        }
        else
            _ptr->setLayout(_layout);
    }
    ~ShadowWidget() {}

public:
    int  shadowWidth() const { return _shadowWidth; }
    void setShadowWidth(int newShadowWidth) {
        if (_shadowWidth == newShadowWidth)
            return;
        int delta = newShadowWidth - _shadowWidth;
        _rect.adjust(delta, delta, -2 * delta, -2 * delta);
        _shadowWidth = newShadowWidth;
        _ptr->setContentsMargins(_shadowWidth, _shadowWidth, _shadowWidth, _shadowWidth);
    }
    const QColor& backgroundColor() const { return _backgroundColor; }

    void          setBackgroundColor(const QColor& newBackgroundColor) { _backgroundColor = newBackgroundColor; }
    int           borderRadius() const { return _borderRadius; }
    void          setBorderRadius(int newBorderRadius) { _borderRadius = newBorderRadius; }
    const QColor& borderColor() const { return _borderColor; }

    void         setBorderColor(const QColor& newBorderColor) { _borderColor = newBorderColor; }
    const QRect& rect() const { return _rect; }

    const QColor& shadowColor() const { return _shadowColor; }

    void setShadowColor(const QColor& newShadowColor) { _shadowColor = newShadowColor; }
    bool isResizable() const { return _resizable; }
    void setResizable(bool resizable) { this->_resizable = resizable; }
    bool isMovable() const { return _movable; }
    void setMovable(bool movable) { this->_movable = movable; }

    TitleBar*       title() { return _title; }
    const TitleBar* title() const { return _title; }

protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        if (_borderRadius == 0) {
            painter.fillRect(_rect, QBrush(_backgroundColor));
            painter.setPen(_borderColor);
            painter.drawRoundedRect(_rect, _borderRadius, _borderRadius);
        }
        else {
            QPainterPath path;
            path.addRoundedRect(_rect, _borderRadius, _borderRadius);
            painter.setPen(_borderColor);
            painter.fillPath(path, _backgroundColor);
            painter.drawPath(path);
        }

        for (int i = 1; i < _shadowWidth; i++) {
            _shadowColor.setAlpha(100 * (1 - std::pow(i * 1.0 / _shadowWidth, 0.4)));
            painter.setPen(_shadowColor);
            if (_borderRadius)
                painter.drawRoundedRect(_rect.x() - i, _rect.y() - i, _rect.width() + 2 * i, _rect.height() + 2 * i, _borderRadius, _borderRadius);
            else
                painter.drawRect(_rect.x() - i, _rect.y() - i, _rect.width() + 2 * i, _rect.height() + 2 * i);
        }
        QWidget::paintEvent(event);
    }
    bool eventFilter(QObject* watched, QEvent* event) override {
        auto* e = static_cast<QMouseEvent*>(event);
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            if (e->button() == Qt::LeftButton) {
                _pressed = true;
                _offset  = GLOBAL_POS(e) - _ptr->geometry().topLeft();
                recalculate(GLOBAL_POS(e));
            }
            break;
        case QEvent::MouseButtonRelease:
            if (e->button() == Qt::LeftButton) {
                _pressed        = false;
                _mouseEdgeFlags = NoEdge;
            }
            break;
        case QEvent::MouseMove:
            if (_pressed) {
                if (_resizable && _cursorShapeChanged) {
                    QRect  origRect = _ptr->geometry();
                    QPoint mousePos = GLOBAL_POS(e);
                    int    left, top, right, bottom;
                    origRect.getCoords(&left, &top, &right, &bottom);
                    if (testFlag(Top))
                        top = mousePos.y();
                    if (testFlag(Left))
                        left = mousePos.x();
                    if (testFlag(Right))
                        right = mousePos.x();
                    if (testFlag(Bottom))
                        bottom = mousePos.y();

                    QRect newRect(QPoint(left, top), QPoint(right, bottom));

                    if (newRect.isValid()) {
                        if (_ptr->minimumWidth() < newRect.width() && _ptr->minimumHeight() < newRect.height())
                            _ptr->setGeometry(newRect);
                    }
                }
                else if (_movable) {
                    _ptr->move(GLOBAL_POS(e) - _offset);
                }
            }
            else if (_resizable)
                updateCursorShape(GLOBAL_POS(e));
            break;
        case QEvent::Leave:
            if (!_pressed)
                _ptr->unsetCursor();
            break;
        case QEvent::HoverMove:
            if (_resizable && !_pressed)
                updateCursorShape(GLOBAL_POS(e));
            break;
        case QEvent::Resize:
            _rect.setSize(_ptr->frameSize() - QSize(2 * _shadowWidth, 2 * _shadowWidth));
            break;
        default:
            break;
        }
        return QWidget::eventFilter(watched, event);
    }

    void init(QLayout* content) {
        if constexpr (std::is_same_v<Base, QMainWindow>) {
            QWidget* window = new QWidget(_ptr->parentWidget());
            window->setLayout(_layout);
            _layout->addWidget(_ptr->centralWidget());
            _ptr->setCentralWidget(window);
        }
        else
            _layout->addLayout(content);
        _rect = QRect(_shadowWidth, _shadowWidth, _ptr->width() - 2 * _shadowWidth, _ptr->height() - 2 * _shadowWidth);
    }

private:
    void updateCursorShape(const QPoint& mousePos) {
        if (_ptr->isFullScreen() || _ptr->isMaximized()) {
            if (_cursorShapeChanged)
                _ptr->unsetCursor();
            return;
        }
        recalculate(mousePos);
        if (testFlag(TopLeft) || testFlag(BottomRight))
            _ptr->setCursor(Qt::SizeFDiagCursor), _cursorShapeChanged = true;
        else if (testFlag(TopRight) || testFlag(BottomLeft))
            _ptr->setCursor(Qt::SizeBDiagCursor), _cursorShapeChanged = true;
        else if (testFlag(Left) || testFlag(Right))
            _ptr->setCursor(Qt::SizeHorCursor), _cursorShapeChanged = true;
        else if (testFlag(Top) || testFlag(Bottom))
            _ptr->setCursor(Qt::SizeVerCursor), _cursorShapeChanged = true;
        else {
            if (_cursorShapeChanged) {
                _ptr->unsetCursor();
                _cursorShapeChanged = false;
            }
        }
    }
    void recalculate(const QPoint& globalMouse) {
        int   globalMouseX = globalMouse.x(), globalMouseY = globalMouse.y();
        QRect frameRect = _ptr->frameGeometry();
        int   frameX = frameRect.x(), frameY = frameRect.y();

        setFlag(Left, globalMouseX >= frameX && globalMouseX <= frameX + _shadowWidth);
        setFlag(Right, globalMouseX >= frameX + frameRect.width() - _shadowWidth && globalMouseX <= frameX + frameRect.width());
        setFlag(Top, globalMouseY >= frameY && globalMouseY <= frameY + _shadowWidth);
        setFlag(Bottom, globalMouseY >= frameY + frameRect.height() - _shadowWidth && globalMouseY <= frameY + frameRect.height());
    }

    inline void setFlag(Edge flag, bool value) { _mouseEdgeFlags = static_cast<Edge>(value ? int(_mouseEdgeFlags) | int(flag) : int(_mouseEdgeFlags) & ~int(flag)); }
    inline bool testFlag(Edge flag) const noexcept {
        return (_mouseEdgeFlags & static_cast<int>(flag)) == static_cast<int>(flag) && (static_cast<int>(flag) != 0 || _mouseEdgeFlags == static_cast<int>(flag));
    }

    bool   _pressed            = false;
    bool   _cursorShapeChanged = false;
    Edge   _mouseEdgeFlags;
    QPoint _offset;

    int          _shadowWidth     = 8;
    QColor       _borderColor     = QColor(0xf3, 0xf3, 0xf3);
    QColor       _backgroundColor = QColor(0xf3, 0xf3, 0xf3);
    QColor       _shadowColor     = QColor(50, 50, 50);
    int          _borderRadius    = 4;
    QRect        _rect;
    bool         _resizable = true;
    bool         _movable   = true;
    TitleBar*    _title;
    QVBoxLayout* _layout;
    Base*        _ptr;
};

#endif  // SHADOWWIDGET_H
