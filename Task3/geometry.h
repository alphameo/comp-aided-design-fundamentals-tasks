#pragma once
#include <uf.h>

int CreateCircle(
    double       cx,
    double       cy,
    double       z,
    double       radius,
    tag_t&       circleTag
);

int CreateLine(
    const double start[3],
    const double end[3],
    tag_t&       lineTag
);

int CreateArc(
    double p1[3],
    double p2[3],
    double p3[3],
    tag_t&       arcTag
);
