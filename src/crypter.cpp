/*
 *
 *
 */

#include <QByteArray>

#include "crypter.h"

#define KEY "V!æN@T3oeL4D1G0Ctm1Nju"

Crypter::Crypter()
{
}

Crypter::~Crypter()
{
}

QString Crypter::encrypt(const QString &word)
{
    w = word;
    doSwap();
    doRot13();
    doXor();

    return w;
}

QString Crypter::desencrypt(const QString &word)
{
    w = word;
    doXor();
    doRot13();
    doSwap();

    return w;
}

void Crypter::doSwap()
{
    for(int i = 0; i < w.size(); i++) {
	switch(w[i].toAscii()) {
	    case '0':
		w[i] = '}';
		break;
	    case '}':
		w[i] = '0';
		break;
	    case '1':
		w[i] = '{';
		break;
	    case '{':
		w[i] = '1';
		break;
	}
    }
}

void Crypter::doRot13()
{
    for(int i = 0; i < w.size(); i++) {
	int r = do_rot13(w.at(i).toAscii());
	if(r) {
	    w[i] = QChar(r);
	}
    }
}

void Crypter::doXor()
{
    for(int i = 0; i < w.size(); i++) {
	int r = do_xor(w.at(i).toAscii(), i);
	if(r) {
	    w[i] = QChar(r);
	}
    }
}

int Crypter::do_rot13(int c)
{
    if(c <= 0) {
	return 0;
    }

    char r;
    if(c < 50 || c > 126 || (c > 93 && c < 97)) {
	return c;
    }

    return (c&64?(r=c&159)&&r<27?((r+12)%26+1)|(c&224):c:c);
}

int Crypter::do_move(int c, int pos)
{
    if(c <= 0) {
	return 0;
    }

    return c+pos;
}

int Crypter::do_xor(int c, int i)
{
    if(c <= 0) {
	return 0;
    }

    if(c < 50 || c > 126 || (c > 93 && c < 97)) {
	return c;
    }

    char r = c ^ KEY[i%strlen(KEY)];
    if(r < 50 || r > 126 || (r > 93 && r < 97)) {
	return c;
    }

    return r;
}
