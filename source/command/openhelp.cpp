#include "c4d.h"
#include "c4d_symbols.h" 
#include "openhelp.h"
#include "tCommentClass.h"

// ID obtained from www.plugincafe.com
#define ID_OPENHELP	1024753

class COpenHelp : public CommandData
{
	INSTANCEOF(COpenHelp,CommandData)

	public:
		virtual Bool Execute(BaseDocument* doc);
		virtual Bool Message(LONG type, void* data);
};

Bool COpenHelp::Execute(BaseDocument* doc)
{
	if (!GeExecuteFile(HelpFile + "index.html"))
		GeOutString(GeLoadString(IDS_HELP_ERROR), GEMB_OK);

	return TRUE;
}

Bool COpenHelp::Message(LONG type, void* data)
{
	return SUPER::Message(type,data);
}

Bool RegisterOpenHelp(void)
{
	// decide by name if the plugin shall be registered - just for user convenience
	String name=GeLoadString(IDS_OPENHELP); if (!name.Content()) return TRUE;
	return RegisterCommandPlugin(ID_OPENHELP, name, 0, AutoBitmap("COpenHelp.tif"), GeLoadString(IDS_OPENHELP_HELP), gNew COpenHelp);
}
