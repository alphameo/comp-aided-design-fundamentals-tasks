#include <uf.h>
#include "geometry.h"
#include "sketch.h"
#include "features.h"

static int BuildHoles(
    const double positions[4][2],
    const char*  names[4],
    double       zPlane,
    double       radius,
    const char*  limit)
{
    for (int i = 0; i < 4; ++i)
    {
        tag_t holeSketch = NULL_TAG;
        tag_t holeCircle = NULL_TAG;

        int errorCode = CreateSketchOnPlane(names[i], zPlane, holeSketch);
        if (errorCode != 0) return errorCode;

        errorCode = CreateCircle(
            positions[i][0], positions[i][1],
            zPlane, radius, holeCircle);
        if (errorCode != 0) return errorCode;

        errorCode = AddObjectsToSketch(holeSketch, 1, &holeCircle);
        if (errorCode != 0) return errorCode;

        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = CreateExtrusion(holeSketch, "0", limit, dir, UF_NEGATIVE);
        if (errorCode != 0) return errorCode;
    }
    return 0;
}

void ufusr(char* param, int* retcode, int paramLen)
{
    *retcode = 0;

    int errorCode = UF_initialize();
    if (errorCode != 0)
    {
        *retcode = errorCode;
        return;
    }

    const double Z_SKETCH  = 119.0;
    const double R_OUTER   = 70.0;
    const double R_INNER   = 25.0;
    const double Z_TOP     = 123.0;
    const double R_CYL     = R_OUTER;
    const double Z_CYL_TOP = 139.0;
    const double R_HOLE    = 6.5;
    const double R_PITCH   = 50.0;
    const double R_BORE    = 27.5;

    const double positions[4][2] = {
        {  R_PITCH,  0.0 },
        {  0.0,  R_PITCH },
        { -R_PITCH,  0.0 },
        {  0.0, -R_PITCH }
    };

    const char* holeNames[4] = {
        "Sketch_Hole_0",
        "Sketch_Hole_1",
        "Sketch_Hole_2",
        "Sketch_Hole_3"
    };

    const char* topHoleNames[4] = {
        "Sketch_TopHole_0",
        "Sketch_TopHole_1",
        "Sketch_TopHole_2",
        "Sketch_TopHole_3"
    };

    tag_t sketchTag   = NULL_TAG;
    tag_t outerCircle = NULL_TAG;
    tag_t innerCircle = NULL_TAG;
    tag_t cylSketch   = NULL_TAG;
    tag_t cylCircle   = NULL_TAG;
    tag_t boreSketch  = NULL_TAG;
    tag_t boreCircle  = NULL_TAG;

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

    errorCode = BuildHoles(positions, holeNames, Z_TOP, R_HOLE, "4");
    if (errorCode != 0) goto cleanup;

    errorCode = CreateSketchOnPlane("Sketch_Cylinder", Z_TOP, cylSketch);
    if (errorCode != 0) goto cleanup;

    errorCode = CreateCircle(0.0, 0.0, Z_TOP, R_CYL, cylCircle);
    if (errorCode != 0) goto cleanup;

    errorCode = AddObjectsToSketch(cylSketch, 1, &cylCircle);
    if (errorCode != 0) goto cleanup;

    {
        double dir[3] = { 0.0, 0.0, 1.0 };
        errorCode = CreateExtrusion(cylSketch, "0", "16", dir, UF_NULLSIGN);
        if (errorCode != 0) goto cleanup;
    }

    errorCode = BuildHoles(positions, topHoleNames, Z_CYL_TOP, R_HOLE, "16");
    if (errorCode != 0) goto cleanup;

    errorCode = CreateSketchOnPlane("Sketch_Bore", Z_CYL_TOP, boreSketch);
    if (errorCode != 0) goto cleanup;

    errorCode = CreateCircle(0.0, 0.0, Z_CYL_TOP, R_BORE, boreCircle);
    if (errorCode != 0) goto cleanup;

    errorCode = AddObjectsToSketch(boreSketch, 1, &boreCircle);
    if (errorCode != 0) goto cleanup;

    {
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = CreateExtrusion(boreSketch, "0", "5", dir, UF_NEGATIVE);
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
