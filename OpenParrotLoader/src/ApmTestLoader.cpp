#include <windows.h>
#include <tlhelp32.h>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include "ApmTestLoader.h"

#if defined(_M_AMD64)
namespace
{
struct Handle
{
    HANDLE value = nullptr;
    ~Handle() { if (value && value != INVALID_HANDLE_VALUE) CloseHandle(value); }
};

bool ReadRemote(HANDLE process, uintptr_t address, void* buffer, size_t size)
{
    SIZE_T read = 0;
    return ReadProcessMemory(process, reinterpret_cast<void*>(address), buffer, size, &read) && read == size;
}

bool WriteRemote(HANDLE process, uintptr_t address, const void* buffer, size_t size)
{
    SIZE_T written = 0;
    return WriteProcessMemory(process, reinterpret_cast<void*>(address), buffer, size, &written) && written == size;
}

bool WriteCode(HANDLE process, uintptr_t address, const void* buffer, size_t size)
{
    DWORD previous = 0;
    if (!VirtualProtectEx(process, reinterpret_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, &previous))
        return false;
    const bool written = WriteRemote(process, address, buffer, size);
    DWORD unused = 0;
    const bool restored = VirtualProtectEx(process, reinterpret_cast<void*>(address), size, previous, &unused) != FALSE;
    return written && restored && FlushInstructionCache(process, reinterpret_cast<void*>(address), size);
}

uintptr_t FindModule(DWORD pid, const std::wstring& name)
{
    Handle snapshot{CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid)};
    MODULEENTRY32W module = {};
    module.dwSize = sizeof(module);
    if (!Module32FirstW(snapshot.value, &module)) return 0;
    do
    {
        if (_wcsicmp(module.szModule, name.c_str()) == 0)
            return reinterpret_cast<uintptr_t>(module.modBaseAddr);
    } while (Module32NextW(snapshot.value, &module));
    return 0;
}

bool ReadRemoteString(HANDLE process, uintptr_t address, std::string& result)
{
    result.clear();
    for (size_t i = 0; i < 512; ++i)
    {
        char ch = 0;
        if (!ReadRemote(process, address + i, &ch, 1)) return false;
        if (!ch) return true;
        result.push_back(ch);
    }
    return false;
}

