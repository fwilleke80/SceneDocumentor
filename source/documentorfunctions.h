#ifndef DOCUMENTORFUNCTIONS_H_
#define DOCUMENTORFUNCTIONS_H_

#include "c4d.h"


Bool ObjectHasComments(BaseObject *op);
Bool SearchForComments(BaseObject *op);
String GetSelectedCycleElementName(BaseList2D *node, Int32 cycle_id);

#endif /* DOCUMENTORFUNCTIONS_H_ */
