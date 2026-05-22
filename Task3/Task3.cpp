#define _CRT_SECURE_NO_WARNINGS

#include <uf.h>
#include <uf_ui.h>
#include <stdio.h>
#include <exception>

#include "UI_NX/UI_Perekhodnik.hpp"

void ufusr(char* param, int* retcode, int paramLen)
{
    *retcode = 0;

    int errorCode = UF_initialize();
    if (errorCode != 0)
    {
        *retcode = errorCode;
        return;
    }

    try
    {
        UI_Perekhodnik dialog;
        dialog.Show();
    }
    catch (std::exception& ex)
    {
        char msg[1024];
        sprintf(msg, "Dialog error.\n\n%s\n\n"
                     "Make sure UI_Perekhodnik.dlx is next to Task3.dll\n"
                     "or in $UGII_USER_DIR/application/",
                ex.what());
        uc1601(msg, 1);
        *retcode = 1;
    }

    UF_terminate();
}

int ufusr_ask_unload(void)
{
    return UF_UNLOAD_IMMEDIATELY;
}
