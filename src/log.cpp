/*
 *
 *
 *
 */

#include <QCoreApplication>
#include <QTextStream>
#include <QDateTime>
#include <QDir>

#include "log.h"
#include "applicationinfo.h"

#if defined(Q_OS_UNIX)
#define nline "\n"
#elif defined(Q_OS_WIN)
#define nline "\r\n"
#endif

namespace Log
{

QFile *logFile = 0;
int level = 1;

bool init()
{
    QString path = ApplicationInfo::logDirPath();
    path += QDir::separator();
    path += ApplicationInfo::name();
    path += ".log";

    logFile = new QFile(path);
    if(!logFile->open(QIODevice::Append)) {
        logFile->open(stderr, QIODevice::WriteOnly);
    }

    if(!logFile) {
        return false;
    }

    return true;
}

void print(int debug, const QString &msg)
{
    if(debug <= level) {
        QTextStream out(logFile);
        out << "( "
	    << QDateTime::currentDateTime().toString(Qt::ISODate)
	    << " ) : "
	    << msg
	    << nline;
    }
}

void warning(const QString &msg)
{
    QTextStream out(logFile);
    out << "WARNING => ( "
	<< QDateTime::currentDateTime().toString(Qt::ISODate)
	<< " ) : "
	<< msg
	<< nline;
}

void error(const QString &msg)
{
    QTextStream out(logFile);
    out << "ERROR => ( "
	<< QDateTime::currentDateTime().toString(Qt::ISODate)
	<< " ) : "
	<< msg
	<< nline;
}

void setLevel(int v_level)
{
    if(v_level > 0) {
	level = v_level;
    }
}

int getLevel()
{
    return level;
}

}
; // namespace Log
