#include "GloveworksClient.h"

QUrl MAPS_URL("https://mygloveworks.com/fastdl/gloveworks-maps.zip");

GloveworksClient::GloveworksClient(QWidget* parent)
    : QMainWindow(parent)
{

    createActions();
    createSysTray();
    tempPath = RegistryManager::createTempFolder();
    trayIcon->show();
}

void GloveworksClient::createSysTray()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(connectAction);
    trayIconMenu->addAction(updateMapsAction);
    trayIconMenu->addAction(settingsAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    QIcon appIcon = QIcon(":/GloveworksClient/logo.ico");
    trayIcon->setIcon(appIcon);
}

void GloveworksClient::manageDownloadWdg()
{
    if (downloadWdg) delete downloadWdg;

    downloadWdg = new QWidget();
    downloadWdg->setMinimumWidth(400);

    logText = createLogWin(downloadWdg);

    downloadWdg->show();

    QVBoxLayout* layout = new QVBoxLayout(downloadWdg);
    layout->addWidget(logText);
    downloadWdg->setLayout(layout);

    Logger::appendLog(logText, INFO_LOG, "Downloading maps from " + MAPS_URL.toString().toStdString(), "<br />");

    mapsDownloadReply = networkManager.get(QNetworkRequest(QUrl(MAPS_URL)));
    connect(mapsDownloadReply, &QNetworkReply::finished, this, &GloveworksClient::mapsDownloadFinished);
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
    connectAction = new QAction(tr("&Connect"), this);
    connect(connectAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    updateMapsAction = new QAction(tr("&Update Maps"), this);
    connect(updateMapsAction, &QAction::triggered, this, &GloveworksClient::manageDownloadWdg);

    settingsAction = new QAction(tr("&Settings"), this);
    connect(settingsAction, &QAction::triggered, qApp, &QCoreApplication::quit);

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