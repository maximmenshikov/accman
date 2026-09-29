/* FullUnlock v4.0 project.
   Account Manager implementation.
   
   (C) ultrashot 2012
*/
#include "stdafx.h"
#include "adb7.h"
#include "AccountManager.h"

#include "common/ProcessFunctions.h"

/**
 * Entry point. When five arguments are supplied and the patched account
 * database is active (a sentinel property probe returns 0xDEADC0DE), applies
 * the privilege grants listed under HKLM\Software\OEM\Accman: the value
 * FULL_TRUST enables full-trust mode; any other value names an account (its
 * name hex-encoded) to add to the privileged group.
 *
 * @param argc    Argument count; the routine is a no-op unless it is 5.
 * @param argv    Argument vector (unused beyond the count check).
 *
 * @return 0 always.
 */
int
_tmain(int argc, _TCHAR *argv[])
{
    if (argc != 5)
        return 0;
    ACCTID account = 0;
    int res =
        CeGetProcessAccount(GetModuleHandle(NULL), &account, sizeof(DWORD));

    wchar_t name[200];
    GetAccountName(account, name, 200);

    DWORD dwData = -1;
    DWORD cbData = 4;
    ADBGetAccountProperty(L"S-1-5-112-0-0-12345", ADBPROP_PRIVILEGES, &cbData,
                          &dwData);

    if (dwData == 0xDEADC0DE)
    {
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\OEM\\Accman", 0,
                         KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            wchar_t keyName[50];
            DWORD keyNameSize = 50;

            DWORD dwIndex = 0;

            DWORD dwType = REG_NONE;
            while (RegEnumValue(hKey, dwIndex++, keyName, &keyNameSize, NULL,
                                &dwType, NULL, NULL) == ERROR_SUCCESS)
            {
                if (dwType != REG_NONE)
                {
                    if (wcsicmp(keyName, L"FULL_TRUST") == 0)
                    {
                        SetFullTrustEnabled(TRUE);
                    }
                    else
                    {
                        wchar_t val[500];
                        wchar_t accountName[100] = {0};
                        for (int x = 0; x < wcslen(keyName); x++)
                        {
                            wchar_t t[4];
                            swprintf(t, L"%02X", towupper(keyName[x]));
                            wcscat(accountName, t);
                        }
                        swprintf(val, L"S-1-5-112-0-0X80-0X%ls", accountName);
                        AddToPrivilegedGroup(val);
                    }
                }
                keyNameSize = 50;
            }
            RegCloseKey(hKey);
        }
    }
    return 0;
}
