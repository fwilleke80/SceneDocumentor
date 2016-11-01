#include "stringfunctions.h"

// Reads a line (String) from a BaseFile
Bool ReadLine(BaseFile *bf, String *v)
{
	CHAR ch,line[1024];
	LONG i = 0, len = bf->TryReadBytes(&ch, 1);

	if (len == 0)
		return FALSE; // end of file

	while (i<1024 && len == 1 && ch != '\n') 
	{
		line[i] = ch;
		len = bf->TryReadBytes(&ch, 1);
		i++;
	}
	v->SetCString(line, i);
	return TRUE;
}


// WriteString function taken from PluginCafé.com
// Posted by Matthias Bober
Bool WriteString(const String line, BaseFile* file)
{
	if(!file) return FALSE;

	CHAR *charline = NULL;
	LONG strlength = line.GetCStringLen(STRINGENCODING_7BITHEX);
	charline = (CHAR*)GeAlloc(strlength+1);

	if(!charline) return FALSE;

	strlength = line.GetCString(charline, strlength+1, STRINGENCODING_7BITHEX);

	LONG i;
	for(i=0; i<strlength; i++) {
		if(!file->WriteChar(charline[i])) return FALSE;
	}

	GeFree(charline);

	return TRUE;
}


// Replace a Substring in a String
String ReplaceStr(String Original, String Search, String Replacement)
{
	String res = Original;
	LONG i = 0;

	while (res.FindFirst(Search, &i, i))
	{
		res.Delete(i, Search.GetLength());
		res.Insert(i, Replacement);
	}

	return res;
}

// If Long < 10, put a "0" in front of value (looks nicer for time and date)
String LongToString2digits(LONG l)
{
	if (l < 10)
		return "0" + LongToString(l);
	else
		return LongToString(l);
}


// Read complete contents (as text) from a BaseFile
String GetTextContent(BaseFile *source)
{
	if (!source) return String();

	String cs = String();
	String Content = String();
	VLONG length = source->GetLength();

	for (VLONG i = 0; i < length; i++)
	{
		if (ReadLine(source, &cs))
			Content = Content + cs;
	}

	return Content;
}


// Get String as Char Array (don't forget to free after use) and apply HTML encoding
CHAR* ConvString(const String& s)
{
	CHAR* code;
	LONG len = s.GetCStringLen(STRINGENCODING_7BITHEX)+1;

	code = (CHAR*)GeAllocNC(len);
	if (code == NULL) return NULL;

	s.GetCString(code, len, STRINGENCODING_HTML);
	return code;
}


// Substitute special characters by their HTML entities and return a String
String ConvertHTMLchars(String tmpl)
{
	CHAR *html = ConvString(tmpl);
	if(!html) return String();

	String res = html;
	GeFree(html);

	return res;
}
