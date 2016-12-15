#include "c4d.h"
#include "c4d_symbols.h" 
#include "removealltags.h"
#include "customgui_datetime.h"
#include "tCommentClass.h"
#include "tComment.h"
#include "stringfunctions.h"
#include "documentorfunctions.h"
#include "commands.h"
#include "main.h"


static const Int32 ID_CREATEDOCUMENT = 1024748;		// ID obtained from www.plugincafe.com

// Template path
#define TmplDir GeGetPluginPath() + "template"

// R11 render engine options
// (Hack, to make it compile in R10)
#ifndef RDATA_RENDERENGINE
#define RDATA_RENDERENGINE	5300
#define RDATA_RENDERENGINE_PREVIEWSOFTWARE	1
#endif

// R10 render engine options
// (Hack, to make it compile in R11)
#ifndef RDATA_RENDERASEDITOR
#define RDATA_RENDERASEDITOR	5300
#endif

/////////////////////////////////////////
// Dialog class & element declaration
/////////////////////////////////////////
class DocCreationDialog : public GeDialog
{
	INSTANCEOF(DocCreationDialog,GeDialog)

private:
	Bool CreateDocument(BaseDocument *doc, BaseContainer *settings);

public:
	DocCreationDialog(void);
	virtual Bool CreateLayout(void);
	virtual Bool InitValues(void);
	virtual Bool Command(Int32 id, const BaseContainer &msg);
	virtual Int32 Message(const BaseContainer &msg, BaseContainer &result);
	virtual Bool CoreMessage(Int32 id, const BaseContainer &msg);
};


// Dialog elements
enum
{
	IDC_CDOC_INFOTEXT = 10000,
	IDC_CDOC_FILENAME,
	IDC_CDOC_FILENAME_CHOOSE,

	IDC_CDOC_TITLE,
	IDC_CDOC_INTROTEXT,
	IDC_CDOC_AUTHOR,
	IDC_CDOC_TMPLDIR,

	IDC_CDOC_IMAGE,
	IDC_CDOC_IMAGE_EDITOR,
	IDC_CDOC_IMAGE_RENDER,
	IDC_CDOC_IMAGE_WIDTH,
	IDC_CDOC_IMAGE_HEIGHT,

	IDC_CDOC_OPENDOCUMENT,

	IDC_CDOC_GENERATE,

	IDC_CDOC_HELP,

	IDC_MEMORY_STAT_
};

/////////////////////////////////////////
// General functions
/////////////////////////////////////////
static void ShowProgress(Float p, RENDERPROGRESSTYPE progress_type, void *private_data)
// Hook into RenderDocument() to show progress in status bar (doesn't seem to work as expected)
{
	StatusSetBar((Int32)(p * 100));
}

