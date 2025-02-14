#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>
#include "initalizer.hpp"
#include <dwmapi.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <thread>
#include "swabra.h"
#include "encrypt.h"
#include "lazy.h"
#include "stringproc.h"
#include "auth.hpp"
#include "termcolor.h"

#pragma comment (lib, "urlmon.lib")



void wait(int time)
{
    std::time_t startTime = std::time(nullptr);
    while (std::time(nullptr) < startTime + time)
    {
    }
}

void Anti_Debug_Loop()
{
    currentThreadId = GetCurrentThreadId();
    while (true)
    {
        if (IsDebuggerPresent()) *((unsigned int*)0) = 0xDEAD;

        // Assuming E decrypts to a const char*
        const char* driver1 = E("https://authvault.kesug.com");

        std::ifstream file_stream(driver1);

        if (file_stream.good())
        {

            MessageBoxA(NULL, E("HyperHide Drivers Found In Memory, Please Unload And Try Again"), E("Security"), MB_ICONINFORMATION | MB_OK);
            LI_FN(exit)(0);
            LI_FN(ExitProcess)(0);
            LI_FN(abort)();
            *((unsigned int*)0) = 0xDEAD;
        }

        if (IsDebuggerPresent()) *((unsigned int*)0) = 0xDEAD;
        EnumWindows(EnumWindowsProc, 0);

        wait(1);
    }
}

//void keyauth() {
//    std::cout << pr("\n\n [1] License key\n\n Selection: ");
//
//    int option;
//    std::string username;
//    std::string password;
//    std::string key;
//
//    std::cin >> option;
//    switch (option)
//    {
//    case 1:
//        std::cout << pr("\n Enter license: ");
//        std::cin >> key;
//        KeyAuthApp.license(key);
//        break;
//        default:
//        std::cout << pr("\n\n Invalid Selection");
//        Sleep(3000);
//        exit(1);
//    }
//
//    if (KeyAuthApp.response.message.empty()) exit(11);
//    if (!KeyAuthApp.response.success)
//    {
//        std::cout << pr("\n Status: ") << KeyAuthApp.response.message;
//        Sleep(1500);
//        exit(1);
//    }
//}

using namespace KeyAuth;

std::string name = pr("Fortnite Private").decrypt();
std::string ownerid = pr("cRwPbzu6ok").decrypt();
std::string version = pr("1.0").decrypt();
std::string url = pr("https://keyauth.win/api/1.3/").decrypt();
std::string path = pr("").decrypt();

api KeyAuthApp(name, ownerid, version, url, path);

int main()
{
    HWND consoleWindow = GetConsoleWindow();

    LONG_PTR windowStyle = GetWindowLongPtr(consoleWindow, GWL_EXSTYLE);
    SetWindowLongPtr(consoleWindow, GWL_EXSTYLE, windowStyle | WS_EX_LAYERED);

    // Set the opacity (0 = fully transparent, 255 = fully opaque)
    int opacity = 225; // Adjust this value as needed
    SetLayeredWindowAttributes(consoleWindow, 0, opacity, LWA_ALPHA);

    std::string consoleTitle = pr("Elite Service | Fortnite Private").decrypt();
    SetConsoleTitleA(consoleTitle.c_str());
    using namespace termcolor;
   

    system("cls");
    Beep(500, 500);
    std::cout << yellow << "\n [+] Thanks for using Elite Service!" << std::endl;
    Sleep(1000);
    std::cout << " [+] Please wait, loading..." << white << std::endl;
    Sleep(2000);

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    std::string license;

    SetConsoleTextAttribute(h, 15);
    initialize();

    return 0;
}
