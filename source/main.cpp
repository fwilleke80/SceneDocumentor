#include "c4d.h"

// IDs for menu separators, obtained from www.plugincafe.com
#define SCENEDOCUMENTOR_SEP01_PLUGID 1024751
#define SCENEDOCUMENTOR_SEP02_PLUGID 1024752

// forward declarations
Bool RegisterTComment(void);
Bool RegisterCreateCommentNulls(void);
Bool RegisterRemoveAllTags(void);
Bool RegisterCreateDocument(void);
Bool RegisterOpenHelp(void);
Bool RegisterCommentInspector(void);

// Declare Crash handler
C4D_CrashHandler old_handler;

#define VersionString "SceneDocumentor 1.1.7"

void SDKCrashHandler(Char *crashinfo)
{
	// don't forget to call the original handler!!!
	if (old_handler) (*old_handler)(crashinfo);
}

Bool PluginStart(void)
{
	// Installing the crash handler
	old_handler = C4DOS.CrashHandler;		// backup the original handler (must be called!)
	C4DOS.CrashHandler = SDKCrashHandler;	// insert the own handler
	
	// Register plugins
	if (!RegisterTComment()) return false;
	if (!RegisterCreateCommentNulls()) return false;
	if (!RegisterRemoveAllTags()) return false;
	if (!RegisterCommandPlugin(SCENEDOCUMENTOR_SEP01_PLUGID, "#$30--", 0, nullptr, "", NewObjClear(CommandData))) return false;
	if (!RegisterCreateDocument()) return false;
	if (!RegisterCommentInspector()) return false;
	if (!RegisterCommandPlugin(SCENEDOCUMENTOR_SEP02_PLUGID, "#$60--", 0, nullptr, "", NewObjClear(CommandData))) return false;
	if (!RegisterOpenHelp()) return false;

	return true;
}

void PluginEnd(void)
{
	// Called when plugin is terminated
}

Bool PluginMessage(Int32 id, void *data)
{
	// React to messages
	switch (id)
	{
		case C4DPL_INIT_SYS:
			// Don't start plugin without resources
			if (!resource.Init()) return false;
			return true;
	}

	return true;
}
