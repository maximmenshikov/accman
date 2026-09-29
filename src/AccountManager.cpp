/* FullUnlock v4.0 project.
   Account Manager implementation.
   
   (C) ultrashot 2012
*/
#include "stdafx.h"
#include "adb7.h"

#define FULL_TRUST_GROUP L"S-1-5-112-0-0XFD"

#define THIRD_PARTY_GROUP 0x55555554
#define TRUSTED_GROUP (THIRD_PARTY_GROUP | 0x1)

/**
 * Read the ADB privileges property of an account.
 *
 * @param account    SID string of the account.
 *
 * @return The privileges property value (0 if unavailable).
 */
DWORD
GetAccountSpecialProperty(LPWSTR account)
{
    DWORD dwAccountPropLength = sizeof(DWORD);
    DWORD dwProp = 0;
    ADBGetAccountProperty(account, ADBPROP_PRIVILEGES, &dwAccountPropLength,
                          &dwProp);
    return dwProp;
}

/**
 * Test whether a privileges value denotes the third-party or trusted group.
 *
 * @param prop    Privileges property value.
 *
 * @return TRUE if the value is the third-party or trusted group.
 */
inline BOOL
_IsThirdParty(DWORD prop)
{
    if (prop == THIRD_PARTY_GROUP || prop == TRUSTED_GROUP)
        return TRUE;
    return FALSE;
}

/**
 * Test whether a privileges value denotes the trusted group.
 *
 * @param prop    Privileges property value.
 *
 * @return TRUE if the value is the trusted group.
 */
inline BOOL
_IsTrusted(DWORD prop)
{
    if (prop == TRUSTED_GROUP)
        return TRUE;
    return FALSE;
}

/**
 * Report whether an account is privileged: either not third-party at all, or
 * third-party but marked trusted.
 *
 * @param account    SID string of the account.
 *
 * @return TRUE if the account is privileged.
 */
BOOL
GetPrivileged(LPWSTR account)
{
    DWORD prop = GetAccountSpecialProperty(account);

    if (_IsThirdParty(prop) == FALSE)
        return TRUE;

    return _IsTrusted(prop);
}

/**
 * Report whether full-trust mode is enabled, i.e. the full-trust group account
 * exists in the account database.
 *
 * @return TRUE if full-trust mode is enabled.
 */
BOOL
IsFullTrustModeEnabled()
{
    DWORD cbSize = sizeof(DWORD);
    DWORD dwValue = 0xDEADC0DE;
    if (ADBGetAccountProperty(FULL_TRUST_GROUP, ADBPROP_ACCTID, &cbSize,
                              &dwValue) == S_OK)
    {
        return TRUE;
    }
    return FALSE;
}

/**
 * Enable or disable full-trust mode by creating or deleting the full-trust
 * group account.
 *
 * @param mode    TRUE to enable full-trust mode, FALSE to disable it.
 */
VOID
SetFullTrustEnabled(BOOL mode)
{
    if (mode)
    {
        ADBCreateAccount(FULL_TRUST_GROUP, ACCT_FLAG_GROUP, 0);
    }
    else
    {
        ADBDeleteAccount(FULL_TRUST_GROUP);
    }
}

/**
 * Mark an account as trusted by setting its ADB privileges property.
 *
 * @param account    SID string of the account.
 */
VOID
AddToPrivilegedGroup(LPWSTR account)
{
    if (account)
    {
        DWORD data = TRUSTED_GROUP;
        ADB_BLOB adbBlob;
        adbBlob.propertyId = ADBPROP_PRIVILEGES;
        adbBlob.pData = &data;
        adbBlob.cbData = sizeof(DWORD);
        ADBSetAccountProperties(account, 1, &adbBlob);
    }
}

/**
 * Demote a trusted account back to the plain third-party group.
 *
 * @param account    SID string of the account.
 */
VOID
RemoveFromPrivilegedGroup(LPWSTR account)
{
    if (account)
    {
        DWORD prop = GetAccountSpecialProperty(account);
        if (_IsTrusted(prop) == TRUE)
        {
            DWORD data = THIRD_PARTY_GROUP;
            ADB_BLOB adbBlob;
            adbBlob.propertyId = ADBPROP_PRIVILEGES;
            adbBlob.pData = &data;
            adbBlob.cbData = sizeof(DWORD);
            ADBSetAccountProperties(account, 1, &adbBlob);
        }
    }
}

/**
 * Move a non-third-party account into the third-party group.
 *
 * @param account    SID string of the account.
 */
VOID
AddToThirdPartyGroup(LPWSTR account)
{
    if (account)
    {
        DWORD prop = GetAccountSpecialProperty(account);
        if (_IsThirdParty(prop) == FALSE)
        {
            DWORD data = THIRD_PARTY_GROUP;
            ADB_BLOB adbBlob;
            adbBlob.propertyId = ADBPROP_PRIVILEGES;
            adbBlob.pData = &data;
            adbBlob.cbData = sizeof(DWORD);
            ADBSetAccountProperties(account, 1, &adbBlob);
        }
    }
}

/**
 * Clear the third-party privileges of an account.
 *
 * @param account    SID string of the account.
 */
VOID
RemoveFromThirdPartyGroup(LPWSTR account)
{
    if (account)
    {
        DWORD prop = GetAccountSpecialProperty(account);
        if (_IsThirdParty(prop) == TRUE)
        {
            DWORD data = 0;
            ADB_BLOB adbBlob;
            adbBlob.propertyId = ADBPROP_PRIVILEGES;
            adbBlob.pData = &data;
            adbBlob.cbData = sizeof(DWORD);
            ADBSetAccountProperties(account, 1, &adbBlob);
        }
    }
}

/**
 * Report whether an account is in the third-party group. One hard-coded
 * account-manager SID is always treated as not third-party.
 *
 * @param account    SID string of the account.
 *
 * @return TRUE if the account is in the third-party group.
 */
BOOL
IsInThirdPartyGroup(LPWSTR account)
{
    if (wcscmp(
            account,
            L"S-1-5-112-0-0X80-0X7B37393445423641452D423246392D344246422D393237372D3431363145344439453346357D") ==
        0)
        return FALSE;

    DWORD prop = GetAccountSpecialProperty(account);
    return _IsThirdParty(prop);
}
