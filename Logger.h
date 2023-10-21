#pragma once

#include <QString>
#include <QPlainTextEdit>

enum LOG_TYPE
{
    SUCCESS_LOG,
    WARNING_LOG,
    ERROR_LOG,
    INFO_LOG
};

class Logger
{
public:
    static QString toQString(const std::string& str)
    {
        return QString::fromStdString(str);
    }

    template<typename... Args>
    static QString toQString(const std::string& str, Args... args)
    {
        return QString::fromStdString(str) + toQString(args...);
    }
    template<typename... Args>
    static QString toErrLog(const std::string& str, Args... args)
    {
        return "> <font style='color: red; font-weight: bold;'> ERROR: " + toQString(str, args...) + "</font>";
    }
    template<typename... Args>
    static QString toSuccessLog(const std::string& str, Args... args)
    {
        return "> <font style='color: green; font-weight: bold;'> SUCCESS: " + toQString(str, args...) + "</font>";
    }

    template<typename... Args>
    static QString toWarningLog(const std::string& str, Args... args)
    {
        return "> <font style='color: gold; font-weight: bold;'> WARNING: " + toQString(str, args...) + "</font>";
    }

    template<typename... Args>
    static void appendLog(QPlainTextEdit* pte, LOG_TYPE type, const std::string& str, Args... args)
    {
        QString logString;
        switch (type)
        {
        case SUCCESS_LOG:
            logString = toSuccessLog(str, args...);
            break;
        case WARNING_LOG:
            logString = toWarningLog(str, args...);
            break;
        case ERROR_LOG:
            logString = toErrLog(str, args...);
            break;
        case INFO_LOG:
            logString = toQString("> INFO: ", str, args...);
            break;
        default:
            return;
        }

        pte->appendHtml(logString);
    }
};