#pragma once

#include "RegistryManager.h"
#include "Logger.h"
#include "Path.h"
#include "ZipManager.h"
#include "ServersManager.h"

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
#include <QPushButton>
#include <QCheckbox>
#include <QUuid>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QJsonArray>
#include <QStringList>
#include <QClipboard>

class GloveworksClient : public QMainWindow
{

public:
    GloveworksClient(QWidget* parent = nullptr);

public slots:
    void manageServersWdg();
    void manageDownloadWdg();
    void manageSettingsWdg();
    void mapsDownloadFinished();
    void cleanupAndQuit();
    void toggleLaunchOnBoot(int state);
    void copyConnect(int row, int column);

private:
    void createActions();
    void createSysTray();
    void closeWithTimer(QWidget* wdg, int timeout);
    void getServersInfo();
    void setTableHeaders(QTableWidget* table, QStringList headers);
    void populateTable(QTableWidget* table, QStringList headers, QJsonArray arr);
    QString createTempFolder();
    QPlainTextEdit* createLogWin(QWidget* wdg);

    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
    QAction* serversAction;
    QAction* updateMapsAction;
    QAction* settingsAction;
    QAction* quitAction;

    QWidget* serversWdg = nullptr;
    QWidget* downloadWdg = nullptr;
    QWidget* settingsWdg = nullptr;

    QNetworkAccessManager networkManager;
    QNetworkReply* mapsDownloadReply = nullptr;

    QPlainTextEdit* logText = nullptr;

    QString tempPath = "";
    QString logFilePath = "";

    QJsonArray serversInfo;
    QTableWidget* serversTable;
};
