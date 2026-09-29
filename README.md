# accman — Account Manager

`accman.exe` is a console tool for Windows CE and Windows Phone 7. It comes from
the FullUnlock v4.0 project (© Maxim Menshikov (ultrashot), 2012). It elevates account privileges
to full trust. It uses the account-database (ADB) API to move accounts into
privileged groups and to enable full-trust mode.

This is legacy research and homebrew code for a platform that reached end of
life long ago.

## How it works

On start (`accman.cpp`), the tool checks that the patched account database is
active. A sentinel `ADBGetAccountProperty` probe returns the magic value
`0xDEADC0DE`. The tool then reads the grants under this key:

```
HKEY_LOCAL_MACHINE\Software\OEM\Accman
```

The tool applies each value under that key through `AccountManager.cpp`:

- A value named `FULL_TRUST` enables full-trust mode. `SetFullTrustEnabled`
  creates the full-trust group account.
- Any other value names an account. The tool hex-encodes the account name into a
  SID and adds it to the privileged group (`AddToPrivilegedGroup`).

`AccountManager.cpp` wraps the privilege model: third-party, trusted, and full
trust. `adb7.cpp` provides thin thunks over the ADB API (`GetAccount`,
`GetAccountName`, `Name2AccountID`). Process-account lookup comes from
`CeGetProcessAccount` in `common/ProcessFunctions.h`.

## Layout

```
accman/
├── accman.vcproj       Visual Studio 2008 project (WM6 Pro ARMv4I)
├── .clang-format       OKTET Labs C style rules for src/
├── src/                Project sources
│   ├── accman.cpp          Entry point; reads HKLM\Software\OEM\Accman
│   ├── AccountManager.cpp/.h   Privilege / full-trust model
│   ├── adb7.cpp            ADB (account database) thunks
│   ├── stdafx.h/.cpp       Precompiled-header stub
│   ├── resource.h, accman.rc
├── sdk/                Vendored SDK headers + import library
│   ├── adb7.h              ADB API
│   └── coredll7.lib
└── common/             Git submodule → github.com/maximmenshikov/common
                        (provides ProcessFunctions.h: CeGetProcessAccount)
```

## Building

You need Visual Studio 2008 with the Windows Mobile 6 Professional SDK (ARMV4I)
installed. The shared `common` headers are a git submodule. To build the tool:

1. Clone the repository.
2. Run `git submodule update --init`.
3. Open `accman.vcproj`.
4. Build the project.

The output is `accman.exe`. The include and library paths point at `src/`,
`sdk/`, and the repository root (for `common/`). The project is otherwise
self-contained.
