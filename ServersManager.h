#pragma once

#include "ssq.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

class ServersManager : public ssq::IServerResponse
{
public:
	QJsonArray serversArr;

	virtual void ServerInfo(ssq::gameserver_t& info)
	{
		QJsonObject obj;

		obj["Name"] = QString(info.name);
		obj["Map"] = QString(info.map);
		obj["Game"] = QString(info.game);
		obj["Players"] = QString(static_cast<char>(info.players) + "/" + static_cast<char>(info.maxplayers));
		obj["Version"] = QString(info.version);
		obj["Full"] = static_cast<int>(info.players) == static_cast<int>(info.maxplayers);
		obj["VAC"] = QString((info.vac) ? "True" : "False");
		obj["Port"] = QString(info.edf.port);
		obj["Tags"] = info.edf.has_tags() ? QString(info.edf.tags) : QString("");

		serversArr.append(obj);
	}

	virtual void ServerFailed() {}
};