// Read exports in the target, including forwarded exports. In particular, do
// not assume a protected DLL has the same export RVAs before and after loading.
uintptr_t FindExport(HANDLE process, DWORD pid, uintptr_t base, const char* name, unsigned depth = 0)
{
    if (!base || depth > 8) return 0;
    IMAGE_DOS_HEADER dos = {};
    IMAGE_NT_HEADERS64 nt = {};
    if (!ReadRemote(process, base, &dos, sizeof(dos)) || dos.e_magic != IMAGE_DOS_SIGNATURE ||
        dos.e_lfanew < 0 || !ReadRemote(process, base + dos.e_lfanew, &nt, sizeof(nt)) ||
        nt.Signature != IMAGE_NT_SIGNATURE || nt.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
        return 0;
    const auto& directory = nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    if (!directory.VirtualAddress || directory.Size < sizeof(IMAGE_EXPORT_DIRECTORY)) return 0;
    IMAGE_EXPORT_DIRECTORY exports = {};
    if (!ReadRemote(process, base + directory.VirtualAddress, &exports, sizeof(exports)) ||
        exports.NumberOfNames > 65536 || exports.NumberOfFunctions > 65536) return 0;
    DWORD index = MAXDWORD;
    if (name[0] == '#')
    {
        const unsigned long ordinal = strtoul(name + 1, nullptr, 10);
        if (ordinal >= exports.Base) index = ordinal - exports.Base;
    }
    else
    {
        std::vector<DWORD> names(exports.NumberOfNames);
        std::vector<WORD> ordinals(exports.NumberOfNames);
        if (!ReadRemote(process, base + exports.AddressOfNames, names.data(), names.size() * sizeof(DWORD)) ||
            !ReadRemote(process, base + exports.AddressOfNameOrdinals, ordinals.data(), ordinals.size() * sizeof(WORD)))
            return 0;
        for (size_t i = 0; i < names.size(); ++i)
        {
            std::string candidate;
            if (!ReadRemoteString(process, base + names[i], candidate)) return 0;
            if (candidate == name) { index = ordinals[i]; break; }
        }
    }
    if (index >= exports.NumberOfFunctions) return 0;
    DWORD rva = 0;
    if (!ReadRemote(process, base + exports.AddressOfFunctions + index * sizeof(DWORD), &rva, sizeof(rva)) || !rva)
        return 0;
    if (rva >= directory.VirtualAddress && static_cast<uint64_t>(rva) <
        static_cast<uint64_t>(directory.VirtualAddress) + directory.Size)
    {
        std::string forwarder;
        if (!ReadRemoteString(process, base + rva, forwarder)) return 0;
        const size_t dot = forwarder.find_last_of('.');
        if (dot == std::string::npos) return 0;
        std::wstring module(forwarder.begin(), forwarder.begin() + dot);
        module += L".dll";
        return FindExport(process, pid, FindModule(pid, module), forwarder.c_str() + dot + 1, depth + 1);
    }
    return base + rva;
}

bool ValidateImage(const std::filesystem::path& path, bool managed)
{
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    const auto size = input.tellg();
    if (!input || size < 256 || size > 128 * 1024 * 1024) return false;
    std::vector<BYTE> bytes(static_cast<size_t>(size));
    input.seekg(0);
    if (!input.read(reinterpret_cast<char*>(bytes.data()), size)) return false;
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(bytes.data());
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < 0 ||
        static_cast<size_t>(dos->e_lfanew) + sizeof(IMAGE_NT_HEADERS64) > bytes.size()) return false;
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(bytes.data() + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return false;
    if (!managed) return nt->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64 &&
        (nt->FileHeader.Characteristics & IMAGE_FILE_DLL) && nt->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC;
    IMAGE_DATA_DIRECTORY clr = {};
    if (nt->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
        const auto* nt32 = reinterpret_cast<const IMAGE_NT_HEADERS32*>(nt);
        if (nt32->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 ||
            nt32->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR) return false;
        clr = nt32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR];
    }
    else if (nt->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC &&
        nt->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64 &&
        nt->OptionalHeader.NumberOfRvaAndSizes > IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR)
        clr = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR];
    else return false;
    if (!clr.VirtualAddress || clr.Size < 24) return false;
    const size_t sectionOffset = dos->e_lfanew + 24 + nt->FileHeader.SizeOfOptionalHeader;
    if (sectionOffset + nt->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER) > bytes.size()) return false;
    const auto* sections = reinterpret_cast<const IMAGE_SECTION_HEADER*>(bytes.data() + sectionOffset);
    for (size_t i = 0; i < nt->FileHeader.NumberOfSections; ++i)
    {
        const auto& section = sections[i];
        if (clr.VirtualAddress < section.VirtualAddress) continue;
        const uint64_t delta = static_cast<uint64_t>(clr.VirtualAddress) - section.VirtualAddress;
        const uint64_t offset = section.PointerToRawData + delta;
        if (delta + 24 > section.SizeOfRawData || offset + 24 > bytes.size()) continue;
        DWORD flags = 0;
        memcpy(&flags, bytes.data() + offset + 16, sizeof(flags));
        // ILONLY, without 32BITREQUIRED, 32BITPREFERRED or NATIVE_ENTRYPOINT.
        return (flags & 1) && !(flags & (2 | 0x20000 | 0x10));
    }
    return false;
}

std::vector<wchar_t> ChildEnvironment()
{
    LPWCH source = GetEnvironmentStringsW();
    std::vector<std::wstring> entries;
    if (!source) return {};
    for (const wchar_t* entry = source; *entry; entry += wcslen(entry) + 1)
    {
        const std::wstring value(entry);
        bool skip = false;
        for (const wchar_t* key : {L"TP_DIRECTHOOK=", L"TP_REMOTETHREAD=", L"TP_POSTSTART_REMOTETHREAD_MS=",
            L"TP_ENTRYPOINT_REMOTETHREAD_MS=", L"TP_LOADER_MANAGED_INIT=", L"TP_APM_TEST_INIT="})
            if (_wcsnicmp(entry, key, wcslen(key)) == 0) skip = true;
        if (!skip) entries.push_back(value);
    }
    FreeEnvironmentStringsW(source);
    entries.push_back(L"TP_APM_TEST_INIT=1");
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
        return _wcsicmp(a.c_str(), b.c_str()) < 0;
    });
    std::vector<wchar_t> block;
    for (const auto& entry : entries)
    {
        block.insert(block.end(), entry.begin(), entry.end());
        block.push_back(0);
    }
    block.push_back(0);
    return block;
}

