#pragma once

#include "Logger.h"

#include <QString>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QPlainTextEdit>

class Path
{
public:
    static QString join(const QString& v) {
        return v;
    }

    template<typename... Args>
    static QString join(const QString& first, Args... args) {
        return QDir(first).filePath(join(args...));
    }

    static bool exists(const QString& path, bool isDir);
    static bool createDirIfNotExists(const QString& path);
    static bool moveFolderContent(QPlainTextEdit* logText, const QString& srcPath, const QString& destPath);
};