
/*
 *
 *
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings
{

public:
    Settings();

    static Settings *instance();
    static void cleanup();

    bool isValid();

    const QString &getHostName() const;
    const QString &getDatabaseName() const;
    const QString &getUserName() const;
    const QString &getPassword() const;

    const int &getMinimumPasswordLength() const;
    const int &getLogLevel() const;

    bool toolBarToolsVisible();
    bool toolBarActionsVisible();

private:
    static Settings *instance_;

    QString hostName;
    QString databaseName;
    QString userName;
    QString password;

    int minimumPasswordLength;
    int logLevel;

    bool m_toolBarToolsVisible;
    bool m_toolBarActionsVisible;

    bool valid;
};

#endif // SETTINGS_H
