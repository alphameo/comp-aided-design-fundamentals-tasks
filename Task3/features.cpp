#include "features.h"
#include <string.h>

int CreateExtrusion(
    tag_t            sketchTag,
    const char*      startVal,
    const char*      endVal,
    const double     direction[3],
    UF_FEATURE_SIGN  sign)
{
    int errorCode = 0;

    uf_list_p_t objectList = nullptr;
    errorCode = UF_MODL_create_list(&objectList);
    if (errorCode != 0)
        return errorCode;

    errorCode = UF_MODL_put_list_item(objectList, sketchTag);
    if (errorCode != 0)
    {
        UF_MODL_delete_list(&objectList);
        return errorCode;
    }

    char startStr[15];
    char endStr[15];
    strcpy_s(startStr, sizeof(startStr), startVal);
    strcpy_s(endStr,   sizeof(endStr),   endVal);

    char*   limits[2] = { startStr, endStr };
    char    taper[5]  = "0.0";
    double  point[3]  = { 0.0, 0.0, 0.0 };
    double  dir[3]    = { direction[0], direction[1], direction[2] };

    uf_list_p_t featList = nullptr;

    errorCode = UF_MODL_create_extruded(
        objectList,
        taper,
        limits,
        point,
        dir,
        sign,
        &featList);

    if (featList != nullptr)
    {
        UF_MODL_delete_list(&featList);
    }
    UF_MODL_delete_list(&objectList);

    return errorCode;
}
