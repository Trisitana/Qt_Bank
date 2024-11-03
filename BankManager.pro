QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++1z

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AdminWindow.cpp \
    Data.cpp \
    Delegates.cpp \
    LoginDialog.cpp \
    SearchDialog.cpp \
    TitleBar.cpp \
    TransactionDialog.cpp \
    UserInfoDialog.cpp \
    UserWindow.cpp \
    main.cpp

HEADERS += \
    AdminWindow.h \
    Data.h \
    Delegates.h \
    LoginDialog.h \
    SearchDialog.h \
    ShadowWidget.h \
    TitleBar.h \
    TransactionDialog.h \
    UserInfoDialog.h \
    UserWindow.h \
    Utils.h

FORMS += \
    AdminWindow.ui \
    LoginDialog.ui \
    SearchDialog.ui \
    TitleBar.ui \
    TransactionDialog.ui \
    UserInfoDialog.ui \
    UserWindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    BankManager.qrc \
    qdarkstyle/light/lightstyle.qrc
