/*
 *
 *
 */

#include "settings.h"

#include <QSettings>
#include <QFile>

#include "log.h"
#include "utils.h"
#include "applicationinfo.h"
#include "common.h"

Settings::Settings()
{
    valid = true;

    if(!QFile::exists(ApplicationInfo::configFilePath())) {
	Log::error(ApplicationInfo::configFilePath() + " file not found.");
	valid = false;
	return;
    }

    QSettings *s;
    s = new QSettings(ApplicationInfo::configFilePath(), QSettings::IniFormat);

    s->beginReadArray("General");
    {
	logLevel = s->value("LogLevel").toInt();
    }
    s->endArray();

    s->beginReadArray("Database");
    {
	hostName = s->value("hostName").toString();
	databaseName = s->value("databaseName").toString();
	userName = s->value("userName").toString();
	password = Utils::desencrypt(s->value("password").toString());
    }
    s->endArray();

    s->beginReadArray("Main");
    {
	minimumPasswordLength = s->value("minimumPasswordLength").toInt();
	m_toolBarToolsVisible = s->value("ToolBarToolsVisible").toBool();
	m_toolBarActionsVisible = s->value("ToolBarActionsVisible").toBool();
    }
    s->endArray();

    delete s;
}

Settings *Settings::instance_ = 0;

Settings *Settings::instance()
{
    if(!instance_) {
	instance_ = new Settings();
	Log::print(3, "Settings new instance()");
    }

    return instance_;
}

void Settings::cleanup()
{
    if(instance_) {
	delete instance_;
	instance_ = 0;
    }

    Log::print(3, "Settings::cleanup()");
}

bool Settings::isValid()
{
    return valid;
}

const QString &Settings::getHostName() const
{
    return hostName;
}

const QString &Settings::getDatabaseName() const
{
    return databaseName;
}

const QString &Settings::getUserName() const
{
    return userName;
}

const QString &Settings::getPassword() const
{
    return password;
}

const int &Settings::getMinimumPasswordLength() const
{
    return minimumPasswordLength;
}

const int &Settings::getLogLevel() const
{
    return logLevel;
}

bool Settings::toolBarToolsVisible()
{
    return m_toolBarToolsVisible;
}

bool Settings::toolBarActionsVisible()
{
    return m_toolBarActionsVisible;
}
