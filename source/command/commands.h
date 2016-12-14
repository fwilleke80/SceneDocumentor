#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "c4d.h"

String GetCommentsText(BaseObject *op);
String GetObjectTitle(BaseObject *op, const String Decor = "==");
String GetChildObjectsComments(BaseObject *op);
void IterateObjectsAndTags(BaseObject *op, Int32 &counter, Bool PutOnTopLevel, BaseObject *parent, Bool IncludeOpName, Bool IncludeTitle, Bool IncludeIcon, Bool JustTest = false);
String ProcessTemplate(String tmpl, BaseDocument *doc, BaseContainer *settings);
void IterateObjectsForDocumentation(BaseObject *op, Int32 &counter, BaseFile *bf, Int32 hLevel, Bool JustTest = false);
void IterateObjectsForRemoval(BaseObject *op, Int32 &counter, Bool SelectedOnly, Bool JustTest);

#endif /* COMMANDS_H_ */
