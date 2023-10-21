#pragma once

#include <QString>
#include <Windows.h>

class RegistryManager
{
public:
    static QString readString(const HKEY root, const std::wstring& subKey, const std::wstring& valueName);
    static QString createTempFolder();
    static QString getSteamPath();
    static void deleteFolder(const QString& path);
};