String ProcessTemplate(String tmpl, BaseDocument *doc, BaseContainer *settings)
// Substitute placeholders
{
	String res = tmpl;

	// Data from settings container
	String title = settings->GetString(IDC_CDOC_TITLE);
	String intro = settings->GetString(IDC_CDOC_INTROTEXT);
	String author = settings->GetString(IDC_CDOC_AUTHOR);
	Int32 imagesetting = settings->GetInt32(IDC_CDOC_IMAGE, IDC_CDOC_IMAGE_EDITOR);

	// If Image placeholder used in template, create image
	Int32 x = 0;
	if (res.FindFirst("%DOC_IMAGE%", &x))
	{
		RenderData *rd = nullptr;

		// Create Filename
		Filename ifn = Filename(settings->GetString(IDC_CDOC_FILENAME));
		ifn.SetSuffix("jpg");

		// Image resolution
		Int32 imgWidth = settings->GetInt32(IDC_CDOC_IMAGE_WIDTH);
		Int32 imgHeight = settings->GetInt32(IDC_CDOC_IMAGE_HEIGHT);

		// Allocate & init bitmap
		BaseBitmap *bmp = BaseBitmap::Alloc();
		if (!bmp) goto AfterBitmap;
		bmp->Init(imgWidth, imgHeight);

		// Get Render Data
		rd = doc->GetActiveRenderData();
		if (!rd) goto AfterBitmap;

		// Get Copy from & modify Render Data
		BaseContainer rdc = rd->GetData();
		rdc.SetInt32(RDATA_XRES, imgWidth);
		rdc.SetInt32(RDATA_YRES, imgHeight);

		// Set editor Rendering, if desired
		if (imagesetting == IDC_CDOC_IMAGE_EDITOR)
		{
			rdc.SetInt32(RDATA_RENDERENGINE, RDATA_RENDERENGINE_PREVIEWSOFTWARE);	// R11
			rdc.SetInt32(RDATA_RENDERASEDITOR, 1);									// R10
		}

		// Render image
		StatusSetText(GeLoadString(IDS_RENDERIMAGE_RUNNING));
		RENDERRESULT renderres = RenderDocument(doc, rdc, ShowProgress, nullptr, bmp, RENDERFLAGS_EXTERNAL, nullptr);
		if (renderres == RENDERRESULT_OK)
		{
			BaseContainer imgdata;
			imgdata.SetInt32(JPGSAVER_QUALITY, 90);
			imgdata.SetInt32(IMAGESAVER_DPI, 96);
			IMAGERESULT Result = bmp->Save(ifn, FILTER_JPG, &imgdata, SAVEBIT_0);
			if (Result != IMAGERESULT_OK)
				GeOutString(GeLoadString(IDS_RENDERIMAGE_ERRORMSG), GEMB_OK);
			StatusClear();
			res = ReplaceStr(res, "%DOC_IMAGE%", "<img class=\"rendering\" src=\"" + ifn.GetFile().GetString() + "\" alt=\"Rendering\" title=\"Rendering\" width=\"" + String::IntToString(imgWidth) + "\" height=\"" + String::IntToString(imgHeight) + "\" />");
		}
		else
		{
			String rendermsg = String();
			switch (renderres)
			{
				case RENDERRESULT_OUTOFMEMORY:
					{
						rendermsg = GeLoadString(IDS_RENDERIMAGE_MSG_NOMEM);
						break;
					}
				case RENDERRESULT_ASSETMISSING:
					{
						rendermsg = GeLoadString(IDS_RENDERIMAGE_MSG_TEXMISSING);
						break;
					}
				case RENDERRESULT_USERBREAK:
					{
						rendermsg = GeLoadString(IDS_RENDERIMAGE_MSG_USERBREAK);
						break;
					}
			}
			res = ReplaceStr(res, "%DOC_IMAGE%", "<b>" + GeLoadString(IDS_RENDERIMAGE_HTML_NOIMAGE, rendermsg) + "</b>");
		}

		// Free Bitmap
		if (bmp) BaseBitmap::Free(bmp);
	}

AfterBitmap:
	// Text placeholders
	res = ReplaceStr(res, "%DOC_TITLE%", title);
	res = ReplaceStr(res, "%DOC_FILENAME%", doc->GetDocumentName().GetString());
	res = ReplaceStr(res, "%DOC_INTROTEXT%", intro);
	res = ReplaceStr(res, "%DOC_AUTHOR%", author);
	res = ReplaceStr(res, "%DOC_GENERATOR%", "FileDocumentor, www.c4d-jack.de");

	// Date variables
	Int32 Year;
	Int32 Month;
	Int32 Day;
	Int32 Hour;
	Int32 Minute;
	Int32 Second;
	DateTime dt;
	GetDateTimeNow(dt);
	Year = dt.year;
	Month = dt.month;
	Day = dt.day;
	Hour = dt.hour;
	Minute = dt.minute;
	Second = dt.second;

	// Date placeholders
	res = ReplaceStr(res, "%DOC_DATE%", String::IntToString(Year) + "-" + LongToString2digits(Month) + "-" + LongToString2digits(Day));
	res = ReplaceStr(res, "%DOC_TIME%", LongToString2digits(Hour) + ":" + LongToString2digits(Minute) + ":" + LongToString2digits(Second));
	res = ReplaceStr(res, "%DOC_YEAR%", String::IntToString(Year));

	return res;
}

