/*
 *
 *
 *
 */

#include "numbertoword.h"

NumberToWord::NumberToWord()
{
}

NumberToWord *NumberToWord::instance_ = 0;

NumberToWord *NumberToWord::instance()
{
    if(!instance_)
	instance_ = new NumberToWord();
    return instance_;
}

void NumberToWord::cleanup()
{
    if(instance_) {
	delete instance_;
	instance_ = 0;
    }
}

const string NumberToWord::numberToWord(const string &num)
{

    string retString, number = num;
    int wz = 0;
    for(unsigned int i = 0; i < number.length(); i++) {
	if(number.at(i) != '0')
	    break;
	++wz;
    }
    if(wz)
	number.erase(0, wz);
    int pos = number.find(",");
    string firstPartNumber, lastPartNumber;
    if(pos > 0)
	firstPartNumber = number.substr(0, pos);
    else
	firstPartNumber = number;

    if(pos > 0)
	lastPartNumber = number.substr(pos + 1, number.length());

    retString = numToWord(firstPartNumber);
    if(pos > 0) {
	retString += "con ";
	retString += numToWord(lastPartNumber);
    }

    return retString;
}

string NumberToWord::numToWord(const string &number)
{
    string retString;
    int length = number.length();
    if(length > 12)
	return "error l > 12";

    if(length > 6) {
	retString = convertToWord(number.substr(0, length - 6));
	if(length - 6 > 1)
	    retString += "millones ";
	else
	    retString += number.at(0) == '1' ? "millon " : "millones ";
	retString += convertToWord(number.substr(length - 6, length));
    } else {
	retString = convertToWord(number);
    }
    return retString;
}

string NumberToWord::convertToWord(const string &number)
{
    string retString = "";
    int length = number.length();
    bool appendNext = true;
    for(int i = 0; i < length; i++) {
	if(!appendNext) {
	    appendNext = true;
	    continue;
	}
	int pos = length - i - 1;
	if(pos == 0 && number.at(i) != '0') {
	    retString += digitZero(number.at(i));
	} else if(pos == 1) {
	    if(number.at(i) == '1') {
		retString += digitOneOne(number.at(i+1));
		appendNext = false;
	    } else {
		retString += digitOne(number.at(i), number.at(i+1) == '0');
	    }
	} else if(pos == 2) {
	    retString += digitTwo(number.at(i), number.at(i+1) == '0' && number.at(i+2) == '0');
	} else if(pos == 3) {
	    retString += digitThree(number.at(i));
	} else if(pos == 4) {
	    if(number.at(i) == '1') {
		retString += digitOneOne(number.at(i+1)) + "mil ";
		appendNext = false;
	    } else {
		retString += digitOne(number.at(i), number.at(i+1) == '0');
	    }
	} else if(pos == 5) {
	    retString += digitTwo(number.at(i), number.at(i+1) == '0' && number.at(i+2) == '0');
	}
    }
    return retString;

}

string NumberToWord::digitZero(char c)
{
    switch(c) {
	case '0':
	  return "cero ";
	case '1':
	  return "uno ";
	case '2':
	  return "dos ";
	case '3':
	  return "tres ";
	case '4':
	  return "cuatro ";
	case '5':
	  return "cinco ";
	case '6':
	  return "seis ";
	case '7':
	  return "siete ";
	case '8':
	  return "ocho ";
	case '9':
	  return "nueve ";
    }
    return "";
}

string NumberToWord::digitOne(char c, bool iscero)
{
    switch(c) {
	case '2':
	  if(iscero)
	      return "veinte ";
	  else
	      return "veinti";
	case '3':
	  if(iscero)
	      return "treinta ";
	  else
	      return "treinta y ";
	case '4':
	  if(iscero)
	      return "cuarenta ";
	  else
	      return "cuarenta y ";
	case '5':
	  if(iscero)
	      return "cincuenta ";
	  else
	      return "cincuenta y ";
	case '6':
	  if(iscero)
	      return "sesenta ";
	  else
	      return "sesenta y ";
	case '7':
	  if(iscero)
	      return "setenta ";
	  else
	      return "setenta y ";
	case '8':
	  if(iscero)
	      return "ochenta ";
	  else
	      return "ochenta y ";
	case '9':
	  if(iscero)
	      return "noventa ";
	  else
	      return "noventa y ";
    }
    return "";
}

string NumberToWord::digitOneOne(char c)
{
    switch(c) {
	case '0':
	  return "diez ";
	case '1':
	  return "once ";
	case '2':
	  return "doce ";
	case '3':
	  return "trece ";
	case '4':
	  return "catorce ";
	case '5':
	  return "quince ";
	case '6':
	  return "dieci" + digitZero('6') + " ";
	case '7':
	  return "dieci" + digitZero('7') + " ";
	case '8':
	  return "dieci" + digitZero('8') + " ";
	case '9':
	  return "dieci" + digitZero('9') + " ";
    }
    return "";
}

string NumberToWord::digitTwo(char c, bool iscero)
{
    switch(c) {
	case '1':
	  if(iscero)
	      return "cien ";
	  else
	      return "ciento ";
	case '2':
	  return "doscientos ";
	case '3':
	  return "trecientos ";
	case '4':
	  return "cuatrocientos ";
	case '5':
	  return "quinientos ";
	case '6':
	  return "seiscientos ";
	case '7':
	  return "setecientos ";
	case '8':
	  return "ochocientos ";
	case '9':
	  return "novecientos ";
    }
    return "";
}

string NumberToWord::digitThree(char c)
{
    string n = "";
    if(c > '1')
	n += digitZero(c);
    n += "mil ";
    return n;
}
