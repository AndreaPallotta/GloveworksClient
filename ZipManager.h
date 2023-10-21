#pragma once

#include "Path.h"
#include "Logger.h"

#include <QString>
#include <QPlainTextEdit>
#include <QDir>

class ZipManager
{
public:
	static bool UnzipFile(QPlainTextEdit* logText, const QString &zipPath, const QString &destPath);
};

