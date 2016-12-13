#include "c4d.h"
#include "ge_dynamicarray.h"


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
		static NodeData *Alloc() { return NewObjClear(tComment); }

		// Custom class members
		Bool CreateNullObj(BaseTag *tag, Bool IncludeOpName, Bool IncludeTitle, Bool IncludeIcon, BaseObject *parent);
		BaseBitmap *GetCustomIcon(Int32 index)						{ return customIcon[index]; }
		void SetCustomIcon(Int32 index, BaseBitmap *b)		{ customIcon[index] = b; }

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

// Unique Plugin ID obtained from www.plugincafe.com
#define ID_TCOMMENT	1024694

// Help path
#define HelpFile GeGetPluginPath() + "help"
