/**
 * Rss Reader by CLPIM
 * 
 * This is Rss Reader plugin for CLPIM. Its main purpose is as an
 * RSS Reader to help manage our RSS feeds.
 *
 * Revision History:
 *
 */

#ifndef RSS_READER_H
#define RSS_READER_H

#include "Task.h"
#include "subscription_manager.h"
#include "subscription_viewer.h"

namespace CLPIM
{
class RSSReader : public Task
{
public:
    /// override
    void Exec();

private:
    SubscriptionManager mSubManager;
    SubscriptionViewer mSubViewer;
};

} // namespace CLPIM

extern "C"
{
    CLPIM::RSSReader* Maker();
    void Cleaner();
}

#endif // RSS_READER_H
