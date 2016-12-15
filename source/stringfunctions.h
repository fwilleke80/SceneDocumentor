#ifndef STRINGFUNCTIONS_H_
#define STRINGFUNCTIONS_H_

#include "c4d.h"


Bool ReadLine(BaseFile *bf, String *v);
Bool WriteString(const String line, BaseFile* file);
String ReplaceStr(String Original, String Search, String Replacement);
String LongToString2digits(Int32 l);
String GetTextContent(BaseFile *source);
Char* ConvString(const String& s);
String ConvertHTMLchars(String tmpl);

#endif /* STRINGFUNCTIONS_H_ */
