#pragma once

#include "RegistryManager.h"
#include "Logger.h"
#include "Path.h"
#include "ZipManager.h"

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QSet>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QPlainTextEdit>
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QFile>
#include <QDir>
#include <QTimer>



class GloveworksClient : public QMainWindow
{

public:
    GloveworksClient(QWidget* parent = nullptr);

public slots:
    void manageDownloadWdg();
    void mapsDownloadFinished();
    void cleanupAndQuit();

private:
    void createActions();
    void createSysTray();
    void closeWithTimer(QWidget* wdg, int timeout);
    QPlainTextEdit* createLogWin(QWidget* wdg);

    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
    QAction* connectAction;
    QAction* updateMapsAction;
    QAction* settingsAction;
    QAction* quitAction;

    QWidget* serverWdg = nullptr;
    QWidget* downloadWdg = nullptr;
    QWidget* optionWdg = nullptr;

    QNetworkAccessManager networkManager;
    QNetworkReply* mapsDownloadReply = nullptr;
    QNetworkReply* serverInfoReply = nullptr;

    QPlainTextEdit* logText = nullptr;

    QString tempPath = nullptr;
};
