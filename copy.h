#ifndef FLAT_INCLUDES
#include "type.h"
#endif

void * glb_toc_copy_mem(glb_toc * target);

inline static void glb_copy_mem(glb * target)
{
    target->toc_mem = glb_toc_copy_mem(&target->toc);
}
