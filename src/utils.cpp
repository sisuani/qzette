/*
 *
 *
 *
 */

#include "utils.h"

#include "crypter.h"

QString Utils::encrypt(const QString &pwd)
{
    Crypter c;
    return c.encrypt(pwd);
}

QString Utils::desencrypt(const QString &encpwd)
{
    Crypter d;
    return d.desencrypt(encpwd);
}

double Utils::myRound(double value)
{
    int n = (int) (value * 10);

    double decimal = value - (double(n) / 10);

    if(decimal != 0.00 && decimal != 0.05)
	value += (decimal > 0.04) ? (0.1 - decimal) : (0.05 - decimal);

    return value;
}

QString Utils::numberToString(double number)
{

}