void IterateObjectsForDocumentation(BaseObject *op, Int32 &counter, BaseFile *bf, Int32 hLevel, Bool JustTest)
// Iterate all objects and their comment tags in the scene,
// write info to output file
{
	BaseTag *tag = nullptr;
	//BaseDocument *doc = op->GetDocument();
	String content = String();
	String opname = String();
	String tagname = String();
	String icontxt = String();

	while (op)
	{
		opname = op->GetName();

		// Iterate tags of object
		tag = op->GetFirstTag();
		while (tag)
		{
			if (tag->GetType() == ID_TCOMMENT)
			{
				if (!JustTest && bf)
				{
					tagname = tag->GetDataInstance()->GetString(COMMENT_TITLE);
					content = tag->GetDataInstance()->GetString(COMMENT_TEXT);
					if (tag->GetDataInstance()->GetInt32(COMMENT_ICONMODE) != COMMENT_ICONMODE_0)
					{
						icontxt = " [" + GetSelectedCycleElementName(tag, COMMENT_ICONMODE) + "]";
					}
					else
						icontxt = "";

					//Convert special chars to HTML entities
					opname = ConvertHTMLchars(opname);
					tagname = ConvertHTMLchars(tagname);
					content = ConvertHTMLchars(content);

					// Write object name, tag name and comment text.
					// Use standard Heading tags + CSS classes named after the hierarchy level of the current object
					// To style the document, edit res\style.css and refer to the CSS classes
					WriteString("<h2 class=\"obj" + String::IntToString(hLevel) + "\">" + GeLoadString(IDS_CDOC_OBJECT) + " \"" + opname + "\"</h2>\n", bf);
					WriteString("<h3 class=\"tag" + String::IntToString(hLevel) + "\">" + tagname + icontxt + "</h3>\n", bf);
					WriteString("<p class=\"comment\">" + content + "</p>\n", bf);
				}
				tag = tag->GetNext();	// Get next tag
				counter++;						// Increase counter
			}
			else
				tag = tag->GetNext();	// Get next tag
		}

		// Iterate hierarchy vertical
		if (op->GetDown())
			IterateObjectsForDocumentation(op->GetDown(), counter, bf, hLevel + 1, JustTest);

		// Next Object
		op = op->GetNext();
	}
}

/////////////////////////////////////////
// Private Dialog functions
/////////////////////////////////////////
Bool DocCreationDialog::CreateDocument(BaseDocument* doc, BaseContainer *settings)
{
	Int32 Counter = 0;			// Counter for processed comment tags
	Filename HEADfilename = TmplDir + "header.html";
	Filename FOOTfilename = TmplDir + "footer.html";
	BaseFile *bf = nullptr;		// File to be created
	BaseFile *tmpl = nullptr;		// HTML template file(s)
	String Cnt = String();		// File content
	Bool OpenDocument = settings->GetBool(IDC_CDOC_OPENDOCUMENT);

	// Make status bar spin
	StatusSetSpin();

	// Get data from settings container
	Filename DocFilename = Filename(settings->GetString(IDC_CDOC_FILENAME));

	DocFilename.SetSuffix("html");

	if (DocFilename.GetString() != "")
	{
		// Get first object in scene
		BaseObject *op = doc->GetFirstObject(); if (!op) return false;

		// Allocate files
		bf = BaseFile::Alloc(); if (!bf) goto CancelError;
		tmpl = BaseFile::Alloc(); if (!tmpl) goto CancelError;

		// Create target file
		if (!bf->Open(DocFilename, FILEOPEN_WRITE)) goto CancelError;

		// Process header template
		if (tmpl->Open(HEADfilename))
		{
			Cnt = GetTextContent(tmpl);
			tmpl->Close();
			Cnt = ProcessTemplate(Cnt, doc, settings);
			Cnt = ConvertHTMLchars(Cnt);
			WriteString(Cnt, bf);
		}
		else
		{
			goto CancelError;
		}

		// Iterate scene and generate document's main contents
		IterateObjectsForDocumentation(op, Counter, bf, 2, false);

		// Process footer template
		if (tmpl->Open(FOOTfilename))
		{
			Cnt = "";
			Cnt = GetTextContent(tmpl);
			tmpl->Close();
			Cnt = ProcessTemplate(Cnt, doc, settings);
			Cnt = ConvertHTMLchars(Cnt);
			WriteString(Cnt, bf);
		}
		else
		{
			goto CancelError;
		}

		bf->Close();						// Close target file
		if (bf) BaseFile::Free(bf);			// Free target file
		if (tmpl) BaseFile::Free(tmpl);		// Free template file

		if (OpenDocument)
			GeExecuteFile(DocFilename);			// Open generated HTML document in browser
		else
			GeOutString(GeLoadString(IDS_CREATEDOCUMENT_DONE, DocFilename.GetString()), GEMB_OK);	// Show message
	}

	// Clear Status Bar
	StatusClear();

	return true;

	// If anything fishy happened...
CancelError:
	if (bf) BaseFile::Free(bf);											// Free target file
	if (tmpl) BaseFile::Free(tmpl);										// Free template file
	GeOutString(GeLoadString(IDS_CREATEDOCUMENT_ERRORMSG), GEMB_OK);	// Show error message

	// Clear Status Bar
	StatusClear();

	return false;
}

