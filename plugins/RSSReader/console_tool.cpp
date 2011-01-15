#include <term.h>
#include <unistd.h>

#include "console_tool.h"

static char *clearScrCmd = 0;
static char *cursorPosCmd = 0;

void SetCursorPos(int x, int y)
{
    if (ConsoleInit())
        putp(tparm(cursorPosCmd, x, y, 0, 0, 0, 0, 0, 0, 0 ));
}

void ClearScr(bool goHome)
{
    if (ConsoleInit())
    {
        putp(clearScrCmd);
        if (goHome)
            SetCursorPos(0, 0);
    }
}

bool ConsoleInit()
{
    int result;
    if (cur_term)
        return true;
    setupterm(NULL, STDOUT_FILENO, &result);
    if (result > 0)
    {
        clearScrCmd = tigetstr("clear");
        cursorPosCmd = tigetstr("cup");
    }
    return (result > 0);
}

