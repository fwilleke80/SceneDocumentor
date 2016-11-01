#include "documentorfunctions.h"
#include "tCommentClass.h"

// Return TRUE of Object "op" has any comment tags attached to it
Bool ObjectHasComments(BaseObject *op)
{
	BaseTag *tag = NULL;

	// Iterate tags of object
	tag = op->GetFirstTag();
	while (tag)
	{
		if (tag->GetType() == ID_TCOMMENT)
			return TRUE;
		else
			tag = tag->GetNext();	// Get next tag
	}
	return FALSE;
}


// Iterate all objects and their comment tags in the scene,
// return TRUE if any were found
Bool SearchForComments(BaseObject *op)
{
	//BaseTag *tag = NULL;

	while (op)
	{
		if (ObjectHasComments(op))
			return TRUE;

		// Iterate hierarchy vertical
		if (op->GetDown())
			SearchForComments(op->GetDown());

		// Next Object
		op = op->GetNext();
	}

	return FALSE;
}

// Returns the name/string for the currently selected item in a cycle
// cycle_id MUST refer to a LONG cycle!
String GetSelectedCycleElementName(BaseList2D *node, LONG cycle_id)
{
	// We need these two
	AutoAlloc<AtomArray> ar; if(!ar) return String();
	AutoAlloc<Description> descr;

	// Get the passed node's description
	node->GetDescription(descr, DESCFLAGS_DESC_0);

	// Get the description element container for the passed ID
	BaseContainer *b = descr->GetParameterI(DescLevel(cycle_id), ar);
	if (!b) return String();

	// Get the cycle container (that's why the passed cycle_id must refer to a cycle)
	BaseContainer c = b->GetContainer(DESC_CYCLE);

	// Return the label string of the selected cycle option
	return c.GetString(node->GetDataInstance()->GetLong(cycle_id));
}