/////////////////////////////////////////
// Public Dialog functions
/////////////////////////////////////////
DocCreationDialog::DocCreationDialog(void)
// Constructor
{
}

Bool DocCreationDialog::CreateLayout(void)
// Create the dialog's layout
{
	// first call the parent instance
	Bool res = GeDialog::CreateLayout();

	// Set dialog title
	SetTitle(GeLoadString(IDS_CDOC_DLGTITLE));

	// Filename selector
	GroupBegin(0, BFH_SCALEFIT, 2, 0, GeLoadString(IDS_CDOC_GRP_FILENAME), 0);
	{
		GroupBorder(BORDER_WITH_TITLE|BORDER_GROUP_IN);
		GroupBorderSpace(4,4,4,4);

		AddEditText(IDC_CDOC_FILENAME, BFH_SCALEFIT);
		AddButton(IDC_CDOC_FILENAME_CHOOSE, BFH_RIGHT, 0, 0, GeLoadString(IDS_CDOC_FILENAME));
	}
	GroupEnd();

	// Additional information
	GroupBegin(0, BFH_SCALEFIT, 1, 0, GeLoadString(IDS_CDOC_GRP_ADDINFO), 0);
	{
		GroupBorder(BORDER_WITH_TITLE|BORDER_GROUP_IN);
		GroupBorderSpace(4,4,4,4);

		// Author, title, etc...
		GroupBegin(0, BFH_SCALEFIT, 2, 0, "", 0);
		{
			AddStaticText(0, BFH_FIT, SizeChr(140), 0, GeLoadString(IDS_CDOC_TITLE), 0);
			AddEditText(IDC_CDOC_TITLE, BFH_SCALEFIT);

			AddStaticText(0, BFH_FIT, SizeChr(140), 0, GeLoadString(IDS_CDOC_AUTHOR), 0);
			AddEditText(IDC_CDOC_AUTHOR, BFH_SCALEFIT);
		}
		GroupEnd();

		this->AddSeparatorH(100, BFH_SCALEFIT);

		// Intro text
		AddStaticText(0, BFH_FIT, SizeChr(140), 0, GeLoadString(IDS_CDOC_INTROTXT), 0);
		AddMultiLineEditText(IDC_CDOC_INTROTEXT, BFH_SCALEFIT, 0, SizePix(100));
	}
	GroupEnd();

	// A big layout group
	GroupBegin(0, BFH_SCALEFIT, 2, 0, "", BFV_GRIDGROUP_EQUALROWS);
	{
		// Embedded image
		GroupBegin(0, BFH_SCALEFIT, 1, 0, GeLoadString(IDS_CDOC_GRP_SETTINGS_IMAGE), 0);
		{
			GroupBorder(BORDER_WITH_TITLE|BORDER_GROUP_IN);
			GroupBorderSpace(4,4,4,4);

			// Editor Preview or Rendering?
			AddRadioGroup(IDC_CDOC_IMAGE, BFH_LEFT, 2);
			AddChild(IDC_CDOC_IMAGE, IDC_CDOC_IMAGE_EDITOR, GeLoadString(IDS_CDOC_IMAGE_EDITOR));
			AddChild(IDC_CDOC_IMAGE, IDC_CDOC_IMAGE_RENDER, GeLoadString(IDS_CDOC_IMAGE_RENDER));

			// Embedded image size
			GroupBegin(0, BFH_SCALEFIT, 2, 0, "", 0);
			{
				AddStaticText(0, BFH_FIT, SizeChr(50), 0, GeLoadString(IDS_CDOC_IMAGE_WIDTH), 0);
				AddEditNumberArrows(IDC_CDOC_IMAGE_WIDTH, BFH_FIT, SizeChr(50));
				AddStaticText(0, BFH_FIT, SizeChr(50), 0, GeLoadString(IDS_CDOC_IMAGE_HEIGHT), 0);
				AddEditNumberArrows(IDC_CDOC_IMAGE_HEIGHT, BFH_FIT, SizeChr(50));
			}
			GroupEnd();
		}
		GroupEnd();

		// Settings
		GroupBegin(0, BFH_SCALEFIT|BFV_TOP, 1, 0, GeLoadString(IDS_CDOC_GRP_SETTINGS), 0);
		{
			GroupBorder(BORDER_WITH_TITLE|BORDER_GROUP_IN);
			GroupBorderSpace(4,4,4,4);

			AddCheckbox(IDC_CDOC_OPENDOCUMENT, BFH_SCALEFIT, SizeChr(140), 0, GeLoadString(IDS_CDOC_OPENDOCUMENT));  
		}
		GroupEnd();

	}
	GroupEnd();

	// Template info
	GroupBegin(0, BFH_SCALEFIT, 1, 0, GeLoadString(IDS_CDOC_GRP_TEMPLATE), 0);
	{
		GroupBorder(BORDER_WITH_TITLE|BORDER_GROUP_IN);
		GroupBorderSpace(4,4,4,4);

		Filename fn = TmplDir;
		AddStaticText(0, BFH_SCALEFIT, SizeChr(140), 0, GeLoadString(IDS_CDOC_TMPLDIR_INFO), 0);

		// Template folder display & open button
		GroupBegin(0, BFH_SCALEFIT, 2, 0, "", 0);
		{
			AddStaticText(0, BFH_SCALEFIT, SizeChr(140), 0, GeLoadString(IDS_CDOC_TMPLDIR, fn.GetString()), 0);
			AddButton(IDC_CDOC_TMPLDIR, BFH_RIGHT, 0, 0, GeLoadString(IDS_CDOC_TMPLDIR_OPEN));
		}
		GroupEnd();
	}
	GroupEnd();

	// Generate Button
	AddButton(IDC_CDOC_GENERATE, BFH_FIT, 0, 0, GeLoadString(IDS_CDOC_GENERATE));

	// Help Button
	AddButton(IDC_CDOC_HELP, BFH_FIT, 0, 0, GeLoadString(IDS_CDOC_HELP));

	return res;
}

