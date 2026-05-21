#include "sketch.h"
#include <uf_sket.h>
#include <string.h>

int CreateSketchOnPlane(
    const char*  name,
    double       zOffset,
    tag_t&       sketchTag)
{
    int errorCode = 0;

    // UF_SKET_initialize_sketch expects a writable char array
    char sketchName[129];
    strcpy_s(sketchName, sizeof(sketchName), name);

    errorCode = UF_SKET_initialize_sketch(sketchName, &sketchTag);
    if (errorCode != 0)
        return errorCode;

    double matrix[9] =
    {
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, zOffset
    };

    int   option     = 2;
    tag_t obj[2]     = { NULL_TAG, NULL_TAG };
    int   ref[2]     = { 0, 0 };
    int   planeDir   = 1;

    errorCode = UF_SKET_create_sketch(
        sketchName,
        option,
        matrix,
        obj,
        ref,
        planeDir,
        &sketchTag);

    return errorCode;
}

int CreateSketchOnPlaneZX(
    const char*  name,
    tag_t&       sketchTag)
{
    int errorCode = 0;

    char sketchName[129];
    strcpy_s(sketchName, sizeof(sketchName), name);

    errorCode = UF_SKET_initialize_sketch(sketchName, &sketchTag);
    if (errorCode != 0)
        return errorCode;

    // ZX-plane (Y=0): sketch X = world X, sketch Y = world Z
    double matrix[9] =
    {
        1.0, 0.0, 0.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 0.0
    };

    int   option     = 2;
    tag_t obj[2]     = { NULL_TAG, NULL_TAG };
    int   ref[2]     = { 0, 0 };
    int   planeDir   = 1;

    errorCode = UF_SKET_create_sketch(
        sketchName,
        option,
        matrix,
        obj,
        ref,
        planeDir,
        &sketchTag);

    return errorCode;
}

int AddObjectsToSketch(
    tag_t  sketchTag,
    int    count,
    tag_t  objects[])
{
    return UF_SKET_add_objects(sketchTag, count, objects);
}
