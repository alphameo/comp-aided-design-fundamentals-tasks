#include "build.h"
#include "geometry.h"
#include "sketch.h"
#include "features.h"

#include <uf.h>
#include <uf_curve.h>
#include <stdio.h>

static const double Z_PROKLADKA1       = 119.0;
static const double R_PROKLADKA1_OUTER = 70.0;
static const double R_PROKLADKA1_INNER = 25.0;
static const double Z_PROKLADKA1_TOP   = 123.0;
static const double R_ZAGLUSHKA        = R_PROKLADKA1_OUTER;
static const double Z_ZAGLUSHKA_TOP    = 139.0;
static const double R_HOLE             = 6.5;
static const double R_PITCH            = 50.0;
static const double R_ZAGLUSHKA_BORE   = 27.5;
static const double KP_HOLE_R          = 17.5;
static const double KP_HOLE_Z          = 59.0;

static const double Z_ORIGIN        = 0.0;
static const double PLATE_HALF      = 80.0;
static const double PLATE_RADIUS    = 20.0;
static const double TUBE_OUTER_R    = 30.0;
static const double TUBE_INNER_R    = R_PROKLADKA1_INNER;
static const double TUBE_HEIGHT     = 105.0;
static const double CORNER_HOLE_R   = 7.5;
static const double CORNER_HOLE_POS = 62.5;
static const double PLATE_L         = PLATE_HALF - PLATE_RADIUS;
static const double PLATE_S         = PLATE_RADIUS * 0.7071067811865476;
static const char   TUBE_HEIGHT_STR[] = "105";

static const double PITCH_POS[4][2] = {
    {  R_PITCH,  0.0 },
    {  0.0,  R_PITCH },
    { -R_PITCH,  0.0 },
    {  0.0, -R_PITCH }
};

double g_bobLength = 30.0;

static const double BOB_LENGTHS[] = { 20.0, 30.0, 40.0 };
static const int NUM_BOB_LENGTHS = sizeof(BOB_LENGTHS) / sizeof(BOB_LENGTHS[0]);

void SetBobLength(int index)
{
    if (index < 0) index = 0;
    if (index >= NUM_BOB_LENGTHS) index = NUM_BOB_LENGTHS - 1;
    g_bobLength = BOB_LENGTHS[index];
}

static int BuildHoles(
    const double positions[4][2],
    const char*  names[4],
    double       zPlane,
    double       radius,
    const char*  limit,
    const double dir[3])
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

        errorCode = CreateExtrusion(holeSketch, "0", limit, dir, UF_NEGATIVE);
        if (errorCode != 0) return errorCode;
    }
    return 0;
}


int BuildProkladka1(void)
{
    tag_t prokladka1Sketch = NULL_TAG;
    tag_t prokladka1Outer  = NULL_TAG;
    tag_t prokladka1Inner  = NULL_TAG;

    int errorCode = CreateSketchOnPlane("Sketch_Prokladka1", Z_PROKLADKA1, prokladka1Sketch);
    if (errorCode != 0) return errorCode;

    errorCode = CreateCircle(0.0, 0.0, Z_PROKLADKA1, R_PROKLADKA1_OUTER, prokladka1Outer);
    if (errorCode != 0) return errorCode;

    errorCode = CreateCircle(0.0, 0.0, Z_PROKLADKA1, R_PROKLADKA1_INNER, prokladka1Inner);
    if (errorCode != 0) return errorCode;

    {
        tag_t objects[2] = { prokladka1Outer, prokladka1Inner };
        errorCode = AddObjectsToSketch(prokladka1Sketch, 2, objects);
        if (errorCode != 0) return errorCode;
    }

    {
        double dir[3] = { 0.0, 0.0, 1.0 };
        errorCode = CreateExtrusion(prokladka1Sketch, "0", "4", dir, UF_NULLSIGN);
        if (errorCode != 0) return errorCode;
    }

    {
        const char* names[4] = {
            "Sketch_Prok1_Hole_0", "Sketch_Prok1_Hole_1",
            "Sketch_Prok1_Hole_2", "Sketch_Prok1_Hole_3"
        };
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = BuildHoles(PITCH_POS, names, Z_PROKLADKA1_TOP, R_HOLE, "4", dir);
        if (errorCode != 0) return errorCode;
    }

    return 0;
}


