find_path(JsonCpp_INCLUDE_DIR
    NAME json/features.h
    PATH_SUFFIXES jsoncpp
    PATHS ${JsonCpp_PKGCONF_INCLUDE_DIRS}
)
