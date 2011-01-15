#include <iostream>
#include <stdio.h>

#include "Log.h"
#include "rss_reader.h"
#include "console_tool.h"
#include "GlobalDef.h"

using namespace std;
using namespace CLPIM;

RSSReader *readerPtr;

RSSReader* Maker()
{
    InitLog();
    readerPtr = new (nothrow) RSSReader;

    return readerPtr;

}

void Cleaner()
{
    delete readerPtr;
}

void RSSReader::Exec()
{
    ClearScr(true);
    while (true)
    {
        bool goOut = false;

        cout << "Rss Reader for CLPIM\n"
             << "Available commands:\n"
             << "1. Import subscriptions from Google Reader.\n"
             << "2. Show subscriptions list.\n"
             << "3. Quit.\n"
             << "Enter 1 or 2 or 3... \n"
             << "Command: ";
        int cmd;
        /// parse command
        {
            string tmp;
            cin >> tmp;
            if (sscanf(tmp.c_str(), "%d", &cmd) < 1)
            {
                cout << "Invalid command: " << tmp << endl;
                continue;
            }
        }

        switch(cmd)
        {
        case 1:
            if (!mSubManager.ImportFromGoogleReader())
                cout << "Import subscription error" << endl;
            break;
        case 2:
            mSubViewer.SetSubInfo(mSubManager.GetSubscriptions());
            mSubViewer.GetIn();
            break;
        case 3:
            goOut = true;
            mSubManager.DumpSubs();
            break;
        default:
            break;
        } // switch

        if (goOut)
            break;
    } // while
}
