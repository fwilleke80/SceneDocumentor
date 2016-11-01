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

void SDKCrashHandler(CHAR *crashinfo)
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
	if (!RegisterTComment()) return FALSE;
	if (!RegisterCreateCommentNulls()) return FALSE;
	if (!RegisterRemoveAllTags()) return FALSE;
	if (!RegisterCommandPlugin(SCENEDOCUMENTOR_SEP01_PLUGID, "#$30--", 0, NULL, "", gNew CommandData)) return FALSE;
	if (!RegisterCreateDocument()) return FALSE;
	if (!RegisterCommentInspector()) return FALSE;
	if (!RegisterCommandPlugin(SCENEDOCUMENTOR_SEP02_PLUGID, "#$60--", 0, NULL, "", gNew CommandData)) return FALSE;
	if (!RegisterOpenHelp()) return FALSE;

	return TRUE;
}

void PluginEnd(void)
{
	// Called when plugin is terminated
}

Bool PluginMessage(LONG id, void *data)
{
	// React to messages
	switch (id)
	{
		case C4DPL_INIT_SYS:
			// Don't start plugin without resources
			if (!resource.Init()) return FALSE;
			return TRUE;
	}

	return TRUE;
}
