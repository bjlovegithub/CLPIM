#include <term.h>
#include <unistd.h>
#include <iostream>

using namespace std;

static char *clearscreen    = 0;
static char *cursorposition = 0;

void cursorxy(int col, int line);
void clear(bool gohome = true);


static bool console_initialize_terminfo()
{
    int result;
    if (cur_term)
        return true;
    setupterm(NULL, STDOUT_FILENO, &result);
    if (result > 0)
    {
        clearscreen    = tigetstr("clear");
        cursorposition = tigetstr("cup");
    }
    return (result > 0);
}

void cursorxy(int col, int line)
{
    if (console_initialize_terminfo())
        putp(tparm(cursorposition, line, col,
                   0, 0, 0, 0, 0, 0, 0 ));
}

void clear(bool gohome)
{
    if (console_initialize_terminfo())
    {
        putp(clearscreen);
        if (gohome)
            cursorxy(0, 0);
    }
}

int main(void)
{
    int c;
    while(true)
    {
        cout << "1 for set cursor, 2 for clear screen, 3 for quit" 
             << endl;
        cin >> c;

        bool goOut = false;
        int x, y;
        switch(c)
        {
        case 1:
            cout << "Enter position (x, y): ";
            cin >> x >> y;
            cursorxy(x, y);
            break;
        case 2:
            clear(true);
            break;
        case 3:
            goOut = true;
            break;
        case 4:
            cout << "Invalid Command!" << endl;
            break;
        } /// switch
        if (goOut)
            break;
    } /// while

    return 0;
}
