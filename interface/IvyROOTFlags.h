#ifndef IVYROOTFLAGS_H
#define IVYROOTFLAGS_H


#include "RVersion.h"


#if ROOT_VERSION_CODE < ROOT_VERSION(6,18,0)
#ifndef _IVY_ROOT_HAS_ITERATORS_
#define _IVY_ROOT_HAS_ITERATORS_
#endif
#endif


#endif
