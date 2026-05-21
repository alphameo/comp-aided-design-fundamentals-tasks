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
        2,          /* 2 = full circle */
        point1,
        point2,
        point3,
        &circleTag);

    return errorCode;
}
