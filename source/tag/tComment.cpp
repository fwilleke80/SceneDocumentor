// Include some stuff from the API
#include "c4d.h"
#include "c4d_symbols.h"
#include "tCommentClass.h"
#include "tComment.h"
#include "documentorfunctions.h"

// Help path
#define HelpFile GeGetPluginPath() + "help"

// If the tag contains text, create a nullptr Object and
// use the tag's object's name + the tag's content as
// name for the new nullptr Object.
// Requires BaseDocument::Start() Undo to be called before calling this function.
Bool tComment::CreateNullObj(BaseTag *tag, Bool IncludeOpName, Bool IncludeTitle, Bool IncludeIcon, BaseObject *parent)
{
	// Get Text from tag
	String newname = tag->GetDataInstance()->GetString(COMMENT_TEXT);

	if (newname != "") {
		// Get object of tag
		BaseObject *op = tag->GetObject(); if (!op) return false;

		// Create new nullptr Object
		BaseObject *no = BaseObject::Alloc(Onull);

		if (no) {
			
			// Include Tag Title
			if (IncludeTitle)
				newname =  tag->GetDataInstance()->GetString(COMMENT_TITLE) + ": " + newname;

			// Include Tag Icon
			if (IncludeIcon && tag->GetDataInstance()->GetInt32(COMMENT_ICONMODE) != COMMENT_ICONMODE_0)
			{
				if (newname.Content())
					newname += " ";

				newname	+= "[" + GetSelectedCycleElementName(tag, COMMENT_ICONMODE) + "]";
			}

			// Include Object name
			if (IncludeOpName) {
				if (IncludeTitle)
					newname = op->GetName() + " -> " + newname;
				else
					newname = op->GetName() + ": " + newname;
			}

			// Set name to nullptr object
			no->SetName(newname);

			if (parent)
				// Put new object into Information group
				no->InsertUnderLast(parent);
			else
				// Put new object above current object
				no->InsertBefore(op);

			// Add Undo for creation for new object
			op->GetDocument()->AddUndo(UNDOTYPE_NEW, no);

			return true;
		}
		else
			return false;
	}
	return false;
}

Bool tComment::Init(GeListNode *node)
{
	// Get pointer to tag's Container
	BaseTag			*tag  = (BaseTag*)node;
	BaseContainer	*data = tag->GetDataInstance();

	data->SetString(COMMENT_HELPTEXT, GeLoadString(IDS_TCOMMENT_HELPTEXT));
	data->SetString(COMMENT_TITLE, GeLoadString(IDS_COMMENT_TITLE));
	data->SetInt32(COMMENT_ICONMODE, COMMENT_ICONMODE_0);

	// Load icons for tag
	// ------------------

	// Standard icon
	BaseBitmap *icon = BaseBitmap::Alloc();
	if (!icon) return false;
	Filename fn = GeGetPluginPath() + "res" + "tComment.tif";
	icon->Init(fn);
	SetCustomIcon(0, icon);

	// Special Icon 1
	icon = BaseBitmap::Alloc();
	if (!icon) return false;
	fn = GeGetPluginPath() + "res" + "tComment_1.tif";
	icon->Init(fn);
	SetCustomIcon(1, icon);

	// Special Icon 2
	icon = BaseBitmap::Alloc();
	if (!icon) return false;
	fn = GeGetPluginPath() + "res" + "tComment_2.tif";
	icon->Init(fn);
	SetCustomIcon(2, icon);

	// Special Icon 3
	icon = BaseBitmap::Alloc();
	if (!icon) return false;
	fn = GeGetPluginPath() + "res" + "tComment_3.tif";
	icon->Init(fn);
	SetCustomIcon(3, icon);

	// Special Icon 4
	icon = BaseBitmap::Alloc();
	if (!icon) return false;
	fn = GeGetPluginPath() + "res" + "tComment_4.tif";
	icon->Init(fn);
	SetCustomIcon(4, icon);

	// Special Icon 5
	icon = BaseBitmap::Alloc();
	if (!icon) return false;
	fn = GeGetPluginPath() + "res" + "tComment_5.tif";
	icon->Init(fn);
	SetCustomIcon(5, icon);

	return true;
}

Bool tComment::GetDDescription(GeListNode *node, Description *description, DESCFLAGS_DESC &flags)
{
	// Load description (user interface)
	if (!description->LoadDescription(ID_TCOMMENT)) return false;
	flags |= DESCFLAGS_DESC_LOADED;

	return true;
}

Bool tComment::Message(GeListNode* node, Int32 type, void* data)
{
	BaseTag *tag = (BaseTag*)node; if(!tag) return true;
	BaseContainer *bc = tag->GetDataInstance(); if (!bc) return true;
	BaseDocument *doc = tag->GetDocument(); if (!doc) return true;

	switch (type)
	{
		// A command button has been clicked
		case MSG_DESCRIPTION_COMMAND:
			{
				DescriptionCommand *dc = (DescriptionCommand*) data;
				switch (dc->id[0].id) {
					case COMMENT_CREATEOBJ:
					{
						// Create nullptr Object
						doc->StartUndo();

						// Get user options
						Bool IncludeOpName = GeOutString(GeLoadString(IDS_CREATECOMMENTNULL_INC_OPNAME), GEMB_YESNO) == GEMB_R_YES;
						Bool IncludeTitle = GeOutString(GeLoadString(IDS_CREATECOMMENTNULL_INC_TITLE), GEMB_YESNO) == GEMB_R_YES;

						// Create nullptr object for this tag
						CreateNullObj(tag, IncludeOpName, IncludeTitle, true, nullptr);

						doc->EndUndo();
						break;
					}

					case COMMENT_OPENHELP:
					{
						// Open HTML help
						if (!GeExecuteFile(HelpFile + "tcomment.html"))
							GeOutString(GeLoadString(IDS_HELP_ERROR), GEMB_OK);
						break;
					}
				}
				break;
			}

		// The custom icon has been requested
		case MSG_GETCUSTOMICON:
			{
				GetCustomIconData *cid = (GetCustomIconData*)data;
				IconData *id = cid->dat;

				if (id) {
					switch (bc->GetInt32(COMMENT_ICONMODE))
					{
						case COMMENT_ICONMODE_0:
							id->bmp = GetCustomIcon(0);
							break;
						case COMMENT_ICONMODE_1:
							id->bmp = GetCustomIcon(1);
							break;
						case COMMENT_ICONMODE_2:
							id->bmp = GetCustomIcon(2);
							break;
						case COMMENT_ICONMODE_3:
							id->bmp = GetCustomIcon(3);
							break;
						case COMMENT_ICONMODE_4:
							id->bmp = GetCustomIcon(4);
							break;
						case COMMENT_ICONMODE_5:
							id->bmp = GetCustomIcon(5);
							break;
						default:
							id->bmp = GetCustomIcon(0);
							break;
					}

					// Adjust image dimensions
					id->x = 0;
					id->y = 0;
					id->w = 32;
					id->h = 32;

					// Yep, we changed something
					cid->filled = true;
				}
				break;
			}
	}

	return SUPER::Message(node,type,data);
}
Bool RegisterTComment(void)
{
	// decide by name if the plugin shall be registered - just for user convenience
	String name=GeLoadString(IDS_TCOMMENT); if (!name.Content()) return true;
	return RegisterTagPlugin(ID_TCOMMENT, name, TAG_VISIBLE|TAG_MULTIPLE, tComment::Alloc, "tComment", AutoBitmap("tComment.tif"), 0);
}