Bool DocCreationDialog::InitValues(void)
// Set default values for dialog elements
{
	// first call the parent instance
	if (!GeDialog::InitValues()) return false;

	// Title
	String title = GetActiveDocument()->GetDocumentName().GetString();
	SetString(IDC_CDOC_TITLE, title);

	//Author
	SerialInfo si;
	GeGetSerialInfo(SERIALINFO_CINEMA4D, &si);
	String author = si.name + ", " + si.organization;
	SetString(IDC_CDOC_AUTHOR, author);

	// Embedded Image Radio Buttons
	SetInt32(IDC_CDOC_IMAGE, IDC_CDOC_IMAGE_EDITOR);

	// Embedded Image size
	SetInt32(IDC_CDOC_IMAGE_WIDTH, 320);
	SetInt32(IDC_CDOC_IMAGE_HEIGHT, 240);

	// Settings
	SetBool(IDC_CDOC_OPENDOCUMENT, true);

	return true;
}

Bool DocCreationDialog::Command(Int32 id, const BaseContainer &msg)
// A command button in the dialog has been clicked
{
	BaseDocument *doc = GetActiveDocument();

	switch (id)
	{
	case IDC_CDOC_GENERATE:
		{
			BaseContainer settings;

			// Put filename into settings container
			Filename fn = Filename();
			GetFilename(IDC_CDOC_FILENAME, fn);
			settings.SetString(IDC_CDOC_FILENAME, fn.GetString());

			// Put author name into settings container
			String title = String();
			GetString(IDC_CDOC_TITLE, title);
			settings.SetString(IDC_CDOC_TITLE, title);

			// Put author name into settings container
			String author = String();
			GetString(IDC_CDOC_AUTHOR, author);
			settings.SetString(IDC_CDOC_AUTHOR, author);

			// Put intro text into settings container
			String intro = String();
			GetString(IDC_CDOC_INTROTEXT, intro);
			settings.SetString(IDC_CDOC_INTROTEXT, intro);

			// Settings for embedded image
			Int32 imagesetting = 0;
			GetInt32(IDC_CDOC_IMAGE, imagesetting);
			settings.SetInt32(IDC_CDOC_IMAGE, imagesetting);
			GetInt32(IDC_CDOC_IMAGE_WIDTH, imagesetting);
			settings.SetInt32(IDC_CDOC_IMAGE_WIDTH, imagesetting);
			GetInt32(IDC_CDOC_IMAGE_HEIGHT, imagesetting);
			settings.SetInt32(IDC_CDOC_IMAGE_HEIGHT, imagesetting);

			// More settings
			Bool val;
			GetBool(IDC_CDOC_OPENDOCUMENT, val);
			settings.SetBool(IDC_CDOC_OPENDOCUMENT, val);

			// Start document generation
			CreateDocument(doc, &settings);
			break;
		}
	case IDC_CDOC_FILENAME_CHOOSE:
		{
			Filename fn = Filename();
			fn.SetFile(doc->GetDocumentName());
			fn.SetSuffix("html");
			String DlgTitle = GeLoadString(IDS_CREATEDOCUMENT_DLGTITLE_SAVEFILE);
			if (!fn.FileSelect(FILESELECTTYPE_ANYTHING, FILESELECT_SAVE, DlgTitle)) return false;
			SetString(IDC_CDOC_FILENAME, fn.GetString());
			break;
		}

	case IDC_CDOC_TMPLDIR:
		{
			GeExecuteFile(TmplDir);
			break;
		}

	case IDC_CDOC_HELP:
		{
			if (!GeExecuteFile(HelpFile + "ccreatedocument.html"))
				GeOutString(GeLoadString(IDS_HELP_ERROR), GEMB_OK);
		}
	}
	return true;
}

