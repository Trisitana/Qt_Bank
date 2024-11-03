#include "AdminWindow.h"

#include "LoginDialog.h"
#include "UserWindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QFile        f(":qdarkstyle/light/lightstyle.qss");
    LoginDialog  dialog;
    AdminWindow  adminWindow;
    adminWindow.setSlot(&dialog);
    UserWindow userWindow;
    userWindow.setSlot(&dialog);
    if (!f.exists()) {
        printf("Unable to set stylesheet, file not found\n");
    }
    else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
    dialog.exec();

    return a.exec();
}
