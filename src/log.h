/*
 *
 */

#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QString>
#include <QFile>

namespace Log
{

bool init();
void setLevel(int level);
int  getLevel();
void print(int nd, const QString &msg);
void warning(const QString &msg);
void error(const QString &msg);

};

#endif // LOG_H
