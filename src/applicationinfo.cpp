/*
 *
 *
 */

#include "applicationinfo.h"

#include <QDir>
#include <QDateTime>
#include <QApplication>

#ifdef HAVE_CONFIG
#include "config.h"
#endif

#define PROG_NAME "qzette"
#define PROG_VERSION "0.0.1"

#if defined(Q_WS_X11) && !defined(QZETTE_DATADIR)
#define QZETTE_DATADIR "/usr/local/share/qzette"
#endif

QString ApplicationInfo::name()
{
    return PROG_NAME;
}

QString ApplicationInfo::version()
{
    return PROG_VERSION;
}

QString ApplicationInfo::build()
{
    QString build = "" __DATE__ "";
    build += " ";
    build += "" __TIME__ "";
    return build;
}

QString ApplicationInfo::resourcesDir()
{
#if defined(Q_WS_X11)
    return QZETTE_DATADIR;
#elif defined(Q_WS_WIN)
    return qApp->applicationDirPath();
#endif
}

QString ApplicationInfo::homeDir()
{
    return QDir::homePath();
}

QString ApplicationInfo::homePath()
{
    QString path = homeDir();
    path += QDir::separator();
    path += ".qzette";

    return path;
}

QString ApplicationInfo::configFilePath()
{
    QString path;
#if defined(Q_OS_UNIX)
    path = homePath();
#elif defined(Q_OS_WIN)
    path = qApp->applicationDirPath();
#endif
    path += QDir::separator();
    path += "qzette.conf";

    return path;
}

QString ApplicationInfo::logDirPath()
{
    QString path;
#if defined(Q_OS_UNIX)
    path = homePath();
#elif defined(Q_OS_WIN)
    path = qApp->applicationDirPath();
#endif
    path += QDir::separator();
    path += "log";

    return path;
}

QString ApplicationInfo::imagesAppPath()
{
    QString path = resourcesDir();
    path += QDir::separator();
    path += "images";
    path += QDir::separator();
    path += "app";

    return path;
}

QString ApplicationInfo::tmpDir()
{
    return QDir::tempPath();
}
