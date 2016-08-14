#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include "curses_provider.h"

std::unique_ptr<CursesProvider> curses;
std::string TMPDIR;
// XXX: ~ should do the job.
std::string HOME_PATH = getenv("HOME");

void atExitFunction() {
    system(std::string("find " + std::string(HOME_PATH) +
                       "/.config/feednix -type f -not -name \'config.json\' "
                       "-and -not -name \'log.txt\' -delete 2> /dev/null")
               .c_str());
    system(std::string("rm -R " + TMPDIR + " 2> /dev/null").c_str());
}

void sighandler(int signum) {
    system(std::string("find " + std::string(HOME_PATH) +
                       "/.config/feednix -type f -not -name \'config.json\' "
                       "-and -not -name \'log.txt\' -delete 2> /dev/null")
               .c_str());
    signal(signum, SIG_DFL);
    kill(getpid(), signum);
    std::cerr << "Aborted" << std::endl;
}

void printUsage(char **argv) {
    std::cout << "Usage: " << argv[0] << " [OPTIONS]" << std::endl;
    std::cout << "\tAn ncurses-based console client for Feedly written in C++"
                << std::endl;
    std::cout << std::endl << "Options:" << std:: endl
                << "\t-h\t\tDisplay this help and exit" << std::endl
                << "\t-v\t\tSet curl to output in verbose mode during login" 
                << std::endl
                << "\t-c\t\tChange tokens with prompt" << std::endl;
    std::cout << std::endl << "Config:" << std::endl
                << "\tFeednix uses a config file to set colors" << std::endl
                << "\tand the amount of posts to be retrived per request." 
                << std::endl;
    std::cout << std::endl << "This file can be found and must be placed in:" 
                << std::endl 
                << "\t$HOME/.config/feednix" << std::endl
                << "\tA log file can also be found here." << std::endl 
                << "\tA sample config can be found in /etc/feednix" 
                << std::endl;
    std::cout << std::endl << "Bugs:" << std::endl 
                << "\tPlease report any bugs on Github "
                "<https://github.com/Jarkore/Feednix>" << std::endl;
}

int main(int argc, char **argv) {
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGSEGV, sighandler);
    atexit(atExitFunction);

    // Insure that config file and directory are present if not copy default
    // into user home
    std::string configPath = HOME_PATH + "/.config/feednix/config.json";
    // XXX: WTF fopen ??
    if (fopen(configPath.c_str(), "r") == nullptr) {
        // XXX: WTF system ? Use C++ code.
        system(("mkdir -p " + HOME_PATH + "/.config/feednix &> /dev/null")
                   .c_str());
        system(("cp /etc/xdg/feednix/config.json " + HOME_PATH +
                "/.config/feednix")
                   .c_str());
        system(("chmod 600 " + HOME_PATH + "/.config/feednix/config.json")
                   .c_str());
    }

    char *sys_tmpdir = getenv("TMPDIR");
    if (not sys_tmpdir) {
        sys_tmpdir = (char *)"/tmp";
    }

    char *pathTemp = (char *)malloc(sizeof(char) * (strlen(sys_tmpdir) + 16));
    strcpy(pathTemp, sys_tmpdir);
    strcat(pathTemp, "/feednix.XXXXXX");

    TMPDIR = std::string(mkdtemp(pathTemp));
    free(pathTemp);

    bool verboseEnabled = false;
    bool changeTokens = false;
    if (argc >= 2) {
        for (int i = 1; i < argc; ++i) {
            if (argv[i][0] == '-' && argv[i][1] == 'h' &&
                strlen(argv[1]) <= 2) {
                printUsage(argv);
                return 0;
            } else {
                if (argv[i][0] == '-' && argv[i][1] == 'v' &&
                    strlen(argv[1]) <= 2) {
                    verboseEnabled = true;
                } else if (argv[i][0] == '-' && argv[i][1] == 'c' &&
                           strlen(argv[1]) <= 2) {
                    changeTokens = true;
                } else if (argv[i][0] != '-' ||
                           ((argv[i][0] == '-' &&
                             ((argv[i][1] != 'v' && argv[i][1] != 'c' &&
                               argv[i][1] != 'h') ||
                              strlen(argv[1]) >= 2)))) {
                    std::cerr << "ERROR: Invalid option "
                              << "\'" << argv[i] << "\'\n"
                              << std::endl;
                    printUsage(argv);
                    return 0;
                }
            }
        }
    } 
    curses = std::make_unique<CursesProvider>(verboseEnabled, changeTokens);

    curses->init();
    curses->eventHandler();

    return 0;
}

