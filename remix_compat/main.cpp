#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#pragma warning(suppress:5050)
import std.core;
import gta_remix_fixes;

[[maybe_unused]] BOOL WINAPI DllMain(HINSTANCE, DWORD ul_reason_for_call,
                                     LPVOID) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
      GTA_RemixBugFix_Inject();
    return TRUE;
}
