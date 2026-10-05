#include "RrcMasterScheduler.h"
#include <iostream>
#include <chrono>

RrcMasterScheduler::RrcMasterScheduler() 
    : m_isRunning(false), m_currentSfn(0), m_currentSlot(0) {}

RrcMasterScheduler::~RrcMasterScheduler() {
    stopScheduler();
}

void RrcMasterScheduler::startScheduler() {
    if (m_isRunning) return;
    
    m_isRunning = true;
    m_schedulerThread = std::thread(&RrcMasterScheduler::schedulerLoop, this);
    std::cout << "[RRC-Scheduler] Master RRC broadcast scheduler thread started.\n";
}

void RrcMasterScheduler::stopScheduler() {
    if (!m_isRunning) return;
    
    m_isRunning = false;
    if (m_schedulerThread.joinable()) {
        m_schedulerThread.join();
    }
    std::cout << "[RRC-Scheduler] Master RRC broadcast scheduler thread stopped.\n";
}

GnbBroadcastManager& RrcMasterScheduler::getMibSib1Manager() {
    return m_mibSib1Manager;
}

GnbOtherSibBroadcastManager& RrcMasterScheduler::getOtherSibManager() {
    return m_otherSibManager;
}

void RrcMasterScheduler::schedulerLoop() {
    // For 15kHz SCS, 1 slot = 1ms. 1 frame = 10 slots (SFN increments every 10 slots).
    // This loop simulates a real-time 1ms slot tick.
    while (m_isRunning) {
        auto startTime = std::chrono::steady_clock::now();

        // 1. Trigger MIB and SIB1 checking
        m_mibSib1Manager.triggerBroadcastTransmission(m_currentSfn, m_currentSlot);

        // 2. Trigger Other SIBs checking
        m_otherSibManager.triggerOtherSibTransmission(m_currentSfn, m_currentSlot);

        // Advance slot and frame counters
        m_currentSlot++;
        if (m_currentSlot >= 10) {
            m_currentSlot = 0;
            m_currentSfn = (m_currentSfn + 1) % 1024; // SFN ranges from 0 to 1023
        }

        // Sleep to maintain real-time 1ms slot pacing (simulated timing)
        std::this_thread::sleep_until(startTime + std::chrono::milliseconds(1));
    }
}
