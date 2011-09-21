/*
 *
 *
 */


#ifndef CRYPTER_H
#define CRYPTER_H

#include <QObject>

class Crypter : public QObject
{
    Q_OBJECT

public:
    Crypter();
    ~Crypter();

    QString encrypt(const QString &enc);
    QString desencrypt(const QString &denc);

private:
    void doXor();
    void doRot13();
    void doSwap();

    //Utils
    int do_rot13(int c);
    int do_move(int c, int pos);
    int do_xor(int c, int pos);

    QString w;
};

#endif /* CRYPTER_H */
