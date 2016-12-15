#include "main.h"


#define PLUGIN_VERSION String("1.1")


Bool PluginStart(void)
{
	GePrint("SceneDocumentor " + PLUGIN_VERSION);
	
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
