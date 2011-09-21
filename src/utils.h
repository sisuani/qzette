/*
 *
 *
 *
 */

#ifndef UTILS_H
#define UTILS_H

#include <QString>

class Utils
{
public:
    static QString encrypt(const QString &pwd);
    static QString desencrypt(const QString &pwd);
    static double myRound(double value);
    static QString numberToString(double number);
};

#endif /* UTILS_H */