Bool DocCreationDialog::CoreMessage(Int32 id, const BaseContainer &msg)
{
	switch (id)
	{
		case EVMSG_CHANGE:
			if (CheckCoreMessage(msg))
			{
			}
			break;
	}
	return GeDialog::CoreMessage(id,msg);
}

Int32 DocCreationDialog::Message(const BaseContainer &msg, BaseContainer &result)
// Recieve a GUI message
// (in this case it's just used for Enabling/Disabling the dialog elements)
{
	// Always disable Filename text field
	Enable(IDC_CDOC_FILENAME, false);

	// Enable/Disable Generate Button
	String fn = String();
	GetString(IDC_CDOC_FILENAME, fn);
	Enable(IDC_CDOC_GENERATE, fn != "");

	return GeDialog::Message(msg,result);
}

/////////////////////////////////////////
// Command class declaration
/////////////////////////////////////////
class CCreateDocument : public CommandData
{
	INSTANCEOF(CCreateDocument,CommandData)

private:
	DocCreationDialog dlg;

public:
	virtual Bool Execute(BaseDocument* doc);
	virtual Int32 GetState(BaseDocument* doc);
	virtual Bool RestoreLayout(void *secret);
};

/////////////////////////////////////////
// Command functions
/////////////////////////////////////////
Int32 CCreateDocument::GetState(BaseDocument* doc)
{
	BaseObject *op = doc->GetFirstObject();
	if (!op) return 0;

	Int32 Counter = 0;
	IterateObjectsForDocumentation(op, Counter, nullptr, 1, true);

	if (Counter > 0)
		return CMD_ENABLED;

	return 0;
}

Bool CCreateDocument::Execute(BaseDocument *doc)
{
	return dlg.Open(DLG_TYPE_ASYNC, ID_CREATEDOCUMENT, -1, -1, 640);
}

Bool CCreateDocument::RestoreLayout(void *secret)
{
	return dlg.RestoreLayout(ID_CREATEDOCUMENT, 0, secret);
}

Bool RegisterCreateDocument()
{
	// decide by name if the plugin shall be registered - just for user convenience
	String name=GeLoadString(IDS_CREATEDOCUMENT); if (!name.Content()) return true;
	return RegisterCommandPlugin(ID_CREATEDOCUMENT, name, 0, AutoBitmap("CCreateDocument.tif"), GeLoadString(IDS_CREATEDOCUMENT_HELP), NewObjClear(CCreateDocument));
}
