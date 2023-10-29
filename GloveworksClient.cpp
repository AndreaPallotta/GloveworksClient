#include "GloveworksClient.h"
#include "ssq.h"
#include <QUdpSocket>
#include <QByteArray>
#include <QNetworkDatagram>
#include <QCoreApplication>
#include <QGuiApplication>

QUrl MAPS_URL("https://mygloveworks.com/fastdl/gloveworks-maps.zip");


GloveworksClient::GloveworksClient(QWidget* parent)
    : QMainWindow(parent)
{

    createActions();
    createSysTray();
    //tempPath = createTempFolder();
    trayIcon->show();

    serversInfo = QJsonArray();

    QGuiApplication::setQuitOnLastWindowClosed(false);
}

void GloveworksClient::createSysTray()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(serversAction);
    trayIconMenu->addAction(updateMapsAction);
    trayIconMenu->addAction(settingsAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    QIcon appIcon = QIcon(":/GloveworksClient/logo.ico");
    trayIcon->setIcon(appIcon);
}

void GloveworksClient::manageServersWdg()
{
    if (serversWdg) delete serversWdg;

    serversWdg = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(serversWdg);

    QTableWidget* serversTable = new QTableWidget(serversWdg);
    serversTable->setRowCount(0);
    serversTable->setColumnCount(8);

    serversTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    serversWdg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    serversTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
    serversTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Map"));
    serversTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Game"));
    serversTable->setHorizontalHeaderItem(3, new QTableWidgetItem("Version"));
    serversTable->setHorizontalHeaderItem(4, new QTableWidgetItem("Players"));
    serversTable->setHorizontalHeaderItem(5, new QTableWidgetItem("VAC"));
    serversTable->setHorizontalHeaderItem(6, new QTableWidgetItem("Tags"));
    serversTable->setHorizontalHeaderItem(7, new QTableWidgetItem("Connect"));

    layout->addWidget(serversTable);

    serversTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //connect(serversWdg, &QWidget::showEvent, this, &GloveworksClient::getServersInfo);

    qDebug() << serversInfo;

    serversWdg->showMaximized();
    serversWdg->setLayout(layout);
}

void GloveworksClient::manageDownloadWdg()
{
    if (downloadWdg) delete downloadWdg;

    downloadWdg = new QWidget();
    logText = createLogWin(downloadWdg);
    QPushButton* closeBtn = new QPushButton("Close", downloadWdg);
    QVBoxLayout* layout = new QVBoxLayout(downloadWdg);

    downloadWdg->setMinimumWidth(500);
    downloadWdg->show();

    layout->addWidget(logText);
    layout->addWidget(closeBtn);
    downloadWdg->setLayout(layout);

    Logger::appendLog(logText, INFO_LOG, "Downloading maps from " + MAPS_URL.toString().toStdString(), "<br />");

    mapsDownloadReply = networkManager.get(QNetworkRequest(QUrl(MAPS_URL)));
    connect(mapsDownloadReply, &QNetworkReply::finished, this, &GloveworksClient::mapsDownloadFinished);
    connect(closeBtn, &QPushButton::clicked, this, [this] { downloadWdg->close(); });
}

void GloveworksClient::manageSettingsWdg()
{
    if (settingsWdg) delete settingsWdg;

    settingsWdg = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(settingsWdg);
    QPushButton* closeBtn = new QPushButton("Close", settingsWdg);
    QCheckBox* onBootCheckbox = new QCheckBox("En&able on boot");

    settingsWdg->setFixedWidth(300);
    settingsWdg->show();

    layout->addWidget(onBootCheckbox);
    layout->addWidget(closeBtn);
    settingsWdg->setLayout(layout);
    
    connect(closeBtn, &QPushButton::clicked, this, [this] { settingsWdg->close(); });
}

