#include "c4d.h"
#include "c4d_symbols.h"
#include "documentorfunctions.h"
#include "createcommentnulls.h"
#include "tCommentClass.h"

// ID obtained from www.plugincafe.com
#define ID_CREATECOMMENTNULLS	1024695

class CCreateCommentNulls : public CommandData
{
	INSTANCEOF(CCreateCommentNulls,CommandData)

	public:
		virtual Bool Execute(BaseDocument* doc);
		virtual Bool Message(LONG type, void* data);
		virtual LONG GetState(BaseDocument* doc);
};

void IterateObjectsAndTags(BaseObject *op, LONG &counter, Bool PutOnTopLevel, BaseObject *parent, Bool IncludeOpName, Bool IncludeTitle, Bool IncludeIcon, Bool JustTest = FALSE)
{
	BaseTag *tag = NULL;

	while (op)
	{
		// Create top object, if not already existent
		if (!parent && PutOnTopLevel && !JustTest) {
			parent = BaseObject::Alloc(Onull); if (!parent) return;
			String Name = GeLoadString(IDS_NULLPARENT);
			parent->SetName(Name);
			op->GetDocument()->InsertObject(parent, NULL, NULL, TRUE);
			op->GetDocument()->AddUndo(UNDOTYPE_NEW, parent);
		}

		// Iterate tags of object
		tag = op->GetFirstTag();
		while (tag)
		{
			if (tag->GetType() == ID_TCOMMENT)
			{
				// Trigger Null Object Creation
				if (JustTest)
				{
					// Test only, just count
					counter++;
				}
				else
				{
					// Create Null Object for this tag
					if (((tComment*)tag)->CreateNullObj(tag, IncludeOpName, IncludeTitle, IncludeIcon, parent))
						counter++;
				}

			}
			tag = tag->GetNext();
		}

		// Iterate hierarchy vertical
		if (op->GetDown())
			IterateObjectsAndTags(op->GetDown(), counter, PutOnTopLevel, parent, IncludeOpName, IncludeTitle, JustTest);
		
		// Next Object
		op = op->GetNext();
	}
}

Bool CCreateCommentNulls::Execute(BaseDocument* doc)
{
	// This variable is used to count how many tags are being processed
	LONG Counter = 0;

    // Start Undo
	doc->StartUndo();

    // Iterate all Objects and their tags. Abort if there's no object
	BaseObject *op = doc->GetFirstObject();
	if (!op) return FALSE;

	// Iterate all objects in the scene, and their tags
	IterateObjectsAndTags(op, Counter, TRUE, NULL, TRUE/*IncludeOpName*/, TRUE/*IncludeTitle*/, TRUE);

	// Finalize Undo
	doc->EndUndo();

	// Redraw viewports
	EventAdd(EVENT_FORCEREDRAW);

	// Write message to status bar
	String Msg = String(GeLoadString(IDS_CREATECOMMENTNULLS_STATUS, LongToString(Counter)));
	StatusSetText(Msg);

	return TRUE;
}

Bool CCreateCommentNulls::Message(LONG type, void* data)
{
	return SUPER::Message(type,data);
}

LONG CCreateCommentNulls::GetState(BaseDocument* doc)
{
	BaseObject *op = doc->GetFirstObject();
	if (!op) return 0;

	LONG Counter = 0;
	IterateObjectsAndTags(op, Counter, FALSE, NULL, FALSE, FALSE, TRUE, TRUE);

	if (Counter > 0)
		return CMD_ENABLED;

	return 0;
}

Bool RegisterCreateCommentNulls(void)
{
	// decide by name if the plugin shall be registered - just for user convenience
	String name=GeLoadString(IDS_CREATECOMMENTNULLS); if (!name.Content()) return TRUE;
	return RegisterCommandPlugin(ID_CREATECOMMENTNULLS, name, 0, AutoBitmap("CCreateCommentNulls.tif"), GeLoadString(IDS_CREATECOMMENTNULLS_HELP), gNew CCreateCommentNulls);
}
