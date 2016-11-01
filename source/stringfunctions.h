#include "c4d.h"

#ifndef __COMMON_STRING_FUNCTIONS__
#define __COMMON_STRING_FUNCTIONS__

Bool ReadLine(BaseFile *bf, String *v);
Bool WriteString(const String line, BaseFile* file);
String ReplaceStr(String Original, String Search, String Replacement);
String LongToString2digits(LONG l);
String GetTextContent(BaseFile *source);
CHAR* ConvString(const String& s);
String ConvertHTMLchars(String tmpl);

#endif
