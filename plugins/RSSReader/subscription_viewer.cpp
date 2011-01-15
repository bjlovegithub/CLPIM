#include "subscription_viewer.h"

#include "Log.h"
#include "console_tool.h"
#include "content_viewer.h"

#include <vector>
#include <cstdio>

using namespace std;
using namespace CLPIM;

void SubscriptionViewer::SetSubInfo(const map<string, string> &info)
{
    mSubInfo = info;
}

void SubscriptionViewer::GetIn()
{
    ClearScr(true);
    cout << "Available subscriptions are:\n ";

    vector<string> subName;
    {
        int count = 1;
        typeof(mSubInfo.begin()) iter;
        for (iter = mSubInfo.begin(); iter != mSubInfo.end(); ++iter)
            subName.push_back(iter->first);

        for (size_t i = 0; i < subName.size(); ++i)
            cout << count++ << ": " << subName[i] << endl;
    }

    while (true)
    {
        cout << "Enter 0 to quit. Select subscription by ID to view the content: ";
        unsigned int id;
        string cmd;
        cin >> cmd;
        if (sscanf(cmd.c_str(), "%u", &id) < 1)
        {
            cout << "Error subscription id: " << cmd << endl;
            continue;
        }
        LOG_DEBUG("Subscritpion id is: " << id);
        
        if (id == 0)
            break;

        if (id >= mSubInfo.size())
        {
            cout << "Invalid subscription id: " << cmd << endl;
            continue;
        }

        ContentViewer cv;
        cv.SetTitle(subName[id]);
        cv.SetUrl(mSubInfo[subName[id]]);
        cv.View();
    }
}
