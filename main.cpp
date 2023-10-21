#include "GloveworksClient.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    GloveworksClient w;
    w.show(); // remove to hide main window

    return app.exec();
}

#else

#include <QLabel>
#include <QDebug>

int main(int argcv, char* argv[])
{
    QApplication app(argc, argv);
    QString text("QSystemTrayIcon is not supported on this platform");

    QLabel* label = new QLabel(text);
    label->setWordWrap();

    label->show();
    qDebug() << text;

    app.exec();
}

#endif
