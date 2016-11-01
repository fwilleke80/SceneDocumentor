#include "c4d.h"
#include "c4d_symbols.h"
#include "documentorfunctions.h"
#include "removealltags.h"
#include "tCommentClass.h"

// ID obtained from www.plugincafe.com
#define ID_REMOVEALLTAGS	1024696

class CRemoveAllTags : public CommandData
{
	INSTANCEOF(CRemoveAllTags,CommandData)

	public:
		virtual Bool Execute(BaseDocument* doc);
		virtual Bool Message(LONG type, void* data);
		virtual LONG GetState(BaseDocument* doc);
};

void IterateObjectsForRemoval(BaseObject *op, LONG &counter, Bool SelectedOnly, Bool JustTest)
{
	if (!op) return;	// Cancel if no object is passed

	BaseTag *tag = NULL;
	BaseDocument *doc = op->GetDocument();
	BaseTag *deltag = NULL;

	while (op) {
		// If only selected object should be processed,
		// and the current object is not selected, continue with next object
		if (!JustTest && SelectedOnly && !op->GetBit(BIT_ACTIVE)) goto Iteration;

		// Iterate tags of object
		tag = op->GetFirstTag();
		while (tag) {
			if (tag->GetType() == ID_TCOMMENT) {
				if (!JustTest) {
					deltag = tag;	// Remember old tag to delete
					tag = tag->GetNext();	// Get next tag
					doc->AddUndo(UNDOTYPE_DELETE, deltag);
					deltag->Remove();		// Remove old tag from scene
					BaseTag::Free(deltag);	// Free old tag
					deltag = NULL;
					counter++;
				}
				else {
					// Just count
					tag = tag->GetNext();	// Get next tag
					counter++;
				}
				
			}
			else
				tag = tag->GetNext();
		}

Iteration:
		// Iterate hierarchy vertical
		if (op->GetDown())
			IterateObjectsForRemoval(op->GetDown(), counter, SelectedOnly, JustTest);
		
		// Next Object
		op = op->GetNext();
	}
}

Bool CRemoveAllTags::Execute(BaseDocument* doc)
{
	LONG Counter = 0;

	if (GeOutString(GeLoadString(IDS_REMOVEALLTAGS_QUESTION), GEMB_YESNO) == GEMB_R_NO)
		return FALSE;

    // Start Undo
	doc->StartUndo();

	// If there's at least one object selected,
	// only remove tags from selected objects
	// REMOVED - nobody wants that
	//Bool SelectedOnly = GeOutString(GeLoadString(IDS_REMOVEALLTAGS_QUESTION_SELECTED), GEMB_YESNO) == GEMB_R_YES;

    // Iterate all Objects and their tags. If Comment tag found, delete it
	BaseObject *op = doc->GetFirstObject();
	IterateObjectsForRemoval(op, Counter, FALSE /*SelectedOnly*/, FALSE);

	// Finalize Undo
	doc->EndUndo();

	// Redraw viewports
	EventAdd(EVENT_FORCEREDRAW);

	String Msg = String(GeLoadString(IDS_REMOVEALLTAGS_STATUS, LongToString(Counter)));
	StatusSetText(Msg);

	return TRUE;
}

Bool CRemoveAllTags::Message(LONG type, void* data)
{
	return SUPER::Message(type,data);
}

LONG CRemoveAllTags::GetState(BaseDocument* doc)
{
	BaseObject *op = doc->GetFirstObject();
	if (!op) return 0;

	LONG Counter = 0;
	IterateObjectsForRemoval(op, Counter, FALSE, TRUE);

	if (Counter > 0)
		return CMD_ENABLED;

	return 0;
}

Bool RegisterRemoveAllTags(void)
{
	// decide by name if the plugin shall be registered - just for user convenience
	String name=GeLoadString(IDS_REMOVEALLTAGS); if (!name.Content()) return TRUE;
	return RegisterCommandPlugin(ID_REMOVEALLTAGS, name, 0, AutoBitmap("CRemoveAllTags.tif"), GeLoadString(IDS_REMOVEALLTAGS_HELP), gNew CRemoveAllTags);
}
