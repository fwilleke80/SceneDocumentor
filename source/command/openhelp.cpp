#include "c4d.h"
#include "c4d_symbols.h" 
#include "openhelp.h"
#include "tCommentClass.h"
#include "commands.h"
#include "main.h"


// ID obtained from www.plugincafe.com
static const Int32 ID_OPENHELP = 1024753;

class COpenHelp : public CommandData
{
	INSTANCEOF(COpenHelp,CommandData)

	public:
		virtual Bool Execute(BaseDocument* doc);
		virtual Bool Message(Int32 type, void* data);
};


Bool COpenHelp::Execute(BaseDocument* doc)
{
	if (!GeExecuteFile(HelpFile + "index.html"))
		GeOutString(GeLoadString(IDS_HELP_ERROR), GEMB_OK);

	return true;
}

Bool COpenHelp::Message(Int32 type, void* data)
{
	return SUPER::Message(type,data);
}

Bool RegisterOpenHelp()
{
	// decide by name if the plugin shall be registered - just for user convenience
	String name=GeLoadString(IDS_OPENHELP); if (!name.Content()) return true;
	return RegisterCommandPlugin(ID_OPENHELP, name, 0, AutoBitmap("COpenHelp.tif"), GeLoadString(IDS_OPENHELP_HELP), NewObjClear(COpenHelp));
}
