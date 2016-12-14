#include "stringfunctions.h"

// Reads a line (String) from a BaseFile
Bool ReadLine(BaseFile *bf, String *v)
{
	Char ch,line[1024];
	Int32 i = 0;
	Int len = bf->TryReadBytes(&ch, 1);

	if (len == 0)
		return false; // end of file

	while (i<1024 && len == 1 && ch != '\n') 
	{
		line[i] = ch;
		len = bf->TryReadBytes(&ch, 1);
		i++;
	}
	v->SetCString(line, i);
	return true;
}


// WriteString function taken from PluginCafé.com
// Posted by Matthias Bober
Bool WriteString(const String line, BaseFile* file)
{
	if(!file) return false;

	Char *charline = nullptr;
	Int32 strlength = line.GetCStringLen(STRINGENCODING_7BITHEX);
	charline = (Char*)NewMemClear(Char, strlength+1);

	if(!charline) return false;

	strlength = line.GetCString(charline, strlength+1, STRINGENCODING_7BITHEX);

	Int32 i;
	for(i=0; i<strlength; i++) {
		if(!file->WriteChar(charline[i])) return false;
	}

	DeleteMem(charline);

	return true;
}


// Replace a Substring in a String
String ReplaceStr(String Original, String Search, String Replacement)
{
	String res = Original;
	Int32 i = 0;

	while (res.FindFirst(Search, &i, i))
	{
		res.Delete(i, Search.GetLength());
		res.Insert(i, Replacement);
	}

	return res;
}

// If Int32 < 10, put a "0" in front of value (looks nicer for time and date)
String LongToString2digits(Int32 l)
{
	if (l < 10)
		return "0" + String::IntToString(l);
	else
		return String::IntToString(l);
}


// Read complete contents (as text) from a BaseFile
String GetTextContent(BaseFile *source)
{
	if (!source) return String();

	String cs = String();
	String Content = String();
	Int length = source->GetLength();

	for (Int i = 0; i < length; i++)
	{
		if (ReadLine(source, &cs))
			Content = Content + cs;
	}

	return Content;
}


// Get String as Char Array (don't forget to free after use) and apply HTML encoding
Char* ConvString(const String& s)
{
	Char* code;
	Int32 len = s.GetCStringLen(STRINGENCODING_7BITHEX)+1;

	code = (Char*)NewMem(Char, len);
	if (code == nullptr) return nullptr;

	s.GetCString(code, len, STRINGENCODING_HTML);
	return code;
}


// Substitute special characters by their HTML entities and return a String
String ConvertHTMLchars(String tmpl)
{
	Char *html = ConvString(tmpl);
	if(!html) return String();

	String res = html;
	DeleteMem(html);

	return res;
}
