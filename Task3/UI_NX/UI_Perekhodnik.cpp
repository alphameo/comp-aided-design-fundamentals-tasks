// windows.h must come before NXOpen headers — its macros (CreateDialog,
// byte) would otherwise clash with NXOpen::UI::CreateDialog() etc.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef CreateDialog
#undef byte

#include "UI_Perekhodnik.hpp"
#include "../build.h"

using namespace NXOpen;
using namespace NXOpen::BlockStyler;

// Marker whose address is guaranteed to be inside Task3.dll,
// used by GetModuleHandleEx to resolve the DLL's own path.
namespace {
    static void __dllMarker() {}
}

Session *(UI_Perekhodnik::theSession) = NULL;
UI *(UI_Perekhodnik::theUI) = NULL;

UI_Perekhodnik::UI_Perekhodnik()
{
    try
    {
        UI_Perekhodnik::theSession = NXOpen::Session::GetSession();
        UI_Perekhodnik::theUI = UI::GetUI();

        HMODULE hMod = NULL;
        if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                               (LPCTSTR)__dllMarker, &hMod))
        {
            theDlxFileName = "UI_Perekhodnik.dlx";
        }
        else
        {
            static char dllPath[MAX_PATH];
            static char dlxPath[MAX_PATH];
            GetModuleFileNameA(hMod, dllPath, MAX_PATH);
            char *lastSlash = strrchr(dllPath, '\\');
            if (lastSlash)
            {
                *(lastSlash + 1) = '\0';
                strcpy(dlxPath, dllPath);
                strcat(dlxPath, "UI_Perekhodnik.dlx");
                theDlxFileName = dlxPath;
            }
            else
            {
                theDlxFileName = "UI_Perekhodnik.dlx";
            }
        }

        theDialog = UI_Perekhodnik::theUI->CreateDialog(theDlxFileName);
        theDialog->AddApplyHandler(make_callback(this, &UI_Perekhodnik::apply_cb));
        theDialog->AddOkHandler(make_callback(this, &UI_Perekhodnik::ok_cb));
        theDialog->AddUpdateHandler(make_callback(this, &UI_Perekhodnik::update_cb));
        theDialog->AddInitializeHandler(make_callback(this, &UI_Perekhodnik::initialize_cb));
        theDialog->AddDialogShownHandler(make_callback(this, &UI_Perekhodnik::dialogShown_cb));
    }
    catch(exception& ex)
    {
        throw;
    }
}

UI_Perekhodnik::~UI_Perekhodnik()
{
    if (theDialog != NULL)
    {
        delete theDialog;
        theDialog = NULL;
    }
}

int UI_Perekhodnik::Show()
{
    try
    {
        theDialog->Show();
    }
    catch(exception& ex)
    {
        UI_Perekhodnik::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
    return 0;
}

void UI_Perekhodnik::initialize_cb()
{
    try
    {
        label0 = dynamic_cast<NXOpen::BlockStyler::Label*>(theDialog->TopBlock()->FindBlock("label0"));
        button0 = dynamic_cast<NXOpen::BlockStyler::Button*>(theDialog->TopBlock()->FindBlock("button0"));
    }
    catch(exception& ex)
    {
        UI_Perekhodnik::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
}

void UI_Perekhodnik::dialogShown_cb()
{
    try
    {
    }
    catch(exception& ex)
    {
        UI_Perekhodnik::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
}

int UI_Perekhodnik::apply_cb()
{
    int errorCode = 0;
    try
    {
        BuildAssembly();
    }
    catch(exception& ex)
    {
        errorCode = 1;
        UI_Perekhodnik::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
    return errorCode;
}

int UI_Perekhodnik::update_cb(NXOpen::BlockStyler::UIBlock* block)
{
    try
    {
        if(block == label0)
        {
        }
        else if(block == button0)
        {
            BuildAssembly();
        }
    }
    catch(exception& ex)
    {
        UI_Perekhodnik::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
    return 0;
}

int UI_Perekhodnik::ok_cb()
{
    int errorCode = 0;
    try
    {
        errorCode = apply_cb();
    }
    catch(exception& ex)
    {
        errorCode = 1;
        UI_Perekhodnik::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
    return errorCode;
}

PropertyList* UI_Perekhodnik::GetBlockProperties(const char *blockID)
{
    return theDialog->GetBlockProperties(blockID);
}
