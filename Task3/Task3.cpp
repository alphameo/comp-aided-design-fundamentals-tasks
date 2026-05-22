#include <uf.h>
#include <uf_curve.h>
#include "geometry.h"
#include "sketch.h"
#include "features.h"

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
    const double KP_HOLE_R          = 17.5;
    const double KP_HOLE_Z          = 59.0;

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
    tag_t prokladka2Sketch      = NULL_TAG;
    tag_t prok2Curves[8]        = { NULL_TAG, NULL_TAG, NULL_TAG, NULL_TAG,
                                    NULL_TAG, NULL_TAG, NULL_TAG, NULL_TAG };

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

    {
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = BuildHoles(positions, prokladka1HoleNames, Z_PROKLADKA1_TOP, R_HOLE, "4", dir);
        if (errorCode != 0) goto cleanup;
    }

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

    {
        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = BuildHoles(positions, zaglushkaHoleNames, Z_ZAGLUSHKA_TOP, R_HOLE, "16", dir);
        if (errorCode != 0) goto cleanup;
    }

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

    {
        const double Z_PROKLADKA2  = 0.0;
        const double PROK2_HALF    = 80.0;
        const double PROK2_RADIUS  = 20.0;
        const double PROK2_L       = PROK2_HALF - PROK2_RADIUS;
        const double PROK2_S       = PROK2_RADIUS * 0.7071067811865476;

        errorCode = CreateSketchOnPlane("Sketch_Prokladka2", Z_PROKLADKA2, prokladka2Sketch);
        if (errorCode != 0) goto cleanup;

        double topLeft0[3]    = { -PROK2_L,  PROK2_HALF, Z_PROKLADKA2 };
        double topRight0[3]   = {  PROK2_L,  PROK2_HALF, Z_PROKLADKA2 };
        double rightTop0[3]   = {  PROK2_HALF,  PROK2_L, Z_PROKLADKA2 };
        double rightBot0[3]   = {  PROK2_HALF, -PROK2_L, Z_PROKLADKA2 };
        double botRight0[3]   = {  PROK2_L, -PROK2_HALF, Z_PROKLADKA2 };
        double botLeft0[3]    = { -PROK2_L, -PROK2_HALF, Z_PROKLADKA2 };
        double leftBot0[3]    = { -PROK2_HALF, -PROK2_L, Z_PROKLADKA2 };
        double leftTop0[3]    = { -PROK2_HALF,  PROK2_L, Z_PROKLADKA2 };

        double arcTRmid[3]    = {  PROK2_L + PROK2_S,  PROK2_L + PROK2_S, Z_PROKLADKA2 };
        double arcBRmid[3]    = {  PROK2_L + PROK2_S, -PROK2_L - PROK2_S, Z_PROKLADKA2 };
        double arcBLmid[3]    = { -PROK2_L - PROK2_S, -PROK2_L - PROK2_S, Z_PROKLADKA2 };
        double arcTLmid[3]    = { -PROK2_L - PROK2_S,  PROK2_L + PROK2_S, Z_PROKLADKA2 };

        errorCode = CreateLine(topLeft0, topRight0, prok2Curves[0]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateArc(topRight0, arcTRmid, rightTop0, prok2Curves[1]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateLine(rightTop0, rightBot0, prok2Curves[2]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateArc(rightBot0, arcBRmid, botRight0, prok2Curves[3]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateLine(botRight0, botLeft0, prok2Curves[4]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateArc(botLeft0, arcBLmid, leftBot0, prok2Curves[5]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateLine(leftBot0, leftTop0, prok2Curves[6]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateArc(leftTop0, arcTLmid, topLeft0, prok2Curves[7]);
        if (errorCode != 0) goto cleanup;

        errorCode = AddObjectsToSketch(prokladka2Sketch, 8, prok2Curves);
        if (errorCode != 0) goto cleanup;

        double dir[3] = { 0.0, 0.0, -1.0 };
        errorCode = CreateExtrusion(prokladka2Sketch, "0", "5", dir, UF_NULLSIGN);
        if (errorCode != 0) goto cleanup;

        const double PROK2_HOLE_R    = 7.5;
        const double PROK2_HOLE_POS  = 62.5;

        const double prok2HolePos[4][2] = {
            {  PROK2_HOLE_POS,  PROK2_HOLE_POS },
            { -PROK2_HOLE_POS,  PROK2_HOLE_POS },
            { -PROK2_HOLE_POS, -PROK2_HOLE_POS },
            {  PROK2_HOLE_POS, -PROK2_HOLE_POS }
        };

        const char* prok2HoleNames[4] = {
            "Sketch_Prok2_Hole_0",
            "Sketch_Prok2_Hole_1",
            "Sketch_Prok2_Hole_2",
            "Sketch_Prok2_Hole_3"
        };

        {
            double dir[3] = { 0.0, 0.0, -1.0 };
            errorCode = BuildHoles(prok2HolePos, prok2HoleNames, Z_PROKLADKA2, PROK2_HOLE_R, "5", dir);
            if (errorCode != 0) goto cleanup;
        }

        tag_t prok2CenterSketch = NULL_TAG;
        tag_t prok2CenterCircle = NULL_TAG;

        errorCode = CreateSketchOnPlane("Sketch_Prok2_Center", Z_PROKLADKA2, prok2CenterSketch);
        if (errorCode != 0) goto cleanup;

        const double PROK2_CENTER_R = 40.0;
        errorCode = CreateCircle(0.0, 0.0, Z_PROKLADKA2, PROK2_CENTER_R, prok2CenterCircle);
        if (errorCode != 0) goto cleanup;

        errorCode = AddObjectsToSketch(prok2CenterSketch, 1, &prok2CenterCircle);
        if (errorCode != 0) goto cleanup;

        {
            double dir[3] = { 0.0, 0.0, -1.0 };
            errorCode = CreateExtrusion(prok2CenterSketch, "0", "5", dir, UF_NEGATIVE);
            if (errorCode != 0) goto cleanup;
        }
    }

    {
        const double Z_KORPUS1    = 0.0;
        const double KR1_HALF     = 80.0;
        const double KR1_RADIUS   = 20.0;
        const double KR1_L        = KR1_HALF - KR1_RADIUS;
        const double KR1_S        = KR1_RADIUS * 0.7071067811865476;
        const double KR1_HOLE_R   = 7.5;
        const double KR1_HOLE_POS = 62.5;
        const double KR1_CENTER_R = 25.0;

        tag_t kr1Sketch = NULL_TAG;
        tag_t kr1Curves[8] = { NULL_TAG, NULL_TAG, NULL_TAG, NULL_TAG,
                               NULL_TAG, NULL_TAG, NULL_TAG, NULL_TAG };

        errorCode = CreateSketchOnPlane("Sketch_Korpus1", Z_KORPUS1, kr1Sketch);
        if (errorCode != 0) goto cleanup;

        double topLeft0[3]    = { -KR1_L,  KR1_HALF, Z_KORPUS1 };
        double topRight0[3]   = {  KR1_L,  KR1_HALF, Z_KORPUS1 };
        double rightTop0[3]   = {  KR1_HALF,  KR1_L, Z_KORPUS1 };
        double rightBot0[3]   = {  KR1_HALF, -KR1_L, Z_KORPUS1 };
        double botRight0[3]   = {  KR1_L, -KR1_HALF, Z_KORPUS1 };
        double botLeft0[3]    = { -KR1_L, -KR1_HALF, Z_KORPUS1 };
        double leftBot0[3]    = { -KR1_HALF, -KR1_L, Z_KORPUS1 };
        double leftTop0[3]    = { -KR1_HALF,  KR1_L, Z_KORPUS1 };

        double arcTRmid[3]    = {  KR1_L + KR1_S,  KR1_L + KR1_S, Z_KORPUS1 };
        double arcBRmid[3]    = {  KR1_L + KR1_S, -KR1_L - KR1_S, Z_KORPUS1 };
        double arcBLmid[3]    = { -KR1_L - KR1_S, -KR1_L - KR1_S, Z_KORPUS1 };
        double arcTLmid[3]    = { -KR1_L - KR1_S,  KR1_L + KR1_S, Z_KORPUS1 };

        errorCode = CreateLine(topLeft0, topRight0, kr1Curves[0]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateArc(topRight0, arcTRmid, rightTop0, kr1Curves[1]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateLine(rightTop0, rightBot0, kr1Curves[2]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateArc(rightBot0, arcBRmid, botRight0, kr1Curves[3]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateLine(botRight0, botLeft0, kr1Curves[4]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateArc(botLeft0, arcBLmid, leftBot0, kr1Curves[5]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateLine(leftBot0, leftTop0, kr1Curves[6]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateArc(leftTop0, arcTLmid, topLeft0, kr1Curves[7]);
        if (errorCode != 0) goto cleanup;

        errorCode = AddObjectsToSketch(kr1Sketch, 8, kr1Curves);
        if (errorCode != 0) goto cleanup;

        {
            double dir[3] = { 0.0, 0.0, 1.0 };
            errorCode = CreateExtrusion(kr1Sketch, "0", "14", dir, UF_NULLSIGN);
            if (errorCode != 0) goto cleanup;
        }

        const double kr1HolePos[4][2] = {
            {  KR1_HOLE_POS,  KR1_HOLE_POS },
            { -KR1_HOLE_POS,  KR1_HOLE_POS },
            { -KR1_HOLE_POS, -KR1_HOLE_POS },
            {  KR1_HOLE_POS, -KR1_HOLE_POS }
        };

        const char* kr1HoleNames[4] = {
            "Sketch_Kr1_Hole_0",
            "Sketch_Kr1_Hole_1",
            "Sketch_Kr1_Hole_2",
            "Sketch_Kr1_Hole_3"
        };

        {
            double dir[3] = { 0.0, 0.0, 1.0 };
            errorCode = BuildHoles(kr1HolePos, kr1HoleNames, Z_KORPUS1, KR1_HOLE_R, "14", dir);
            if (errorCode != 0) goto cleanup;
        }

        {
            tag_t kr1CenterSketch = NULL_TAG;
            tag_t kr1CenterCircle = NULL_TAG;

            errorCode = CreateSketchOnPlane("Sketch_Kr1_Center", Z_KORPUS1, kr1CenterSketch);
            if (errorCode != 0) goto cleanup;

            errorCode = CreateCircle(0.0, 0.0, Z_KORPUS1, KR1_CENTER_R, kr1CenterCircle);
            if (errorCode != 0) goto cleanup;

            errorCode = AddObjectsToSketch(kr1CenterSketch, 1, &kr1CenterCircle);
            if (errorCode != 0) goto cleanup;

            double dir[3] = { 0.0, 0.0, 1.0 };
            errorCode = CreateExtrusion(kr1CenterSketch, "0", "14", dir, UF_NEGATIVE);
            if (errorCode != 0) goto cleanup;
        }
    }

    {
        const double Z_KORPUS_PATRUBOK = 0.0;
        const double KP_OUTER_R = 30.0;
        const double KP_INNER_R = 25.0;

        tag_t kpSketch = NULL_TAG;
        tag_t kpOuter  = NULL_TAG;
        tag_t kpInner  = NULL_TAG;

        errorCode = CreateSketchOnPlane("Sketch_Korpus_Patrubok", Z_KORPUS_PATRUBOK, kpSketch);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateCircle(0.0, 0.0, Z_KORPUS_PATRUBOK, KP_OUTER_R, kpOuter);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateCircle(0.0, 0.0, Z_KORPUS_PATRUBOK, KP_INNER_R, kpInner);
        if (errorCode != 0) goto cleanup;

        {
            tag_t objs[2] = { kpOuter, kpInner };
            errorCode = AddObjectsToSketch(kpSketch, 2, objs);
            if (errorCode != 0) goto cleanup;
        }

        {
            double dir[3] = { 0.0, 0.0, 1.0 };
            errorCode = CreateExtrusion(kpSketch, "0", "105", dir, UF_POSITIVE);
            if (errorCode != 0) goto cleanup;
        }

        {
            tag_t kpHoleSketch = NULL_TAG;

            errorCode = CreateSketchOnPlaneZX("Sketch_KP_Hole", kpHoleSketch);
            if (errorCode != 0) goto cleanup;

            tag_t kpHoleCircle = NULL_TAG;

            double pt1[3] = { KP_HOLE_R, 0.0, KP_HOLE_Z };
            double pt2[3] = { -KP_HOLE_R, 0.0, KP_HOLE_Z };
            double pt3[3] = { 0.0, 0.0, KP_HOLE_Z + KP_HOLE_R };

            errorCode = UF_CURVE_create_arc_thru_3pts(2, pt1, pt2, pt3, &kpHoleCircle);
            if (errorCode != 0) goto cleanup;

            errorCode = AddObjectsToSketch(kpHoleSketch, 1, &kpHoleCircle);
            if (errorCode != 0) goto cleanup;

            {
                double dir[3] = { 0.0, 1.0, 0.0 };
                errorCode = CreateExtrusion(kpHoleSketch, "0", "60", dir, UF_NEGATIVE);
                if (errorCode != 0) goto cleanup;
            }
        }
    }

    {
        const double Z_KORPUS2    = 119.0;
        const double KR2_OUTER_R  = 70.0;
        const double KR2_INNER_R  = 25.0;

        tag_t kr2Sketch = NULL_TAG;
        tag_t kr2Outer  = NULL_TAG;
        tag_t kr2Inner  = NULL_TAG;

        errorCode = CreateSketchOnPlane("Sketch_Korpus2", Z_KORPUS2, kr2Sketch);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateCircle(0.0, 0.0, Z_KORPUS2, KR2_OUTER_R, kr2Outer);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateCircle(0.0, 0.0, Z_KORPUS2, KR2_INNER_R, kr2Inner);
        if (errorCode != 0) goto cleanup;

        {
            tag_t objs[2] = { kr2Outer, kr2Inner };
            errorCode = AddObjectsToSketch(kr2Sketch, 2, objs);
            if (errorCode != 0) goto cleanup;
        }

        {
            double dir[3] = { 0.0, 0.0, -1.0 };
            errorCode = CreateExtrusion(kr2Sketch, "0", "25", dir, UF_POSITIVE);
            if (errorCode != 0) goto cleanup;
        }

        {
            const double KR2_HOLE_R    = 6.5;
            const double KR2_HOLE_POS  = 50.0;

            const double kr2HolePos[4][2] = {
                {  KR2_HOLE_POS,  0.0 },
                {  0.0,  KR2_HOLE_POS },
                { -KR2_HOLE_POS,  0.0 },
                {  0.0, -KR2_HOLE_POS }
            };

            const char* kr2HoleNames[4] = {
                "Sketch_Kr2_Hole_0",
                "Sketch_Kr2_Hole_1",
                "Sketch_Kr2_Hole_2",
                "Sketch_Kr2_Hole_3"
            };

            double dir[3] = { 0.0, 0.0, -1.0 };
            errorCode = BuildHoles(kr2HolePos, kr2HoleNames, Z_KORPUS2, KR2_HOLE_R, "25", dir);
            if (errorCode != 0) goto cleanup;
        }
    }

    {
        const double BOB_Y_PLANE   = 30.0;
        const double BOB_Z         = KP_HOLE_Z;
        const double BOB_OUTER_R   = KP_HOLE_R;
        const double BOB_INNER_R   = 10.5;

        tag_t bobSketch = NULL_TAG;

        errorCode = CreateSketchOnPlaneParallelZX("Sketch_Bobyshka", BOB_Y_PLANE, bobSketch);
        if (errorCode != 0) goto cleanup;

        tag_t bobOuter = NULL_TAG;
        double pt1[3] = { BOB_OUTER_R, BOB_Y_PLANE, BOB_Z };
        double pt2[3] = { -BOB_OUTER_R, BOB_Y_PLANE, BOB_Z };
        double pt3[3] = { 0.0, BOB_Y_PLANE, BOB_Z + BOB_OUTER_R };
        errorCode = UF_CURVE_create_arc_thru_3pts(2, pt1, pt2, pt3, &bobOuter);
        if (errorCode != 0) goto cleanup;

        tag_t bobInner = NULL_TAG;
        pt1[0] = BOB_INNER_R; pt1[2] = BOB_Z;
        pt2[0] = -BOB_INNER_R; pt2[2] = BOB_Z;
        pt3[2] = BOB_Z + BOB_INNER_R;
        errorCode = UF_CURVE_create_arc_thru_3pts(2, pt1, pt2, pt3, &bobInner);
        if (errorCode != 0) goto cleanup;

        {
            tag_t objs[2] = { bobOuter, bobInner };
            errorCode = AddObjectsToSketch(bobSketch, 2, objs);
            if (errorCode != 0) goto cleanup;
        }

        {
            double dir[3] = { 0.0, -1.0, 0.0 };
            errorCode = CreateExtrusion(bobSketch, "0", "30", dir, UF_POSITIVE);
            if (errorCode != 0) goto cleanup;
        }

        {
            double dir[3] = { 0.0, 1.0, 0.0 };
            errorCode = CreateExtrusion(bobSketch, "0", "15", dir, UF_POSITIVE);
            if (errorCode != 0) goto cleanup;
        }
    }

    {
        const double Z_FLANETZ    = -5.0;
        const double FL_HALF      = 80.0;
        const double FL_RADIUS    = 20.0;
        const double FL_L         = FL_HALF - FL_RADIUS;
        const double FL_S         = FL_RADIUS * 0.7071067811865476;
        const double FL_HOLE_R    = 7.5;
        const double FL_HOLE_POS  = 62.5;

        tag_t flSketch = NULL_TAG;
        tag_t flCurves[8] = { NULL_TAG, NULL_TAG, NULL_TAG, NULL_TAG,
                              NULL_TAG, NULL_TAG, NULL_TAG, NULL_TAG };

        errorCode = CreateSketchOnPlane("Sketch_Flanetz", Z_FLANETZ, flSketch);
        if (errorCode != 0) goto cleanup;

        double topLeft0[3]    = { -FL_L,  FL_HALF, Z_FLANETZ };
        double topRight0[3]   = {  FL_L,  FL_HALF, Z_FLANETZ };
        double rightTop0[3]   = {  FL_HALF,  FL_L, Z_FLANETZ };
        double rightBot0[3]   = {  FL_HALF, -FL_L, Z_FLANETZ };
        double botRight0[3]   = {  FL_L, -FL_HALF, Z_FLANETZ };
        double botLeft0[3]    = { -FL_L, -FL_HALF, Z_FLANETZ };
        double leftBot0[3]    = { -FL_HALF, -FL_L, Z_FLANETZ };
        double leftTop0[3]    = { -FL_HALF,  FL_L, Z_FLANETZ };

        double arcTRmid[3]    = {  FL_L + FL_S,  FL_L + FL_S, Z_FLANETZ };
        double arcBRmid[3]    = {  FL_L + FL_S, -FL_L - FL_S, Z_FLANETZ };
        double arcBLmid[3]    = { -FL_L - FL_S, -FL_L - FL_S, Z_FLANETZ };
        double arcTLmid[3]    = { -FL_L - FL_S,  FL_L + FL_S, Z_FLANETZ };

        errorCode = CreateLine(topLeft0, topRight0, flCurves[0]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateArc(topRight0, arcTRmid, rightTop0, flCurves[1]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateLine(rightTop0, rightBot0, flCurves[2]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateArc(rightBot0, arcBRmid, botRight0, flCurves[3]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateLine(botRight0, botLeft0, flCurves[4]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateArc(botLeft0, arcBLmid, leftBot0, flCurves[5]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateLine(leftBot0, leftTop0, flCurves[6]);
        if (errorCode != 0) goto cleanup;

        errorCode = CreateArc(leftTop0, arcTLmid, topLeft0, flCurves[7]);
        if (errorCode != 0) goto cleanup;

        errorCode = AddObjectsToSketch(flSketch, 8, flCurves);
        if (errorCode != 0) goto cleanup;

        {
            double dir[3] = { 0.0, 0.0, -1.0 };
            errorCode = CreateExtrusion(flSketch, "0", "25", dir, UF_NULLSIGN);
            if (errorCode != 0) goto cleanup;
        }

        const double flHolePos[4][2] = {
            {  FL_HOLE_POS,  FL_HOLE_POS },
            { -FL_HOLE_POS,  FL_HOLE_POS },
            { -FL_HOLE_POS, -FL_HOLE_POS },
            {  FL_HOLE_POS, -FL_HOLE_POS }
        };

        const char* flHoleNames[4] = {
            "Sketch_Fl_Hole_0",
            "Sketch_Fl_Hole_1",
            "Sketch_Fl_Hole_2",
            "Sketch_Fl_Hole_3"
        };

        {
            double dir[3] = { 0.0, 0.0, -1.0 };
            errorCode = BuildHoles(flHolePos, flHoleNames, Z_FLANETZ, FL_HOLE_R, "25", dir);
            if (errorCode != 0) goto cleanup;
        }

        {
            const double Z_FLANETZ_BOTTOM = -30.0;
            const double FL_CENTER_R      = 40.0;

            tag_t flCenterSketch = NULL_TAG;
            tag_t flCenterCircle = NULL_TAG;

            errorCode = CreateSketchOnPlane("Sketch_Fl_Center", Z_FLANETZ_BOTTOM, flCenterSketch);
            if (errorCode != 0) goto cleanup;

            errorCode = CreateCircle(0.0, 0.0, Z_FLANETZ_BOTTOM, FL_CENTER_R, flCenterCircle);
            if (errorCode != 0) goto cleanup;

            errorCode = AddObjectsToSketch(flCenterSketch, 1, &flCenterCircle);
            if (errorCode != 0) goto cleanup;

            double dir[3] = { 0.0, 0.0, -1.0 };
            errorCode = CreateExtrusion(flCenterSketch, "0", "30", dir, UF_POSITIVE);
            if (errorCode != 0) goto cleanup;
        }

        {
            tag_t flThroughSketch = NULL_TAG;
            tag_t flThroughCircle = NULL_TAG;

            errorCode = CreateSketchOnPlane("Sketch_Fl_Through", Z_FLANETZ, flThroughSketch);
            if (errorCode != 0) goto cleanup;

            errorCode = CreateCircle(0.0, 0.0, Z_FLANETZ, 25.3, flThroughCircle);
            if (errorCode != 0) goto cleanup;

            errorCode = AddObjectsToSketch(flThroughSketch, 1, &flThroughCircle);
            if (errorCode != 0) goto cleanup;

            double dir[3] = { 0.0, 0.0, -1.0 };
            errorCode = CreateExtrusion(flThroughSketch, "0", "55", dir, UF_NEGATIVE);
            if (errorCode != 0) goto cleanup;
        }
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
