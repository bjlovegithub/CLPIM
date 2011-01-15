#ifndef RSS_READER_COMMON_H
#define RSS_READER_COMMON_H

#define CONF_FILE "rss_reader_conf"

#include <string>

/**
 * @Brief Automatically close file when dtor is called.
 */
class FileHandler
{
public:
    FileHandler(const char *filePath);
    ~FileHandler();
};

/**
 * @brief Article holdes the meta info of each subscription entry.
 */
struct Article
{
    std::string mTitle;
    std::string mContent;
    std::string mUrl;
    bool mIsReaded;
};

#endif /// RSS_READER_COMMON_H
