#include "Path.h"

bool Path::exists(const QString& path, bool isDir = true)
{
    if (isDir)
    {
        QDir dir(path);
        return dir.exists();
    }
    else {
        return QFile::exists(path);
    }
}

bool Path::createDirIfNotExists(const QString& path)
{
    bool exists = Path::exists(path);

    if (exists) return true;

    QDir dir(path);

    if (!dir.mkpath("."))
    {
        return false;
    }

    return true;
}

bool Path::moveFolderContent(QPlainTextEdit* logText, const QString& srcPath, const QString& destPath)
{
    QDir srcDir(srcPath);
    QDir destDir(destPath);

    if (!Path::exists(srcPath) || !srcDir.isReadable()) return false;

    Path::createDirIfNotExists(destPath);

    QStringList contentList = srcDir.entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
    int successCount = 0;

    for (const QString& entry : contentList)
    {
        QString srcFilePath = srcDir.filePath(entry);
        QString destFilePath = destDir.filePath(entry);

        if (QFile::exists(destFilePath))
        {
            QFile::remove(destFilePath);
        }

        if (!QFile::rename(srcFilePath, destFilePath))
        {
            Logger::appendLog(logText, ERROR_LOG, "Failed moving file ", entry.toStdString());
        }
        else
        {
            Logger::appendLog(logText, SUCCESS_LOG, "Moved file ", entry.toStdString());
            successCount++;
        }
    }

    Logger::appendLog(logText, successCount > 0 ? SUCCESS_LOG : WARNING_LOG, "Files moved: ", std::to_string(successCount), "/", std::to_string(contentList.size()));

    return true;
}