bool HoldAtClrEntry(PROCESS_INFORMATION& pi)
{
    uintptr_t entry = 0;
    BYTE original = 0;
    bool initialBreakpoint = false;
    const ULONGLONG deadline = GetTickCount64() + 30000;
    while (GetTickCount64() < deadline)
    {
        DEBUG_EVENT event = {};
        if (!WaitForDebugEvent(&event, 250))
        {
            if (GetLastError() == ERROR_SEM_TIMEOUT) continue;
            return false;
        }
        DWORD disposition = DBG_CONTINUE;
        bool failed = false;
        if (event.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT)
        {
            if (event.u.CreateProcessInfo.hFile) CloseHandle(event.u.CreateProcessInfo.hFile);
            if (event.u.CreateProcessInfo.hProcess != pi.hProcess) CloseHandle(event.u.CreateProcessInfo.hProcess);
            if (event.u.CreateProcessInfo.hThread != pi.hThread) CloseHandle(event.u.CreateProcessInfo.hThread);
        }
        else if (event.dwDebugEventCode == CREATE_THREAD_DEBUG_EVENT)
            CloseHandle(event.u.CreateThread.hThread);
        else if (event.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT)
        {
            Handle file{event.u.LoadDll.hFile};
            wchar_t path[32768] = {};
            if (file.value && GetFinalPathNameByHandleW(file.value, path, static_cast<DWORD>(std::size(path)), 0))
            {
                const wchar_t* filename = wcsrchr(path, L'\\');
                if (filename && _wcsicmp(filename + 1, L"mscoree.dll") == 0)
                {
                    entry = FindExport(pi.hProcess, pi.dwProcessId,
                        reinterpret_cast<uintptr_t>(event.u.LoadDll.lpBaseOfDll), "_CorExeMain");
                    const BYTE breakpoint = 0xcc;
                    failed = !entry || !ReadRemote(pi.hProcess, entry, &original, 1) ||
                        !WriteCode(pi.hProcess, entry, &breakpoint, 1);
                    wprintf(L"APM: armed CLR entry at %p\n", reinterpret_cast<void*>(entry));
                    fflush(stdout);
                }
            }
        }
        else if (event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
        {
            const auto& exception = event.u.Exception.ExceptionRecord;
            if (exception.ExceptionCode == EXCEPTION_BREAKPOINT && entry &&
                reinterpret_cast<uintptr_t>(exception.ExceptionAddress) == entry && event.dwThreadId == pi.dwThreadId)
            {
                CONTEXT context = {};
                context.ContextFlags = CONTEXT_CONTROL;
                const char* stage = "GetThreadContext";
                bool ready = GetThreadContext(pi.hThread, &context) != FALSE;
                if (ready) { stage = "restore CLR entry"; ready = WriteCode(pi.hProcess, entry, &original, 1); }
                context.Rip = entry;
                if (ready) { stage = "SetThreadContext"; ready = SetThreadContext(pi.hThread, &context) != FALSE; }
                if (ready) { stage = "SuspendThread"; ready = SuspendThread(pi.hThread) != MAXDWORD; }
                if (ready) { stage = "ContinueDebugEvent"; ready = ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE) != FALSE; }
                if (ready) { stage = "DebugActiveProcessStop"; ready = DebugActiveProcessStop(pi.dwProcessId) != FALSE; }
                if (ready)
                {
                    wprintf(L"APM: primary thread held at _CorExeMain; startup debugger detached\n");
                    fflush(stdout);
                    return true;
                }
                wprintf(L"APM: %hs failed at CLR entry (Win32 %lu)\n", stage, GetLastError());
                return false;
            }
            else if (exception.ExceptionCode == EXCEPTION_BREAKPOINT && !initialBreakpoint)
                initialBreakpoint = true;
            else
            {
                disposition = DBG_EXCEPTION_NOT_HANDLED;
                if (!event.u.Exception.dwFirstChance)
                {
                    wprintf(L"APM: startup exception 0x%08lX at %p\n", exception.ExceptionCode, exception.ExceptionAddress);
                    failed = true;
                }
            }
        }
        else if (event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
        {
            wprintf(L"APM: target exited before CLR entry (0x%08lX)\n", event.u.ExitProcess.dwExitCode);
            failed = true;
        }
        if (!ContinueDebugEvent(event.dwProcessId, event.dwThreadId, disposition) || failed) return false;
    }
    wprintf(L"APM: timed out waiting for _CorExeMain\n");
    return false;
}

bool CallRemote(PROCESS_INFORMATION& pi, uintptr_t procedure, void* parameter, DWORD timeout, DWORD& result)
{
    Handle thread{CreateRemoteThread(pi.hProcess, nullptr, 0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(procedure), parameter, 0, nullptr)};
    if (!thread.value) return false;
    HANDLE handles[] = {thread.value, pi.hProcess};
    const DWORD wait = WaitForMultipleObjects(2, handles, FALSE, timeout);
    if (wait != WAIT_OBJECT_0 || WaitForSingleObject(pi.hProcess, 0) == WAIT_OBJECT_0)
    {
        DWORD exitCode = STILL_ACTIVE;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        wprintf(L"APM: remote call did not complete (wait=0x%08lX, process=0x%08lX)\n", wait, exitCode);
        return false;
    }
    return GetExitCodeThread(thread.value, &result) != FALSE;
}

bool InitializeCore(PROCESS_INFORMATION& pi, const std::wstring& core, const char* gameId)
{
    const uintptr_t kernel32 = FindModule(pi.dwProcessId, L"kernel32.dll");
    struct LoadData
    {
        uintptr_t loadLibrary;
        uintptr_t getLastError;
        uintptr_t path;
        uintptr_t module;
        DWORD error;
        DWORD reserved;
        char gameId[8];
    } data = {};
    strcpy_s(data.gameId, gameId);
    data.loadLibrary = FindExport(pi.hProcess, pi.dwProcessId, kernel32, "LoadLibraryW");
    data.getLastError = FindExport(pi.hProcess, pi.dwProcessId, kernel32, "GetLastError");
    if (!data.loadLibrary || !data.getLastError) return false;

    // DWORD WINAPI Load(LoadData*): preserve RBX, reserve x64 shadow space,
    // record the full-width HMODULE and the error from the loading thread.
    const BYTE stub[] = {
        0x53, 0x48, 0x83, 0xec, 0x20, 0x48, 0x8b, 0xd9,
        0x48, 0x8b, 0x4b, 0x10, 0xff, 0x13, 0x48, 0x89, 0x43, 0x18,
        0x48, 0x85, 0xc0, 0x75, 0x06, 0xff, 0x53, 0x08, 0x89, 0x43, 0x20,
        0x33, 0xc0, 0x48, 0x83, 0xc4, 0x20, 0x5b, 0xc3
    };
    const size_t pathSize = (core.size() + 1) * sizeof(wchar_t);
    const uintptr_t allocation = reinterpret_cast<uintptr_t>(VirtualAllocEx(pi.hProcess, nullptr,
        4096 + sizeof(data) + pathSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    if (!allocation) return false;
    data.path = allocation + 4096 + sizeof(data);
    const uintptr_t remoteData = allocation + 4096;
    if (!WriteRemote(pi.hProcess, allocation, stub, sizeof(stub)) ||
        !WriteRemote(pi.hProcess, remoteData, &data, sizeof(data)) ||
        !WriteRemote(pi.hProcess, data.path, core.c_str(), pathSize)) return false;
    DWORD previous = 0;
    if (!VirtualProtectEx(pi.hProcess, reinterpret_cast<void*>(allocation), 4096, PAGE_EXECUTE_READ, &previous) ||
        !FlushInstructionCache(pi.hProcess, reinterpret_cast<void*>(allocation), sizeof(stub))) return false;
    wprintf(L"APM: loading core with normal LoadLibraryW\n");
    fflush(stdout);
    DWORD result = 0;
    if (!CallRemote(pi, allocation, reinterpret_cast<void*>(remoteData), 60000, result) ||
        !ReadRemote(pi.hProcess, remoteData, &data, sizeof(data))) return false;
    if (!data.module)
    {
        wprintf(L"APM: LoadLibraryW failed (Win32 %lu / 0x%08lX)\n", data.error, data.error);
        return false;
    }
    wprintf(L"APM: DLL loaded at %p\n", reinterpret_cast<void*>(data.module));
    const uintptr_t initialize = FindExport(pi.hProcess, pi.dwProcessId, data.module, "TP_InitializeApmTest");
    if (!initialize)
    {
        wprintf(L"APM: core does not export TP_InitializeApmTest; use an updated TP DLL\n");
        return false;
    }
    wprintf(L"APM: initializing hooks; menu is still held\n");
    fflush(stdout);
    if (!CallRemote(pi, initialize, reinterpret_cast<void*>(remoteData + offsetof(LoadData, gameId)), 120000, result)) return false;
    VirtualFreeEx(pi.hProcess, reinterpret_cast<void*>(allocation), 0, MEM_RELEASE);
    if (result != ERROR_SUCCESS)
    {
        wprintf(L"APM: initialization failed (Win32 %lu / 0x%08lX); see tp_log.txt\n", result, result);
        return false;
    }
    wprintf(L"APM: hooks initialized\n");
    return true;
}
}
#endif

int RunApmTestLoader(int argc, wchar_t* argv[])
{
#if !defined(_M_AMD64)
    wprintf(L"APM: this mode requires OpenParrotLoader64 and an x64/AnyCPU menu\n");
    return ERROR_NOT_SUPPORTED;
#else
    if (argc < 5 || argc > 6)
    {
        wprintf(L"Usage: OpenParrotLoader64 --apm-test <GAME-ID> <DLL-or-basename> <EXE> [arguments]\n");
        return ERROR_BAD_ARGUMENTS;
    }
    PROCESS_INFORMATION pi = {};
    Handle process;
    Handle thread;
    try
    {
        char gameId[5] = {};
        if (wcslen(argv[2]) != 4) return ERROR_INVALID_PARAMETER;
        for (size_t i = 0; i < 4; ++i)
        {
            wchar_t ch = argv[2][i];
            if (ch >= L'a' && ch <= L'z') ch -= L'a' - L'A';
            if (!(ch >= L'A' && ch <= L'Z') && !(ch >= L'0' && ch <= L'9'))
                return ERROR_INVALID_PARAMETER;
            gameId[i] = static_cast<char>(ch);
        }
        std::filesystem::path core = std::filesystem::absolute(argv[3]);
        if (_wcsicmp(core.extension().c_str(), L".dll") != 0) core += L".dll";
        const auto game = std::filesystem::absolute(argv[4]);
        if (!ValidateImage(core, false) || !ValidateImage(game, true))
        {
            wprintf(L"APM: expected an x64 native core DLL and an IL-only x64/AnyCPU executable\n");
            return ERROR_BAD_EXE_FORMAT;
        }
        std::wstring command = L"\"" + game.wstring() + L"\"";
        if (argc == 6) command += L" " + std::wstring(argv[5]);
        auto environment = ChildEnvironment();
        if (environment.empty()) return ERROR_NOT_ENOUGH_MEMORY;
        STARTUPINFOW startup = {};
        startup.cb = sizeof(startup);
        startup.dwFlags = STARTF_USESHOWWINDOW;
        startup.wShowWindow = SW_SHOWNORMAL;
        wprintf(L"APM native startup\nCore: %ls\nMenu: %ls\n", core.c_str(), game.c_str());
        if (!CreateProcessW(game.c_str(), command.data(), nullptr, nullptr, FALSE,
            DEBUG_ONLY_THIS_PROCESS | CREATE_UNICODE_ENVIRONMENT, environment.data(),
            game.parent_path().c_str(), &startup, &pi))
        {
            const DWORD error = GetLastError();
            wprintf(L"APM: CreateProcess failed (Win32 %lu)\n", error);
            return error;
        }
        process.value = pi.hProcess;
        thread.value = pi.hThread;
        if (!HoldAtClrEntry(pi) || !InitializeCore(pi, core.wstring(), gameId))
        {
            wprintf(L"APM: startup failed; menu will not be released (last Win32 %lu)\n", GetLastError());
            TerminateProcess(pi.hProcess, ERROR_DLL_INIT_FAILED);
            DebugActiveProcessStop(pi.dwProcessId);
            return ERROR_DLL_INIT_FAILED;
        }
        wprintf(L"APM: releasing menu (PID %lu)\n", pi.dwProcessId);
        fflush(stdout);
        if (ResumeThread(pi.hThread) == MAXDWORD)
        {
            TerminateProcess(pi.hProcess, ERROR_DLL_INIT_FAILED);
            return ERROR_DLL_INIT_FAILED;
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD exitCode = 0;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        wprintf(L"APM: menu exited (0x%08lX)\n", exitCode);
        return static_cast<int>(exitCode);
    }
    catch (const std::exception& ex)
    {
        wprintf(L"APM: %hs\n", ex.what());
        if (pi.hProcess) TerminateProcess(pi.hProcess, ERROR_DLL_INIT_FAILED);
        DebugActiveProcessStop(pi.dwProcessId);
        return ERROR_DLL_INIT_FAILED;
    }
#endif
}
