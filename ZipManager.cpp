#include "ZipManager.h"
#include "miniz/miniz.h"

bool ZipManager::UnzipFile(QPlainTextEdit* logText, const QString &zipPath, const QString &destPath)
{
	mz_zip_archive zipArchive;
	mz_bool status;

	mz_zip_zero_struct(&zipArchive);
	status = mz_zip_reader_init_file(&zipArchive, zipPath.toUtf8(), 0);

	if (!status)
	{
		Logger::appendLog(logText, ERROR_LOG, "Failed to open the zip file");
		return false;
	}

	bool folderCreated = Path::createDirIfNotExists(destPath);

	if (!folderCreated)
	{
		mz_zip_reader_end(&zipArchive);
		Logger::appendLog(logText, ERROR_LOG, "Failed to create output directory");
		return false;
	}

	for (mz_uint i = 0; i < mz_zip_reader_get_num_files(&zipArchive); ++i)
	{
		mz_zip_archive_file_stat fileStat;

		if (!mz_zip_reader_file_stat(&zipArchive, i, &fileStat))
		{
			Logger::appendLog(logText, ERROR_LOG, "Failed to get file stats for file #", std::to_string(i));
			continue;
		}

		QString extractedFilePath = Path::join(destPath, QString::fromUtf8(fileStat.m_filename));

		if (QString::fromUtf8(fileStat.m_filename).endsWith("/"))
		{
			Path::createDirIfNotExists(extractedFilePath);
			continue;
		}

		if (!mz_zip_reader_extract_to_file(&zipArchive, i, extractedFilePath.toUtf8().constData(), 0))
		{
			Logger::appendLog(logText, ERROR_LOG, "Failed to extract file #", std::to_string(i));
		}
	}

	mz_zip_reader_end(&zipArchive);
	return true;
}