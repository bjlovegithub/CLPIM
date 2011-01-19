/**
 * FileUtil provides interface for file and directory manipulation functions.
 * By billjeff.
 *
 * Revision History:
 *     Jan/19/2011 - Add RemoveFile.
 */

#ifndef CLPIM_FILE_UTIL_H
#define CLPIM_FILE_UTIL_H

#include <string>
#include <cstdio>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace CLPIM
{

/**
 * FileHandler is just like a @SharedPtr, it wrappers the FILE descriptor
 * and our code has not to concern about file open and close.
 * NOTE: THIS CLASS IS NOT THREAD SAFE.
 */
static const size_t BUFFER_SIZE = 2048;
class FileHandler
{
public:
    /**
     * Constructor
     */
    FileHandler();

    /**
     * Constructor, open a file named @fileName with @mode. Default mode
     * is O_RDONLY.
     */
    FileHandler(const std::string &fileName, int mode = O_RDONLY);
    
    /**
     * Destructor, flush and close file.
     */
    ~FileHandler();

    /**
     * Open a file with @mode.
     * @param filePath File path.
     * @param mode File open mode.
     * @return Return true for opening file successfully, otherwise false.
     */
    bool Open(const std::string &filePath, int mode = O_RDONLY);

    /**
     * Write content to a file.
     * @return true for write successfully, return false for errous.
     */
    bool Write(const std::string &content);

    /**
     * Read some number of characters from file.
     * @param num Number of bytes to be read from file.
     * @param content Keep the content read from file.
     * @return true if read successfully or is not reaching the end of file,
     *         return false for error or reaching the end of file.
     */
    bool Read(size_t num, std::string &content);

    /**
     * Read some number of characters from file at specified offset.
     * @param offset Start position in file.
     * @param num Number of bytes to be read from file.
     * @param content Keep the content read from file.
     * @return true if read successfully or is not reaching the end of file,
     *         return false for error or reaching the end of file.
     */
    bool Read(size_t offset, size_t num, std::string &content);

    /**
     * Read a word from file
     * @param content Keep the readed word. Word is consisting of non-whitespace
     *        characters.
     * @return Return true for successfully read a word, otherwise false.
     * NOTE: If current pointer is at the end of file, ReadWord will return true
     *       with content is an emtpy string.
     */
    bool ReadWord(std::string &content);

    /**
     * Read one line from input file.
     */
    bool ReadLine(std::string &content);

    /**
     * Set current position to other place.
     * @param offset
     * @startPos Available value is SEEK_SET/SEEK_CUR/SEEK_END.
     * @return Return true for operating successfully, return false for 
     *         failure.
     */
    bool Seek(size_t offset, int startPos = SEEK_SET);

    /**
     * Check mFD is OK or not. If mFD is not -1, return true, otherwise
     * return false.
     */
    bool IsOk();

private:
    /**
     * Check parameter c is white space or not. Return true if c is whitespace.
     */
    bool IsWhiteSpace(char c);

private:
    int mFD;
    char mBuffer[BUFFER_SIZE];
    /// pointers point to begin and end position of mBuffer.
    size_t mBegin, mEnd;
};

/**
 * Class FileUtil provides interface to manipulate files and directories.
 */
class FileUtil
{
public:
    /**
     * CreateFolder will create a folder at @filePath with 777 mode(
     * so any user can access this folder).
     * @param filePath should be in the following form:
     *        1. /home/admin/folder -- full path
     *        2. admin/folder       -- relative path
     * @return Return true for creating folder successfully,
     *         otherwise return false.
     */
    static bool CreateFolder(const std::string &filePath);

    /**
     * Delete a folder at @filePath.
     * @param filePath See @CreateFolder
     */
    static bool DeleteFolder(const std::string &filePath);

    /**
     * CheckFile checks whether a file or folder is existed specified by
     * @filePath.
     * @return Return true if file or folder is existed, otherwise return
     *         false.
     */
    static bool CheckFile(const std::string &filePath);

    /**
     * Remove file from specified path.
     * @filePath.
     * @return Return true if operation succeed, otherwise return false.
     */
    static bool RemoveFile(const std::string &filePath);

    /**
     * Get current working directory.
     * @return Current working directory.
     */
    static const char* GetCWD();
};

} // namespace CLPIM

#endif // CLPIM_FILE_UTIL_H
