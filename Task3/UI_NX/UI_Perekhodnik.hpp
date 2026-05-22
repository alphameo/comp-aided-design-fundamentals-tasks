#ifndef UI_PEREKHODNIK_H_INCLUDED
#define UI_PEREKHODNIK_H_INCLUDED

#include <uf_defs.h>
#include <uf_ui_types.h>
#include <iostream>
#include <string>
#include <NXOpen/Session.hxx>
#include <NXOpen/UI.hxx>
#include <NXOpen/NXMessageBox.hxx>
#include <NXOpen/Callback.hxx>
#include <NXOpen/NXException.hxx>
#include <NXOpen/BlockStyler_UIBlock.hxx>
#include <NXOpen/BlockStyler_BlockDialog.hxx>
#include <NXOpen/BlockStyler_PropertyList.hxx>
#include <NXOpen/BlockStyler_Label.hxx>
#include <NXOpen/BlockStyler_Button.hxx>
#include <NXOpen/BlockStyler_Button.hxx>
#include <NXOpen/BlockStyler_StringBlock.hxx>
#include <NXOpen/BlockStyler_Enumeration.hxx>

using namespace std;
using namespace NXOpen;
using namespace NXOpen::BlockStyler;

class UI_Perekhodnik
{
public:
    static Session *theSession;
    static UI *theUI;
    UI_Perekhodnik();
    ~UI_Perekhodnik();
    int Show();

    void initialize_cb();
    void dialogShown_cb();
    int apply_cb();
    int ok_cb();
    int update_cb(NXOpen::BlockStyler::UIBlock* block);
    PropertyList* GetBlockProperties(const char *blockID);

private:
    const char* theDlxFileName;
    NXOpen::BlockStyler::BlockDialog* theDialog;
    NXOpen::BlockStyler::Button* bitmap0;
    NXOpen::BlockStyler::Button* button1;
    NXOpen::BlockStyler::StringBlock* string0;
    NXOpen::BlockStyler::Enumeration* bobSelector;
    std::string m_savePath = "D:\\Temp\\NX\\Alekseev_P_9_Perekhodnik.prt";
    std::string m_previewPath;
};

#endif
