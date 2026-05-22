#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <objbase.h>
#include <shobjidl.h>
#undef CreateDialog
#undef byte

#include "UI_Perekhodnik.hpp"
#include "../build.h"

#include <uf_part.h>
#include <string>

using namespace NXOpen;
using namespace NXOpen::BlockStyler;

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
            static char pngPath[MAX_PATH];
            GetModuleFileNameA(hMod, dllPath, MAX_PATH);

            // DLL: Task3\x64\Release\Task3.dll
            // .dlx: Task3\UI_NX\UI_Perekhodnik.dlx
            {
                char tmp[MAX_PATH];
                strcpy(tmp, dllPath);
                char *p = strrchr(tmp, '\\');
                if (p) *p = '\0';
                p = strrchr(tmp, '\\');
                if (p) *p = '\0';  // now at x64
                p = strrchr(tmp, '\\');
                if (p) *p = '\0';  // now at Task3 root
                strcat(tmp, "\\UI_NX\\UI_Perekhodnik.dlx");
                strcpy(dlxPath, tmp);
                theDlxFileName = dlxPath;
            }

            // preview: Task3\UI_NX\preview.bmp
            {
                char tmp[MAX_PATH];
                strcpy(tmp, dllPath);
                char *p = strrchr(tmp, '\\');
                if (p) *p = '\0';
                p = strrchr(tmp, '\\');
                if (p) *p = '\0';
                p = strrchr(tmp, '\\');
                if (p) *p = '\0';
                strcat(tmp, "\\UI_NX\\preview.bmp");
                strcpy(pngPath, tmp);
                m_previewPath = pngPath;
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
        bitmap0 = dynamic_cast<NXOpen::BlockStyler::Button*>(theDialog->TopBlock()->FindBlock("bitmap0"));
        button1 = dynamic_cast<NXOpen::BlockStyler::Button*>(theDialog->TopBlock()->FindBlock("button1"));
        string0 = dynamic_cast<NXOpen::BlockStyler::StringBlock*>(theDialog->TopBlock()->FindBlock("string0"));
        bobSelector = dynamic_cast<NXOpen::BlockStyler::Enumeration*>(theDialog->TopBlock()->FindBlock("bobSelector"));
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
        if (string0 != NULL)
            string0->SetValue(m_savePath);

        if (bitmap0 != NULL && !m_previewPath.empty())
        {
            PropertyList* props = bitmap0->GetProperties();
            props->SetString("Bitmap", m_previewPath.c_str());
            delete props;
        }

        if (bobSelector != NULL)
        {
            PropertyList* props = bobSelector->GetProperties();
            props->SetEnum("Value", 1);
            delete props;
        }
    }
    catch(exception& ex)
    {
        UI_Perekhodnik::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
}

static int BrowseForSaveFile(std::string& outPath, const std::string& defPath)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    BOOL needUninit = (hr == S_OK);
    if (FAILED(hr))
        return 1;

    IFileSaveDialog* pfd = NULL;
    hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
                          IID_PPV_ARGS(&pfd));

    int result = 1;
    if (SUCCEEDED(hr))
    {
        COMDLG_FILTERSPEC filter[] = {
            { L"NX Part Files", L"*.prt" },
            { L"All Files",     L"*.*"   }
        };
        pfd->SetFileTypes(2, filter);
        pfd->SetDefaultExtension(L"prt");

        {
            std::wstring wdef;
            wdef.assign(defPath.begin(), defPath.end());
            pfd->SetFileName(wdef.c_str());
        }

        hr = pfd->Show(NULL);
        if (SUCCEEDED(hr))
        {
            IShellItem* psi = NULL;
            hr = pfd->GetResult(&psi);
            if (SUCCEEDED(hr))
            {
                PWSTR path = NULL;
                hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &path);
                if (SUCCEEDED(hr))
                {
                    char folder[MAX_PATH];
                    WideCharToMultiByte(CP_ACP, 0, path, -1,
                                        folder, MAX_PATH, NULL, NULL);
                    outPath = folder;
                    result = 0;
                    CoTaskMemFree(path);
                }
                psi->Release();
            }
        }
        pfd->Release();
    }

    if (needUninit)
        CoUninitialize();
    return result;
}

int UI_Perekhodnik::update_cb(NXOpen::BlockStyler::UIBlock* block)
{
    try
    {
        if (block == string0)
        {
            if (string0->Value().GetLocaleText())
                m_savePath = string0->Value().GetLocaleText();
        }
        else if (block == button1)
        {
            std::string path;
            if (BrowseForSaveFile(path, m_savePath) == 0)
            {
                m_savePath = path;
                string0->SetValue(m_savePath);
            }
        }
        else if (block == bobSelector)
        {
            PropertyList* props = bobSelector->GetProperties();
            int sel = props->GetEnum("Value");
            SetBobLength(sel);
            delete props;
        }
    }
    catch(exception& ex)
    {
        UI_Perekhodnik::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
    return 0;
}

static void CreateDirRecursive(const char* path)
{
    char tmp[MAX_PATH];
    strcpy(tmp, path);
    for (char* p = tmp; *p; p++)
        if (*p == '/') *p = '\\';
    for (char* p = tmp + 1; *p; p++)
    {
        if (*p == '\\')
        {
            char c = *p;
            *p = '\0';
            CreateDirectoryA(tmp, NULL);
            *p = c;
        }
    }
    CreateDirectoryA(tmp, NULL);
}

static int CreatePartAndBuild(const std::string& fullPath)
{
    std::string dir = fullPath;
    size_t pos = dir.find_last_of("\\/");
    if (pos != std::string::npos)
    {
        dir.resize(pos);
        CreateDirRecursive(dir.c_str());
    }

    tag_t partTag = NULL_TAG;

    // if the part is already loaded, close it first
    partTag = UF_PART_ask_part_tag(fullPath.c_str());
    if (partTag != NULL_TAG)
        UF_PART_close(partTag, 1, 1);

    partTag = NULL_TAG;
    DeleteFileA(fullPath.c_str());

    int errorCode = UF_PART_new(fullPath.c_str(), 1, &partTag);
    if (errorCode != 0)
        return errorCode;

    errorCode = BuildAssembly();
    if (errorCode != 0)
        return errorCode;

    return UF_PART_save();
}

int UI_Perekhodnik::apply_cb()
{
    try
    {
        if (string0->Value().GetLocaleText())
            m_savePath = string0->Value().GetLocaleText();

        if (bobSelector != NULL)
        {
            PropertyList* props = bobSelector->GetProperties();
            int sel = props->GetEnum("Value");
            SetBobLength(sel);
            delete props;
        }

        if (m_savePath.empty())
        {
            UI_Perekhodnik::theUI->NXMessageBox()->Show("Error",
                NXOpen::NXMessageBox::DialogTypeError, "Save path is empty.");
            return 1;
        }

        int result = CreatePartAndBuild(m_savePath);
        if (result != 0)
        {
            char msg[256];
            sprintf(msg, "Failed to create part and build.\n%s\nError: %d",
                    m_savePath.c_str(), result);
            UI_Perekhodnik::theUI->NXMessageBox()->Show("Error",
                NXOpen::NXMessageBox::DialogTypeError, msg);
            return 1;
        }

        return 0;
    }
    catch(exception& ex)
    {
        UI_Perekhodnik::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
        return 1;
    }
}

int UI_Perekhodnik::ok_cb()
{
    return apply_cb();
}

PropertyList* UI_Perekhodnik::GetBlockProperties(const char *blockID)
{
    return theDialog->GetBlockProperties(blockID);
}