int BuildZaglushka(void)
{
    tag_t zaglushkaSketch     = NULL_TAG;
    tag_t zaglushkaCircle     = NULL_TAG;
    tag_t zaglushkaBoreSketch = NULL_TAG;
    tag_t zaglushkaBoreCircle = NULL_TAG;

    int errorCode = CreateSketchOnPlane("Sketch_Zaglushka", Z_PROKLADKA1_TOP, zaglushkaSketch);
    if (errorCode != 0) return errorCode;

    errorCode = CreateCircle(0.0, 0.0, Z_PROKLADKA1_TOP, R_ZAGLUSHKA, zaglushkaCircle);
    if (errorCode != 0) return errorCode;

    errorCode = AddObjectsToSketch(zaglushkaSketch, 1, &zaglushkaCircle);
    if (errorCode != 0) return errorCode;

    {
        double dir[3] = { 0.0, 0.0, 1.0 };
        errorCode = CreateExtrusion(zaglushkaSketch, "0", "16", dir, UF_NULLSIGN);
        if (errorCode != 0) return errorCode;
    }

    {
        const char* names[4] = {
            "Sketch_Zag_Hole_0", "Sketch_Zag_Hole_1",
            "Sketch_Zag_Hole_2", "Sketch_Zag_Hole_3"
        };
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = BuildHoles(PITCH_POS, names, Z_ZAGLUSHKA_TOP, R_HOLE, "16", dir);
        if (errorCode != 0) return errorCode;
    }

    errorCode = CreateSketchOnPlane("Sketch_Zag_Bore", Z_ZAGLUSHKA_TOP, zaglushkaBoreSketch);
    if (errorCode != 0) return errorCode;

    errorCode = CreateCircle(0.0, 0.0, Z_ZAGLUSHKA_TOP, R_ZAGLUSHKA_BORE, zaglushkaBoreCircle);
    if (errorCode != 0) return errorCode;

    errorCode = AddObjectsToSketch(zaglushkaBoreSketch, 1, &zaglushkaBoreCircle);
    if (errorCode != 0) return errorCode;

    {
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = CreateExtrusion(zaglushkaBoreSketch, "0", "5", dir, UF_NEGATIVE);
        if (errorCode != 0) return errorCode;
    }

    return 0;
}


