#ifndef CURSES_PROV_H
#define CURSES_PROV_H

#include <curses.h>
#include <menu.h>
#include <panel.h>
#include <iostream>
#include "feedly_provider.h"

#define CTG_WIN_WIDTH 40
#define VIEW_WIN_HEIGHT_PER 50

class CursesProvider {
   public:
    CursesProvider();
    CursesProvider(bool verbose, bool change);
    ~CursesProvider();
    void init();
    void eventHandler();

   private:
    FeedlyProvider feedly;
    WINDOW *ctgWin = NULL;
    WINDOW *postsWin = NULL;
    WINDOW *viewWin = NULL;
    PANEL *panels[3];
    PANEL *top = NULL;
    ITEM **ctgItems = NULL;
    ITEM **postsItems = NULL;
    MENU *ctgMenu = NULL;
    MENU *postsMenu = NULL;
    MENU *curMenu = NULL;
    std::string lastEntryRead, statusLine[3];
    bool currentRank = 0;
    bool activatePreview = 1;
    bool activeCount = 1;
    bool markReadWhileScroll = 1;
    int totalPosts = 0, numRead = 0, numUnread = 0;
    int viewWinHeightPer = VIEW_WIN_HEIGHT_PER;
    int viewWinHeight = 0;
    int ctgWinWidth = CTG_WIN_WIDTH;

    bool currentCategoryRead;
    void createCategoriesMenu();
    void createPostsMenu();
    void changeSelectedItem(MENU *curMenu, int req);
    void ctgMenuCallback(char *label);
    void postsMenuCallback(ITEM *item, bool preview);
    void newFeedDialog();
    void markItemRead(ITEM *item);
    void updateUnreadCount();
    void win_show(WINDOW *win, char *label, int label_color, bool highlight);
    void print_in_middle(WINDOW *win, int starty, int startx, int width,
                         const char *str, chtype color);
    void print_in_center(WINDOW *win, int starty, int startx, int height,
                         int width, const char *str, chtype color);
    void clear_statusline();
    void update_statusline(const char *update, const char *post,
                           bool showCounter);
    void update_infoline(const char *info);
};
#endif
