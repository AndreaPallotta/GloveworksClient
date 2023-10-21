#include "RegistryManager.h"

#include <QSettings>
#include <QDir>
#include <QUuid>

QString RegistryManager::readString(const HKEY hKey, const std::wstring& subKey, const std::wstring& valueName)
{
    size_t bufferSize = 0xFFF;
    std::wstring valueBuf;
    valueBuf.resize(bufferSize);
    DWORD cbData = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
    LSTATUS rc = RegGetValueW(
        hKey,
        subKey.c_str(),
        valueName.c_str(),
        RRF_RT_REG_SZ,
        nullptr,
        static_cast<void*>(valueBuf.data()),
        &cbData
    );

    if (rc != ERROR_SUCCESS)
    {
        return QString();
    }

    cbData /= sizeof(wchar_t);
    valueBuf.resize(static_cast<size_t>(cbData - 1));
    return QString::fromStdWString(valueBuf);
}

QString RegistryManager::createTempFolder()
{
    QString tempFolder = readString(HKEY_CURRENT_USER, L"Environment\\", L"TMP");

    if (tempFolder.isEmpty()) return "ERR NOT FOUND";

    QUuid uuid = QUuid::createUuid();
    QString tempSubDir = tempFolder + "\\" + QString::fromUtf8(uuid.toRfc4122().toHex());

    QDir tempDir;
    if (tempDir.mkpath(tempSubDir))
    {
        return tempSubDir;
    }
    else
    {
        return "ERR NOT CREATED";
    }
}

QString RegistryManager::getSteamPath()
{
    QString steamPath = readString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", L"InstallPath");

    if (steamPath.isEmpty())
    {
        steamPath = readString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam\\", L"InstallPath");
    }

    return steamPath;
}

void RegistryManager::deleteFolder(const QString& path)
{
    QDir dir(path);
    if (dir.exists())
    {
        dir.removeRecursively();
    }
}
