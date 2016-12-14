#ifndef MAIN_H_
#define MAIN_H_

#include "c4d.h"


// IDs for menu separators, obtained from www.plugincafe.com
#define SCENEDOCUMENTOR_SEP01_PLUGID 1024751
#define SCENEDOCUMENTOR_SEP02_PLUGID 1024752


#define VersionString "SceneDocumentor 1.1.7"


// forward declarations
Bool RegisterTComment(void);
Bool RegisterCreateCommentNulls(void);
Bool RegisterRemoveAllTags(void);
Bool RegisterCreateDocument(void);
Bool RegisterOpenHelp(void);
Bool RegisterCommentInspector(void);


#endif /* MAIN_H_ */
