#include "c4d.h"

#ifndef __SCENEDOCUMENTOR_FUNCTIONS__
#define __SCENEDOCUMENTOR_FUNCTIONS__

Bool ObjectHasComments(BaseObject *op);
Bool SearchForComments(BaseObject *op);
String GetSelectedCycleElementName(BaseList2D *node, LONG cycle_id);

#endif
