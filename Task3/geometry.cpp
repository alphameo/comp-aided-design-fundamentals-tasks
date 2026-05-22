#include "geometry.h"
#include <uf_curve.h>

int CreateCircle(
    double  cx,
    double  cy,
    double  z,
    double  radius,
    tag_t&  circleTag)
{
    double point1[3] = { cx + radius, cy,         z };
    double point2[3] = { cx - radius, cy,         z };
    double point3[3] = { cx,          cy + radius, z };

    int errorCode = UF_CURVE_create_arc_thru_3pts(
        2,
        point1,
        point2,
        point3,
        &circleTag);

    return errorCode;
}

int CreateLine(
    const double start[3],
    const double end[3],
    tag_t&       lineTag)
{
    UF_CURVE_line_t lineData;
    lineData.start_point[0] = start[0];
    lineData.start_point[1] = start[1];
    lineData.start_point[2] = start[2];
    lineData.end_point[0] = end[0];
    lineData.end_point[1] = end[1];
    lineData.end_point[2] = end[2];

    return UF_CURVE_create_line(&lineData, &lineTag);
}

int CreateArc(
    double p1[3],
    double p2[3],
    double p3[3],
    tag_t&       arcTag)
{
    return UF_CURVE_create_arc_thru_3pts(
        1,
        p1, p2, p3,
        &arcTag);
}
