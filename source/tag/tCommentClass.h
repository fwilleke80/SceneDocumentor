#ifndef TCOMMENTCLASS_H_
#define TCOMMENTCLASS_H_

#include "c4d.h"


// Unique Plugin ID obtained from www.plugincafe.com
static const Int32 ID_TCOMMENT = 1024694;

// Help path
#define HelpFile GeGetPluginPath() + "help"


// Plugin Class declaration
class tComment : public TagData
{
	INSTANCEOF(tComment,TagData)
	
private:
	// Array with icon bitmaps
	maxon::BaseArray<BaseBitmap*> customIcon;
	
public:
	// Standard class members
	virtual Bool Init(GeListNode *node);
	virtual Bool GetDDescription(GeListNode *node, Description *description, DESCFLAGS_DESC &flags);
	virtual Bool Message(GeListNode* node, Int32 type, void* data);
	static NodeData *Alloc();
	
	// Custom class members
	Bool CreateNullObj(BaseTag *tag, Bool IncludeOpName, Bool IncludeTitle, Bool IncludeIcon, BaseObject *parent);
	BaseBitmap *GetCustomIcon(Int32 index);
	void SetCustomIcon(Int32 index, BaseBitmap *b);
	
	// Deconstructor: Clear icon storage on destruction
	~tComment()
	{
		BaseBitmap *icon = customIcon[0];
		if (icon) BaseBitmap::Free(icon);
		icon = customIcon[1];
		if (icon) BaseBitmap::Free(icon);
		icon = customIcon[2];
		if (icon) BaseBitmap::Free(icon);
		icon = customIcon[3];
		if (icon) BaseBitmap::Free(icon);
		icon = customIcon[4];
		if (icon) BaseBitmap::Free(icon);
		icon = customIcon[5];
		if (icon) BaseBitmap::Free(icon);
	}
};


#endif /* TCOMMENTCLASS_H_ */
