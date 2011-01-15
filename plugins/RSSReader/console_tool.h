#ifndef CONSOLE_TOOL_H
#define CONSOLE_TOOL_H

#include <term.h>
#include <unistd.h>

/**
 * Init console
 */
bool ConsoleInit();

/**
 * @brief Set cursor to the position specified by @param x, @param y.
 */
void SetCursorPos(int x, int y);

/**
 * @brief Clear screen
 * @param goHome If true, then set cursor to the beginning of the line.
 */
void ClearScr(bool goHome);

#endif /// CONSOLE_TOOL_H