void GloveworksClient::mapsDownloadFinished()
{

    if (!mapsDownloadReply || !logText)
    {
        Logger::appendLog(logText, INFO_LOG, "Initializing download... ");
        return;
    }

    if (mapsDownloadReply->error() != QNetworkReply::NoError)
    {
        Logger::appendLog(logText, ERROR_LOG, mapsDownloadReply->errorString().toStdString());
        mapsDownloadReply->deleteLater();
        return;
    }

    QByteArray data = mapsDownloadReply->readAll();
    QString filePath = Path::join(tempPath, "gloveworks-maps.zip");

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly))
    {
        Logger::appendLog(logText, ERROR_LOG, "Failed to save the downloaded file.");
        return;
    }

    file.write(data);
    file.close();
    Logger::appendLog(logText, SUCCESS_LOG, "Maps downloaded to: " + filePath.toStdString());

    QString unzipPath = Path::join(tempPath, "gloveworks-maps");
    bool unzipState = ZipManager::UnzipFile(logText, filePath, unzipPath);

    if (!unzipState)
    {
        Logger::appendLog(logText, ERROR_LOG, "Failed to unzip maps");
        return;
    }

    Logger::appendLog(logText, SUCCESS_LOG, "Unzip'd maps to: " + unzipPath.toStdString());

    QString steamPath = RegistryManager::getSteamPath();

    if (steamPath.isEmpty())
    {
        Logger::appendLog(logText, ERROR_LOG, "Cannot find Steam path");
        return;
    }

    QString destPath = Path::join(steamPath, "steamapps", "common", "Counter-Strike Global Offensive", "game", "csgo", "maps");
    Logger::appendLog(logText, INFO_LOG, "Moving content from ", unzipPath.toStdString(), " to ", destPath.toStdString(), "<br />");

    if (!Path::moveFolderContent(logText, Path::join(unzipPath, "maps"), destPath))
    {
        Logger::appendLog(logText, ERROR_LOG, "Failed to move files");
    }

    Logger::appendLog(logText, INFO_LOG, "Successfully downloaded maps.Closing in 10 seconds...");
    closeWithTimer(downloadWdg, 10000);
}

QPlainTextEdit* GloveworksClient::createLogWin(QWidget* wdg)
{
    QPlainTextEdit* pte = new QPlainTextEdit(downloadWdg);
    pte->setReadOnly(true);
    pte->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    pte->setCenterOnScroll(true);
    pte->setFixedHeight(300);

    return pte;
}

void GloveworksClient::createActions()
{
    serversAction = new QAction(tr("&Servers"), this);
    connect(serversAction, &QAction::triggered, this, &GloveworksClient::manageServersWdg);

    updateMapsAction = new QAction(tr("Update &Maps"), this);
    connect(updateMapsAction, &QAction::triggered, this, &GloveworksClient::manageDownloadWdg);

    settingsAction = new QAction(tr("S&ettings"), this);
    connect(settingsAction, &QAction::triggered, this, &GloveworksClient::manageSettingsWdg);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, this, &GloveworksClient::cleanupAndQuit);
}

void GloveworksClient::cleanupAndQuit()
{
    RegistryManager::deleteFolder(tempPath);
    qApp->quit();
}

void GloveworksClient::closeWithTimer(QWidget* wdg, int timeout)
{
    QTimer* delayTimer = new QTimer(wdg);
    delayTimer->setSingleShot(true);
    delayTimer->start(timeout);

    connect(delayTimer, &QTimer::timeout, [=]() {
        wdg->close();
    });
}

void GloveworksClient::toggleLaunchOnBoot(int state)
{
    QString appName("gloveworksClient.exe");

    if (!Path::exists(appName, false)) return;

    QString userName = QDir::home().dirName();
    QString appNameLink = Path::join(appName, ".lnk");
    QString startupLink = Path::join("C:", "Users", userName, "AppData", "Roaming", "Microsoft", "Start Menu", "Programs", "Startup", appNameLink);

    if (state == Qt::Checked) {
        QFile::link(appName, appNameLink);

        QFile::copy(appNameLink, startupLink);
    }
    else if (state == Qt::Unchecked)
    {
        QFile::remove(startupLink);
    }
}

void GloveworksClient::getServersInfo()
{
    // TODO: Read servers from a file
    QStringList *servers = new QStringList();
    servers->append("15.235.181.104:27015");

    unsigned long timeout = 5000;
    ServersManager res;
    ssq::IQuery* query;

    for (const QString& server : *servers)
    {
        query = ssq::ServerInfo(&res);
        if (!query->Connect(server.toUtf8(), 0, timeout) || !query->Perform())
        {
            delete query;
            continue;
        }

        delete query;
    }

    serversInfo = res.serversArr;
}

QString GloveworksClient::createTempFolder()
{
    QString tempFolder = RegistryManager::readString(HKEY_CURRENT_USER, L"Environment\\", L"TMP");

    if (tempFolder.isEmpty()) return "ERR NOT FOUND";

    QUuid uuid = QUuid::createUuid();
    QString tempSubDir = Path::join(tempFolder,"\\",QString::fromUtf8(uuid.toRfc4122().toHex()));

    QDir tempDir;
    if (tempDir.mkpath(tempSubDir))
    {
        return tempSubDir;
    }
    else
    {
        return "Failed to create temp folder";
    }
}