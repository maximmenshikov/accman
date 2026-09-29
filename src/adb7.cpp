/* FullUnlock v4.0 project.
   Account Manager implementation.
   
   (C) ultrashot 2012
*/
#include "stdafx.h"
#include "adb7.h"

extern "C"
{
    HRESULT CeGetProcessAccount(HANDLE hProcess, PACCTID accountId,
                                DWORD cbSize);
}

/**
 * Return the owner account of a process.
 *
 * @param hProcess    Handle of the process to query.
 *
 * @return The owner account id.
 */
ACCTID
GetAccount(HANDLE hProcess)
{
    ACCTID account;
    CeGetProcessAccount(hProcess, &account, sizeof(ACCTID));
    return account;
}

/**
 * Resolve an account id to its name.
 *
 * @param accountID             Account id to resolve.
 * @param lpwszAccountName      Buffer that receives the account name.
 * @param dwAccountNameLength   Buffer size, in characters.
 *
 * @return TRUE on success, FALSE on failure.
 */
BOOL
GetAccountName(ACCTID accountID, LPWSTR lpwszAccountName,
               DWORD dwAccountNameLength)
{
    DWORD strSize = dwAccountNameLength;
    ACCTID account = accountID;
    if (ADBNameFromAccountID(&account, lpwszAccountName, &strSize) ==
        ERROR_SUCCESS)
        return TRUE;
    return FALSE;
}

/**
 * Resolve an account id to its normalized name.
 *
 * @param accountID             Account id to resolve.
 * @param lpwszAccountName      Buffer that receives the normalized name.
 * @param dwAccountNameLength   Buffer size, in characters.
 *
 * @return FALSE unconditionally in this implementation (the normalized name is
 *         still written to lpwszAccountName on success).
 */
BOOL
GetNormalizedAccountName(ACCTID accountID, LPWSTR lpwszAccountName,
                         DWORD dwAccountNameLength)
{
    ACCTID account = accountID;

    wchar_t name[500];
    DWORD nameLength = 500;
    if (GetAccountName(account, name, nameLength) == TRUE)
    {
        DWORD strSize = dwAccountNameLength;
        ADBNormalizeAccountName(name, lpwszAccountName, &strSize);
    }
    return FALSE;
}

/**
 * Resolve an account name to its id.
 *
 * @param lpwszAccountName    Account name to resolve.
 *
 * @return The account id, or 0 if the name was not found.
 */
ACCTID
Name2AccountID(LPWSTR lpwszAccountName)
{
    ACCTID account = 0;
    ADBAccountIDFromName(lpwszAccountName, &account);
    return account;
}
