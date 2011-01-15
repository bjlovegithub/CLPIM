/**
 * This file contains:
 *   1. Macroes for Unicode related operations and controls.
 *   2. Hleper functions for character set convertions.
 */

#ifndef CLPIM_UNICODE_H
#define CLPIM_UNICODE_H

namespace CLPIM
{
/// The internal character set is UTF-8
#define INTERNAL_CHAR_SET "utf-8"

/**
 * Convert source string from one character set into another character set.
 * @param from Character set name for source string.
 * @param to Character set name for destinate string.
 * @param src Source string.
 * @return Return a pointer to the desinate string, or return NULL for failure.
 */
char* CharSetConvert(const char *to, const char *from, char *src);

/**
 * Convert string from Internal Character Set into user's local character set.
 * @param src Source string.
 * @return Return a pointer to the desinate string, or return NULL for failure.
 */
char* InternalToLocal(char *src);

/**
 * Convert string from user's character set into Internal Character Set.
 * @param src Source string.
 * @return Return a pointer to the desinate string, or return NULL for failure.
 */
char* LocalToInternal(char *src);

/**
 * Detect input string's character set.
 * Supported charset is defined in Unicode.cpp files.
 * @param src Input string.
 * @return Charset name string.
 */
const char* GetCharSet(char *src);


} // namespace CLPIM

#endif // CLPIM_UNICODE_H
