set(SOURCES curses_provider.cpp
    feedly_provider.cpp
    main.cpp)

set(HEADERS curses_provider.h
    feedly_provider.h)

add_executable(feednix ${SOURCES})

target_link_libraries(feednix curl jsoncpp ncurses menu panel)
