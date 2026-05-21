#pragma once
#include <uf.h>
#include <uf_modl.h>

int CreateExtrusion(
    tag_t            sketchTag,
    const char*      startVal,
    const char*      endVal,
    const double     direction[3],
    UF_FEATURE_SIGN  sign
);
