/*
 *
 *
 *
 *
 */

#ifndef NUMBERTOWORD_H
#define NUMBERTOWORD_H

#include <string>

using namespace std;

class NumberToWord
{
public:
    NumberToWord();

    static NumberToWord *instance();
    static void cleanup();

    const string numberToWord(const string &number);

private:
    static NumberToWord *instance_;

    string numToWord(const string &number);
    string convertToWord(const string &number);

    string digitZero(char c);
    string digitOne(char c, bool iscero);
    string digitOneOne(char c);
    string digitTwo(char c, bool iscero);
    string digitThree(char c);
};

#endif // NUMBERTOWORD_H
