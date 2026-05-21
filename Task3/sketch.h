#pragma once
#include <uf.h>

int CreateSketchOnPlane(
    const char* name,
    double      zOffset,
    tag_t&      sketchTag
);

int AddObjectsToSketch(
    tag_t       sketchTag,
    int         count,
    tag_t       objects[]
);
