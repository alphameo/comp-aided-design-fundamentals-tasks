#include <uf.h>
#include "geometry.h"
#include "sketch.h"
#include "features.h"

void ufusr(char* param, int* retcode, int paramLen)
{
    *retcode = 0;

    int errorCode = UF_initialize();
    if (errorCode != 0)
    {
        *retcode = errorCode;
        return;
    }

    const double Z_SKETCH = 119.0;
    const double R_OUTER  = 70.0;
    const double R_INNER  = 25.0;

    tag_t sketchTag   = NULL_TAG;
    tag_t outerCircle = NULL_TAG;
    tag_t innerCircle = NULL_TAG;

    errorCode = CreateSketchOnPlane("Sketch_Base", Z_SKETCH, sketchTag);
    if (errorCode != 0) goto cleanup;

    errorCode = CreateCircle(0.0, 0.0, Z_SKETCH, R_OUTER, outerCircle);
    if (errorCode != 0) goto cleanup;

    errorCode = CreateCircle(0.0, 0.0, Z_SKETCH, R_INNER, innerCircle);
    if (errorCode != 0) goto cleanup;

    {
        tag_t objects[2] = { outerCircle, innerCircle };
        errorCode = AddObjectsToSketch(sketchTag, 2, objects);
        if (errorCode != 0) goto cleanup;
    }

    {
        double dir[3] = { 0.0, 0.0, 1.0 };
        errorCode = CreateExtrusion(sketchTag, "0", "4", dir, UF_NULLSIGN);
        if (errorCode != 0) goto cleanup;
    }

cleanup:
    if (errorCode != 0)
    {
        *retcode = errorCode;
    }

    UF_terminate();
}

int ufusr_ask_unload(void)
{
    return UF_UNLOAD_IMMEDIATELY;
}
