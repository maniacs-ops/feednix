#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

#include "curses_provider.h"

// XXX: make it static and const and remove extern call in other file
std::string HOME_PATH = getenv("HOME");
static const std::string CONFIG_DIR_PATH = HOME_PATH + "/.config/feednix";
static const std::string DEFAULT_CONFIG_FILE_PATH = "/etc/xdg/feednix/config.json";
static const std::string USER_CONFIG_FILE_PATH = CONFIG_DIR_PATH + "/config.json";

std::unique_ptr<CursesProvider> curses;
std::string TMPDIR;

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

bool checkIfConfigDirectoryExists() {
    struct stat st;
    if(stat(CONFIG_DIR_PATH.c_str(), &st) == 0){
        if(st.st_mode & S_IFDIR)
                return true;
    }
}

bool createConfigDirectory() {
    int result = mkdir(CONFIG_DIR_PATH.c_str(), 0755);
    return result == 0;
}

bool copyDefaultConfigFile() {
    std::ifstream source(DEFAULT_CONFIG_FILE_PATH, std::ios::binary);
    std::ofstream destination(USER_CONFIG_FILE_PATH, std::ios::binary);

    if(source && destination) {
        destination << source.rdbuf();
        return true;
    }
    return false;
}

int main(int argc, char **argv) {
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGSEGV, sighandler);
    atexit(atExitFunction);

    if(!checkIfConfigDirectoryExists()) {
        if(createConfigDirectory()){
            if(!copyDefaultConfigFile()){
                std::cerr << "Error while copying default config file" 
                    << std::endl;
                return -1;
            }
        }
        else{
            std::cerr << "Error while createing the config directory" 
                << std::endl;
            return -1;
        }
    
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