int BuildProkladka2(void)
{
    const double Z_PROKLADKA2 = Z_ORIGIN;

    tag_t prokladka2Sketch = NULL_TAG;
    tag_t prok2Curves[8]   = { NULL_TAG, NULL_TAG, NULL_TAG, NULL_TAG,
                               NULL_TAG, NULL_TAG, NULL_TAG, NULL_TAG };

    int errorCode = CreateSketchOnPlane("Sketch_Prokladka2", Z_PROKLADKA2, prokladka2Sketch);
    if (errorCode != 0) return errorCode;

    double topLeft0[3]    = { -PLATE_L,  PLATE_HALF, Z_PROKLADKA2 };
    double topRight0[3]   = {  PLATE_L,  PLATE_HALF, Z_PROKLADKA2 };
    double rightTop0[3]   = {  PLATE_HALF,  PLATE_L, Z_PROKLADKA2 };
    double rightBot0[3]   = {  PLATE_HALF, -PLATE_L, Z_PROKLADKA2 };
    double botRight0[3]   = {  PLATE_L, -PLATE_HALF, Z_PROKLADKA2 };
    double botLeft0[3]    = { -PLATE_L, -PLATE_HALF, Z_PROKLADKA2 };
    double leftBot0[3]    = { -PLATE_HALF, -PLATE_L, Z_PROKLADKA2 };
    double leftTop0[3]    = { -PLATE_HALF,  PLATE_L, Z_PROKLADKA2 };

    double arcTRmid[3]    = {  PLATE_L + PLATE_S,  PLATE_L + PLATE_S, Z_PROKLADKA2 };
    double arcBRmid[3]    = {  PLATE_L + PLATE_S, -PLATE_L - PLATE_S, Z_PROKLADKA2 };
    double arcBLmid[3]    = { -PLATE_L - PLATE_S, -PLATE_L - PLATE_S, Z_PROKLADKA2 };
    double arcTLmid[3]    = { -PLATE_L - PLATE_S,  PLATE_L + PLATE_S, Z_PROKLADKA2 };

    errorCode = CreateLine(topLeft0, topRight0, prok2Curves[0]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateArc(topRight0, arcTRmid, rightTop0, prok2Curves[1]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateLine(rightTop0, rightBot0, prok2Curves[2]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateArc(rightBot0, arcBRmid, botRight0, prok2Curves[3]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateLine(botRight0, botLeft0, prok2Curves[4]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateArc(botLeft0, arcBLmid, leftBot0, prok2Curves[5]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateLine(leftBot0, leftTop0, prok2Curves[6]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateArc(leftTop0, arcTLmid, topLeft0, prok2Curves[7]);
    if (errorCode != 0) return errorCode;

    errorCode = AddObjectsToSketch(prokladka2Sketch, 8, prok2Curves);
    if (errorCode != 0) return errorCode;

    {
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = CreateExtrusion(prokladka2Sketch, "0", "5", dir, UF_NULLSIGN);
        if (errorCode != 0) return errorCode;
    }

    {
        const double prok2HolePos[4][2] = {
            {  CORNER_HOLE_POS,  CORNER_HOLE_POS },
            { -CORNER_HOLE_POS,  CORNER_HOLE_POS },
            { -CORNER_HOLE_POS, -CORNER_HOLE_POS },
            {  CORNER_HOLE_POS, -CORNER_HOLE_POS }
        };
        const char* prok2HoleNames[4] = {
            "Sketch_Prok2_Hole_0", "Sketch_Prok2_Hole_1",
            "Sketch_Prok2_Hole_2", "Sketch_Prok2_Hole_3"
        };
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = BuildHoles(prok2HolePos, prok2HoleNames, Z_PROKLADKA2, CORNER_HOLE_R, "5", dir);
        if (errorCode != 0) return errorCode;
    }

    {
        tag_t prok2CenterSketch = NULL_TAG;
        tag_t prok2CenterCircle = NULL_TAG;

        errorCode = CreateSketchOnPlane("Sketch_Prok2_Center", Z_PROKLADKA2, prok2CenterSketch);
        if (errorCode != 0) return errorCode;

        const double PROK2_CENTER_R = 40.0;
        errorCode = CreateCircle(0.0, 0.0, Z_PROKLADKA2, PROK2_CENTER_R, prok2CenterCircle);
        if (errorCode != 0) return errorCode;

        errorCode = AddObjectsToSketch(prok2CenterSketch, 1, &prok2CenterCircle);
        if (errorCode != 0) return errorCode;

        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = CreateExtrusion(prok2CenterSketch, "0", "5", dir, UF_NEGATIVE);
        if (errorCode != 0) return errorCode;
    }

    return 0;
}


int BuildKorpusFlanetz1(void)
{
    const double Z_KORPUS_FL1    = Z_ORIGIN;
    const double KR1_CENTER_R = 25.0;

    tag_t kr1Sketch = NULL_TAG;
    tag_t kr1Curves[8] = { NULL_TAG, NULL_TAG, NULL_TAG, NULL_TAG,
                           NULL_TAG, NULL_TAG, NULL_TAG, NULL_TAG };

    int errorCode = CreateSketchOnPlane("Sketch_Korpus_Flanetz1", Z_KORPUS1, kr1Sketch);
    if (errorCode != 0) return errorCode;

    double topLeft0[3]    = { -PLATE_L,  PLATE_HALF, Z_KORPUS_FL1 };
    double topRight0[3]   = {  PLATE_L,  PLATE_HALF, Z_KORPUS_FL1 };
    double rightTop0[3]   = {  PLATE_HALF,  PLATE_L, Z_KORPUS_FL1 };
    double rightBot0[3]   = {  PLATE_HALF, -PLATE_L, Z_KORPUS_FL1 };
    double botRight0[3]   = {  PLATE_L, -PLATE_HALF, Z_KORPUS_FL1 };
    double botLeft0[3]    = { -PLATE_L, -PLATE_HALF, Z_KORPUS_FL1 };
    double leftBot0[3]    = { -PLATE_HALF, -PLATE_L, Z_KORPUS_FL1 };
    double leftTop0[3]    = { -PLATE_HALF,  PLATE_L, Z_KORPUS_FL1 };

    double arcTRmid[3]    = {  PLATE_L + PLATE_S,  PLATE_L + PLATE_S, Z_KORPUS_FL1 };
    double arcBRmid[3]    = {  PLATE_L + PLATE_S, -PLATE_L - PLATE_S, Z_KORPUS_FL1 };
    double arcBLmid[3]    = { -PLATE_L - PLATE_S, -PLATE_L - PLATE_S, Z_KORPUS_FL1 };
    double arcTLmid[3]    = { -PLATE_L - PLATE_S,  PLATE_L + PLATE_S, Z_KORPUS_FL1 };

    errorCode = CreateLine(topLeft0, topRight0, kr1Curves[0]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateArc(topRight0, arcTRmid, rightTop0, kr1Curves[1]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateLine(rightTop0, rightBot0, kr1Curves[2]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateArc(rightBot0, arcBRmid, botRight0, kr1Curves[3]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateLine(botRight0, botLeft0, kr1Curves[4]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateArc(botLeft0, arcBLmid, leftBot0, kr1Curves[5]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateLine(leftBot0, leftTop0, kr1Curves[6]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateArc(leftTop0, arcTLmid, topLeft0, kr1Curves[7]);
    if (errorCode != 0) return errorCode;

    errorCode = AddObjectsToSketch(kr1Sketch, 8, kr1Curves);
    if (errorCode != 0) return errorCode;

    {
        double dir[3] = { 0.0, 0.0, 1.0 };
        errorCode = CreateExtrusion(kr1Sketch, "0", "14", dir, UF_NULLSIGN);
        if (errorCode != 0) return errorCode;
    }

    {
        const double kr1HolePos[4][2] = {
            {  CORNER_HOLE_POS,  CORNER_HOLE_POS },
            { -CORNER_HOLE_POS,  CORNER_HOLE_POS },
            { -CORNER_HOLE_POS, -CORNER_HOLE_POS },
            {  CORNER_HOLE_POS, -CORNER_HOLE_POS }
        };
        const char* kr1HoleNames[4] = {
            "Sketch_Kfl_Hole_0", "Sketch_Kfl1_Hole_1",
            "Sketch_Kfl_Hole_2", "Sketch_Kfl1_Hole_3"
        };
        double dir[3] = { 0.0, 0.0, 1.0 };
        errorCode = BuildHoles(kr1HolePos, kr1HoleNames, Z_KORPUS_FL1, CORNER_HOLE_R, "14", dir);
        if (errorCode != 0) return errorCode;
    }

    {
        tag_t kr1CenterSketch = NULL_TAG;
        tag_t kr1CenterCircle = NULL_TAG;

        errorCode = CreateSketchOnPlane("Sketch_Kfl1_Center", Z_KORPUS1, kr1CenterSketch);
        if (errorCode != 0) return errorCode;

        errorCode = CreateCircle(0.0, 0.0, Z_KORPUS1, KR1_CENTER_R, kr1CenterCircle);
        if (errorCode != 0) return errorCode;

        errorCode = AddObjectsToSketch(kr1CenterSketch, 1, &kr1CenterCircle);
        if (errorCode != 0) return errorCode;

        double dir[3] = { 0.0, 0.0, 1.0 };
        errorCode = CreateExtrusion(kr1CenterSketch, "0", "14", dir, UF_NEGATIVE);
        if (errorCode != 0) return errorCode;
    }

    return 0;
}


int BuildKorpusPatrubok(void)
{
    const double Z_KORPUS_PATRUBOK = Z_ORIGIN;

    tag_t kpSketch = NULL_TAG;
    tag_t kpOuter  = NULL_TAG;
    tag_t kpInner  = NULL_TAG;

    int errorCode = CreateSketchOnPlane("Sketch_Korpus_Patrubok", Z_KORPUS_PATRUBOK, kpSketch);
    if (errorCode != 0) return errorCode;

    errorCode = CreateCircle(0.0, 0.0, Z_KORPUS_PATRUBOK, TUBE_OUTER_R, kpOuter);
    if (errorCode != 0) return errorCode;

    errorCode = CreateCircle(0.0, 0.0, Z_KORPUS_PATRUBOK, TUBE_INNER_R, kpInner);
    if (errorCode != 0) return errorCode;

    {
        tag_t objs[2] = { kpOuter, kpInner };
        errorCode = AddObjectsToSketch(kpSketch, 2, objs);
        if (errorCode != 0) return errorCode;
    }

    {
        double dir[3] = { 0.0, 0.0, 1.0 };
        errorCode = CreateExtrusion(kpSketch, "0", TUBE_HEIGHT_STR, dir, UF_POSITIVE);
        if (errorCode != 0) return errorCode;
    }

    {
        tag_t kpHoleSketch = NULL_TAG;

        errorCode = CreateSketchOnPlaneZX("Sketch_KP_Hole", kpHoleSketch);
        if (errorCode != 0) return errorCode;

        tag_t kpHoleCircle = NULL_TAG;
        double pt1[3] = { KP_HOLE_R, 0.0, KP_HOLE_Z };
        double pt2[3] = { -KP_HOLE_R, 0.0, KP_HOLE_Z };
        double pt3[3] = { 0.0, 0.0, KP_HOLE_Z + KP_HOLE_R };
        errorCode = UF_CURVE_create_arc_thru_3pts(2, pt1, pt2, pt3, &kpHoleCircle);
        if (errorCode != 0) return errorCode;

        errorCode = AddObjectsToSketch(kpHoleSketch, 1, &kpHoleCircle);
        if (errorCode != 0) return errorCode;

        double dir[3] = { 0.0, 1.0, 0.0 };
        errorCode = CreateExtrusion(kpHoleSketch, "0", "60", dir, UF_NEGATIVE);
        if (errorCode != 0) return errorCode;
    }

    return 0;
}


int BuildKorpusFlanetz2(void)
{
    tag_t kfl2Sketch = NULL_TAG;
    tag_t kfl2Outer  = NULL_TAG;
    tag_t kfl2Inner  = NULL_TAG;

    int errorCode = CreateSketchOnPlane("Sketch_Korpus_Flanetz2", Z_PROKLADKA1, kr2Sketch);
    if (errorCode != 0) return errorCode;

    errorCode = CreateCircle(0.0, 0.0, Z_PROKLADKA1, R_PROKLADKA1_OUTER, kr2Outer);
    if (errorCode != 0) return errorCode;

    errorCode = CreateCircle(0.0, 0.0, Z_PROKLADKA1, R_PROKLADKA1_INNER, kr2Inner);
    if (errorCode != 0) return errorCode;

    {
        tag_t objs[2] = { kr2Outer, kr2Inner };
        errorCode = AddObjectsToSketch(kr2Sketch, 2, objs);
        if (errorCode != 0) return errorCode;
    }

    {
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = CreateExtrusion(kr2Sketch, "0", "25", dir, UF_POSITIVE);
        if (errorCode != 0) return errorCode;
    }

    {
        const char* kr2HoleNames[4] = {
            "Sketch_Kfl2_Hole_0", "Sketch_Kfl2_Hole_1",
            "Sketch_Kfl2_Hole_2", "Sketch_Kfl2_Hole_3"
        };
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = BuildHoles(PITCH_POS, kr2HoleNames, Z_PROKLADKA1, R_HOLE, "25", dir);
        if (errorCode != 0) return errorCode;
    }

    return 0;
}


int BuildBobyshka(void)
{
    const double BOB_Y_PLANE = TUBE_OUTER_R;
    const double BOB_Z       = KP_HOLE_Z;
    const double BOB_OUTER_R = KP_HOLE_R;
    const double BOB_INNER_R = 10.5;

    int errorCode;
    tag_t bobSketch = NULL_TAG;

    errorCode = CreateSketchOnPlaneParallelZX("Sketch_Bobyshka", BOB_Y_PLANE, bobSketch);
    if (errorCode != 0) return errorCode;

    tag_t bobOuter = NULL_TAG;
    {
        double pt1[3] = { BOB_OUTER_R, BOB_Y_PLANE, BOB_Z };
        double pt2[3] = { -BOB_OUTER_R, BOB_Y_PLANE, BOB_Z };
        double pt3[3] = { 0.0, BOB_Y_PLANE, BOB_Z + BOB_OUTER_R };
        errorCode = UF_CURVE_create_arc_thru_3pts(2, pt1, pt2, pt3, &bobOuter);
        if (errorCode != 0) return errorCode;
    }

    tag_t bobInner = NULL_TAG;
    {
        double pt1[3] = { BOB_INNER_R, BOB_Y_PLANE, BOB_Z };
        double pt2[3] = { -BOB_INNER_R, BOB_Y_PLANE, BOB_Z };
        double pt3[3] = { 0.0, BOB_Y_PLANE, BOB_Z + BOB_INNER_R };
        errorCode = UF_CURVE_create_arc_thru_3pts(2, pt1, pt2, pt3, &bobInner);
        if (errorCode != 0) return errorCode;
    }

    {
        tag_t objs[2] = { bobOuter, bobInner };
        errorCode = AddObjectsToSketch(bobSketch, 2, objs);
        if (errorCode != 0) return errorCode;
    }

    {
        double dir[3] = { 0.0, -1.0, 0.0 };
        errorCode = CreateExtrusion(bobSketch, "0", "30", dir, UF_POSITIVE);
        if (errorCode != 0) return errorCode;
    }

    {
        char lenStr[16];
        sprintf_s(lenStr, sizeof(lenStr), "%.0f", g_bobLength);
        double dir[3] = { 0.0, 1.0, 0.0 };
        errorCode = CreateExtrusion(bobSketch, "0", lenStr, dir, UF_POSITIVE);
        if (errorCode != 0) return errorCode;
    }

    {
        tag_t boreSketch = NULL_TAG;
        tag_t boreCircle = NULL_TAG;

        errorCode = CreateSketchOnPlane("Sketch_Bob_Bore", 0.0, boreSketch);
        if (errorCode != 0) return errorCode;

        errorCode = CreateCircle(0.0, 0.0, Z_ORIGIN, R_PROKLADKA1_INNER, boreCircle);
        if (errorCode != 0) return errorCode;

        errorCode = AddObjectsToSketch(boreSketch, 1, &boreCircle);
        if (errorCode != 0) return errorCode;

        double dir[3] = { 0.0, 0.0, 1.0 };
        errorCode = CreateExtrusion(boreSketch, "0", TUBE_HEIGHT_STR, dir, UF_NEGATIVE);
        if (errorCode != 0) return errorCode;
    }

    return 0;
}


int BuildFlanetz(void)
{
    const double Z_FLANETZ        = -5.0;
    const double Z_FLANETZ_BOTTOM = -30.0;
    const double FL_CENTER_R      = 40.0;

    tag_t flSketch = NULL_TAG;
    tag_t flCurves[8] = { NULL_TAG, NULL_TAG, NULL_TAG, NULL_TAG,
                          NULL_TAG, NULL_TAG, NULL_TAG, NULL_TAG };

    int errorCode = CreateSketchOnPlane("Sketch_Flanetz", Z_FLANETZ, flSketch);
    if (errorCode != 0) return errorCode;

    double topLeft0[3]    = { -PLATE_L,  PLATE_HALF, Z_FLANETZ };
    double topRight0[3]   = {  PLATE_L,  PLATE_HALF, Z_FLANETZ };
    double rightTop0[3]   = {  PLATE_HALF,  PLATE_L, Z_FLANETZ };
    double rightBot0[3]   = {  PLATE_HALF, -PLATE_L, Z_FLANETZ };
    double botRight0[3]   = {  PLATE_L, -PLATE_HALF, Z_FLANETZ };
    double botLeft0[3]    = { -PLATE_L, -PLATE_HALF, Z_FLANETZ };
    double leftBot0[3]    = { -PLATE_HALF, -PLATE_L, Z_FLANETZ };
    double leftTop0[3]    = { -PLATE_HALF,  PLATE_L, Z_FLANETZ };

    double arcTRmid[3]    = {  PLATE_L + PLATE_S,  PLATE_L + PLATE_S, Z_FLANETZ };
    double arcBRmid[3]    = {  PLATE_L + PLATE_S, -PLATE_L - PLATE_S, Z_FLANETZ };
    double arcBLmid[3]    = { -PLATE_L - PLATE_S, -PLATE_L - PLATE_S, Z_FLANETZ };
    double arcTLmid[3]    = { -PLATE_L - PLATE_S,  PLATE_L + PLATE_S, Z_FLANETZ };

    errorCode = CreateLine(topLeft0, topRight0, flCurves[0]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateArc(topRight0, arcTRmid, rightTop0, flCurves[1]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateLine(rightTop0, rightBot0, flCurves[2]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateArc(rightBot0, arcBRmid, botRight0, flCurves[3]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateLine(botRight0, botLeft0, flCurves[4]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateArc(botLeft0, arcBLmid, leftBot0, flCurves[5]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateLine(leftBot0, leftTop0, flCurves[6]);
    if (errorCode != 0) return errorCode;
    errorCode = CreateArc(leftTop0, arcTLmid, topLeft0, flCurves[7]);
    if (errorCode != 0) return errorCode;

    errorCode = AddObjectsToSketch(flSketch, 8, flCurves);
    if (errorCode != 0) return errorCode;

    {
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = CreateExtrusion(flSketch, "0", "25", dir, UF_NULLSIGN);
        if (errorCode != 0) return errorCode;
    }

    {
        const double flHolePos[4][2] = {
            {  CORNER_HOLE_POS,  CORNER_HOLE_POS },
            { -CORNER_HOLE_POS,  CORNER_HOLE_POS },
            { -CORNER_HOLE_POS, -CORNER_HOLE_POS },
            {  CORNER_HOLE_POS, -CORNER_HOLE_POS }
        };
        const char* flHoleNames[4] = {
            "Sketch_Fl_Hole_0", "Sketch_Fl_Hole_1",
            "Sketch_Fl_Hole_2", "Sketch_Fl_Hole_3"
        };
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = BuildHoles(flHolePos, flHoleNames, Z_FLANETZ, CORNER_HOLE_R, "25", dir);
        if (errorCode != 0) return errorCode;
    }

    {
        tag_t flCenterSketch = NULL_TAG;
        tag_t flCenterCircle = NULL_TAG;

        errorCode = CreateSketchOnPlane("Sketch_Fl_Center", Z_FLANETZ_BOTTOM, flCenterSketch);
        if (errorCode != 0) return errorCode;

        errorCode = CreateCircle(0.0, 0.0, Z_FLANETZ_BOTTOM, FL_CENTER_R, flCenterCircle);
        if (errorCode != 0) return errorCode;

        errorCode = AddObjectsToSketch(flCenterSketch, 1, &flCenterCircle);
        if (errorCode != 0) return errorCode;

        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = CreateExtrusion(flCenterSketch, "0", "30", dir, UF_POSITIVE);
        if (errorCode != 0) return errorCode;
    }

    {
        tag_t flThroughSketch = NULL_TAG;
        tag_t flThroughCircle = NULL_TAG;

        errorCode = CreateSketchOnPlane("Sketch_Fl_Through", Z_FLANETZ, flThroughSketch);
        if (errorCode != 0) return errorCode;

        errorCode = CreateCircle(0.0, 0.0, Z_FLANETZ, 25.3, flThroughCircle);
        if (errorCode != 0) return errorCode;

        errorCode = AddObjectsToSketch(flThroughSketch, 1, &flThroughCircle);
        if (errorCode != 0) return errorCode;

        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = CreateExtrusion(flThroughSketch, "0", "55", dir, UF_NEGATIVE);
        if (errorCode != 0) return errorCode;
    }

    return 0;
}

int BuildAssembly(void)
{
    int errorCode;

    errorCode = BuildProkladka1();
    if (errorCode != 0) return errorCode;

    errorCode = BuildZaglushka();
    if (errorCode != 0) return errorCode;

    errorCode = BuildProkladka2();
    if (errorCode != 0) return errorCode;

    errorCode = BuildKorpusFlanetz1();
    if (errorCode != 0) return errorCode;

    errorCode = BuildKorpusPatrubok();
    if (errorCode != 0) return errorCode;

    errorCode = BuildKorpusFlanetz2();
    if (errorCode != 0) return errorCode;

    errorCode = BuildBobyshka();
    if (errorCode != 0) return errorCode;

    errorCode = BuildFlanetz();
    if (errorCode != 0) return errorCode;

    return 0;
}
