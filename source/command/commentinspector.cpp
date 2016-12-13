#include "c4d.h"
#include "c4d_symbols.h" 
#include "commentinspector.h"
#include "tComment.h"
#include "tCommentClass.h"
#include "stringfunctions.h"
#include "documentorfunctions.h"


// ID obtained from www.plugincafe.com
#define ID_COMMENTINSPECTOR	1025157
#define DOCUMENTOR_EVENT_MESSAGE	1025158


/////////////////////////////////////////
// Dialog class & element declaration
/////////////////////////////////////////
class CommentInspectorDialog : public GeDialog
{
	INSTANCEOF(CommentInspectorDialog,GeDialog)

private:
	String GetComments(Bool UseChildren);

public:
	CommentInspectorDialog(void);
	virtual Bool CreateLayout(void);
	virtual Bool InitValues(void);
	virtual Int32 Message(const BaseContainer &msg, BaseContainer &result);
	virtual Bool CoreMessage(Int32 id, const BaseContainer &msg);
};


// Dialog elements
enum
{
	IDC_CINSP_TEXT = 10000,
	IDC_CINSP_USECHILDREN,

	IDC_MEMORY_STAT_
};


/////////////////////////////////////////
// General functions
/////////////////////////////////////////

#define CRLF GeGetLineEnd()


// Get a block of text, containing all comments from object "op"
String GetCommentsText(BaseObject *op)
{
	BaseTag *tag = nullptr;
	String result = String();
	String block = String();
	String icontxt = String();
	BaseContainer *td = nullptr;

	// Iterate tags of object
	tag = op->GetFirstTag();
	while (tag)
	{
		if (tag->GetType() == ID_TCOMMENT)
		{
			td = tag->GetDataInstance();

			block = "";

			if (td->GetInt32(COMMENT_ICONMODE) != COMMENT_ICONMODE_0)
			{
				icontxt = " [" + GetSelectedCycleElementName(tag, COMMENT_ICONMODE) + "]";
			}
			if (td->GetString(COMMENT_TITLE).Content())
				block += td->GetString(COMMENT_TITLE) + icontxt + ":" + CRLF;
			if (td->GetString(COMMENT_TEXT).Content())
				block += td->GetString(COMMENT_TEXT) + CRLF;

			if (block.Content())
				result += block;
		}

		tag = tag->GetNext();	// Get next tag
	}
	return result;
}


// Get object name and put decorative lines around it ;-)
String GetObjectTitle(BaseObject *op, const String Decor = "==")
{
	if (op)
		return Decor + " " + op->GetName() + " " + Decor;
	else
		return String();
}

// Iterate "op"'s children and collect the comments into a block of text
String GetChildObjectsComments(BaseObject *op)
{
	BaseObject *child = nullptr;
	String result = String();
	String block = String();

	child = op->GetDown();
	while (child)
	{
		// Get Comments
		if (ObjectHasComments(child))
		{
			block = GetObjectTitle(child, "--") + CRLF;
			block += GetCommentsText(child);

			result += block;
		}

		// Iteration continues...
		if (child->GetDown())
			result += GetChildObjectsComments(child);
		child = child->GetNext();
	}

	return result;
}


/////////////////////////////////////////
// Private Dialog functions
/////////////////////////////////////////

// Get all comments from all selected objects
String CommentInspectorDialog::GetComments(Bool UseChildren)
{
	// Get active document
	BaseDocument *doc = GetActiveDocument();
	if (!doc) return String();

	// Get AtomArray with selected objects
	AtomArray *sel = AtomArray::Alloc();
	if (!sel) return String();
	doc->GetActiveObjects(*sel, GETACTIVEOBJECTFLAGS::GETACTIVEOBJECTFLAGS_CHILDREN);

	// More stuff we'll need
	BaseObject *op = nullptr;
	Int32 i = 0;
	String result = String();
	String block = String();

	// Iterate selected objects
	for (i=0; i < sel->GetCount(); i++)
	{
		op = (BaseObject*)(sel->GetIndex(i));
		if (ObjectHasComments(op))
		{
			block = GetObjectTitle(op) + CRLF;
			block += GetCommentsText(op);

			// Append new text block to result
			result += block;

			// If this is not the last selected object (more objects
			// will follow), add another line break.
			if (i < sel->GetCount() - 1)
				result += CRLF;
		}

		if (UseChildren)
		{
			result += GetChildObjectsComments(op);
		}
	}

	if (sel) AtomArray::Free(sel);

	return result;
}


