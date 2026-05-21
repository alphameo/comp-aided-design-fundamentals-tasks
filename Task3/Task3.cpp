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

    const double Z_PROKLADKA1       = 119.0;
    const double R_PROKLADKA1_OUTER = 70.0;
    const double R_PROKLADKA1_INNER = 25.0;
    const double Z_PROKLADKA1_TOP   = 123.0;
    const double R_ZAGLUSHKA        = R_PROKLADKA1_OUTER;
    const double Z_ZAGLUSHKA_TOP    = 139.0;
    const double R_HOLE             = 6.5;
    const double R_PITCH            = 50.0;
    const double R_ZAGLUSHKA_BORE   = 27.5;

    const double positions[4][2] = {
        {  R_PITCH,  0.0 },
        {  0.0,  R_PITCH },
        { -R_PITCH,  0.0 },
        {  0.0, -R_PITCH }
    };

    const char* prokladka1HoleNames[4] = {
        "Sketch_Prok1_Hole_0",
        "Sketch_Prok1_Hole_1",
        "Sketch_Prok1_Hole_2",
        "Sketch_Prok1_Hole_3"
    };

    const char* zaglushkaHoleNames[4] = {
        "Sketch_Zag_Hole_0",
        "Sketch_Zag_Hole_1",
        "Sketch_Zag_Hole_2",
        "Sketch_Zag_Hole_3"
    };

    tag_t prokladka1Sketch      = NULL_TAG;
    tag_t prokladka1Outer       = NULL_TAG;
    tag_t prokladka1Inner       = NULL_TAG;
    tag_t zaglushkaSketch       = NULL_TAG;
    tag_t zaglushkaCircle       = NULL_TAG;
    tag_t zaglushkaBoreSketch   = NULL_TAG;
    tag_t zaglushkaBoreCircle   = NULL_TAG;

    errorCode = CreateSketchOnPlane("Sketch_Prokladka1", Z_PROKLADKA1, prokladka1Sketch);
    if (errorCode != 0) goto cleanup;

    errorCode = CreateCircle(0.0, 0.0, Z_PROKLADKA1, R_PROKLADKA1_OUTER, prokladka1Outer);
    if (errorCode != 0) goto cleanup;

    errorCode = CreateCircle(0.0, 0.0, Z_PROKLADKA1, R_PROKLADKA1_INNER, prokladka1Inner);
    if (errorCode != 0) goto cleanup;

    {
        tag_t objects[2] = { prokladka1Outer, prokladka1Inner };
        errorCode = AddObjectsToSketch(prokladka1Sketch, 2, objects);
        if (errorCode != 0) goto cleanup;
    }

    {
        double dir[3] = { 0.0, 0.0, 1.0 };
        errorCode = CreateExtrusion(prokladka1Sketch, "0", "4", dir, UF_NULLSIGN);
        if (errorCode != 0) goto cleanup;
    }

    errorCode = BuildHoles(positions, prokladka1HoleNames, Z_PROKLADKA1_TOP, R_HOLE, "4");
    if (errorCode != 0) goto cleanup;

    errorCode = CreateSketchOnPlane("Sketch_Zaglushka", Z_PROKLADKA1_TOP, zaglushkaSketch);
    if (errorCode != 0) goto cleanup;

    errorCode = CreateCircle(0.0, 0.0, Z_PROKLADKA1_TOP, R_ZAGLUSHKA, zaglushkaCircle);
    if (errorCode != 0) goto cleanup;

    errorCode = AddObjectsToSketch(zaglushkaSketch, 1, &zaglushkaCircle);
    if (errorCode != 0) goto cleanup;

    {
        double dir[3] = { 0.0, 0.0, 1.0 };
        errorCode = CreateExtrusion(zaglushkaSketch, "0", "16", dir, UF_NULLSIGN);
        if (errorCode != 0) goto cleanup;
    }

    errorCode = BuildHoles(positions, zaglushkaHoleNames, Z_ZAGLUSHKA_TOP, R_HOLE, "16");
    if (errorCode != 0) goto cleanup;

    errorCode = CreateSketchOnPlane("Sketch_Zag_Bore", Z_ZAGLUSHKA_TOP, zaglushkaBoreSketch);
    if (errorCode != 0) goto cleanup;

    errorCode = CreateCircle(0.0, 0.0, Z_ZAGLUSHKA_TOP, R_ZAGLUSHKA_BORE, zaglushkaBoreCircle);
    if (errorCode != 0) goto cleanup;

    errorCode = AddObjectsToSketch(zaglushkaBoreSketch, 1, &zaglushkaBoreCircle);
    if (errorCode != 0) goto cleanup;

    {
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = CreateExtrusion(zaglushkaBoreSketch, "0", "5", dir, UF_NEGATIVE);
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
