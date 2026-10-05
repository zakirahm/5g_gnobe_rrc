#include "RrcOtherSibBroadcast.h"
#include <iostream>

GnbOtherSibBroadcastManager::GnbOtherSibBroadcastManager() {
    // Initialize default values for SIB2 and SIB3
    m_sib2Config = {
        .t_ReselectionTimerPresent = true,
        .t_Reselection = 1,          // 1 second default
        .freqPriorityOffset = 0
    };

    m_sib3Config = {
        .pci = 42,
        .q_RxLevMin = -110           // -110 dBm
    };
}

void GnbOtherSibBroadcastManager::registerCallback(LowerLayerCallback cb) {
    m_lowerLayerCallback = cb;
}

std::vector<uint8_t> GnbOtherSibBroadcastManager::encodeSib2(const Sib2Parameters& params) {
    // In production, invoke ASN.1 UPER encoder for SIB2
    std::vector<uint8_t> payload;
    payload.push_back(0x02); // Mock identifier tag for SIB2
    payload.push_back(params.t_Reselection);
    return payload;
}

std::vector<uint8_t> GnbOtherSibBroadcastManager::encodeSib3(const Sib3Parameters& params) {
    // In production, invoke ASN.1 UPER encoder for SIB3
    std::vector<uint8_t> payload;
    payload.push_back(0x03); // Mock identifier tag for SIB3
    payload.push_back(static_cast<uint8_t>(params.pci & 0xFF));
    return payload;
}

void GnbOtherSibBroadcastManager::triggerOtherSibTransmission(uint32_t currentSfn, uint8_t currentSlot) {
    // Example Schedule: SIB2 broadcasted every 320ms (SFN mod 32 == 0)
    if (currentSlot == 0 && (currentSfn % 32) == 0) {
        auto sib2Bytes = encodeSib2(m_sib2Config);
        if (m_lowerLayerCallback) {
            OtherSibTxPrimitive prim{currentSfn, currentSlot, 1, sib2Bytes}; // SI-1 containing SIB2
            m_lowerLayerCallback(prim);
        }
    }

    // Example Schedule: SIB3 broadcasted every 640ms (SFN mod 64 == 0)
    if (currentSlot == 0 && (currentSfn % 64) == 0) {
        auto sib3Bytes = encodeSib3(m_sib3Config);
        if (m_lowerLayerCallback) {
            OtherSibTxPrimitive prim{currentSfn, currentSlot, 2, sib3Bytes}; // SI-2 containing SIB3
            m_lowerLayerCallback(prim);
        }
    }
}
