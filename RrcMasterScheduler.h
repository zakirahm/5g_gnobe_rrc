#ifndef RRC_MASTER_SCHEDULER_H
#define RRC_MASTER_SCHEDULER_H

#include "RrcBroadcast.h"
#include "RrcOtherSibBroadcast.h"
#include <thread>
#include <atomic>
#include <cstdint>

class RrcMasterScheduler {
public:
    RrcMasterScheduler();
    ~RrcMasterScheduler();

    // Starts the background master scheduling thread
    void startScheduler();

    // Stops the scheduling thread gracefully
    void stopScheduler();

    // References to the individual broadcast modules
    GnbBroadcastManager& getMibSib1Manager();
    GnbOtherSibBroadcastManager& getOtherSibManager();

private:
    // Main execution loop for the thread
    void schedulerLoop();

    GnbBroadcastManager m_mibSib1Manager;
    GnbOtherSibBroadcastManager m_otherSibManager;

    std::thread m_schedulerThread;
    std::atomic<bool> m_isRunning;
    
    // Timing counters
    uint32_t m_currentSfn;
    uint8_t m_currentSlot;
};

#endif // RRC_MASTER_SCHEDULER_H