/////////////////////////////////////////
// Public Dialog functions
/////////////////////////////////////////
CommentInspectorDialog::CommentInspectorDialog(void)
// Constructor
{
}

Bool CommentInspectorDialog::CreateLayout(void)
// Create the dialog's layout
{
	// first call the parent instance
	Bool res = GeDialog::CreateLayout();

	// Set dialog title
	SetTitle(GeLoadString(IDS_CINSP_DLGTITLE));

	AddMultiLineEditText(IDC_CINSP_TEXT, BFH_FIT|BFV_FIT|BFV_SCALE|BFH_SCALE);
	AddCheckbox(IDC_CINSP_USECHILDREN, BFH_LEFT, 0, 0, GeLoadString(IDS_CINSP_USECHILDREN));

	return res;
}

Bool CommentInspectorDialog::InitValues(void)
// Set default values for dialog elements
{
	// first call the parent instance
	if (!GeDialog::InitValues()) return false;

	SetString(IDC_CINSP_TEXT, String(""));
	SetBool(IDC_CINSP_USECHILDREN, true);

	return true;
}

Bool CommentInspectorDialog::CoreMessage(Int32 id, const BaseContainer &msg)
{
	switch (id)
	{
	case EVMSG_CHANGE:
	case DOCUMENTOR_EVENT_MESSAGE:
		if (CheckCoreMessage(msg))
		{
			Bool UseChildren = false;
			GetBool(IDC_CINSP_USECHILDREN, UseChildren);

			String cnt = GetComments(UseChildren);
			SetString(IDC_CINSP_TEXT, cnt);
		}
		break;
	}
	return GeDialog::CoreMessage(id,msg);
}


Int32 CommentInspectorDialog::Message(const BaseContainer &msg, BaseContainer &result)
// Receive a GUI message
// (in this case it's just used for Enabling/Disabling the dialog elements)
{
	// Always disable text field
	//Enable(IDC_CINSP_TEXT, false);

	switch (msg.GetId())
	{
	case BFM_ACTION:
		{
			if (msg.GetInt32(BFM_ACTION_ID) == IDC_CINSP_USECHILDREN)
				SpecialEventAdd(DOCUMENTOR_EVENT_MESSAGE);
		}
	}

	return GeDialog::Message(msg,result);
}

/////////////////////////////////////////
// Command class declaration
/////////////////////////////////////////
class CCommentInspector : public CommandData
{
	INSTANCEOF(CCommentInspector,CommandData)

private:
	CommentInspectorDialog dlg;

public:
	virtual Bool Execute(BaseDocument* doc);
	virtual Bool RestoreLayout(void *secret);
};

/////////////////////////////////////////
// Command functions
/////////////////////////////////////////
Bool CCommentInspector::Execute(BaseDocument *doc)
{
	return dlg.Open(DLG_TYPE_ASYNC, ID_COMMENTINSPECTOR, -1, -1, 640);
}

Bool CCommentInspector::RestoreLayout(void *secret)
{
	return dlg.RestoreLayout(ID_COMMENTINSPECTOR, 0, secret);
}

/////////////////////////////////////////
// Register function
/////////////////////////////////////////
Bool RegisterCommentInspector(void)
{
	// decide by name if the plugin shall be registered - just for user convenience
	String name=GeLoadString(IDS_COMMENTINSPECTOR); if (!name.Content()) return true;
	return RegisterCommandPlugin(ID_COMMENTINSPECTOR, name, 0, AutoBitmap("CCommentInspector.tif"), GeLoadString(IDS_COMMENTINSPECTOR_HELP), NewObjClear(CCommentInspector));
}
