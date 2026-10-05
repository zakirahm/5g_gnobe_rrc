#include "RrcBroadcast.h"
#include <iostream>
#include <cstring>

GnbBroadcastManager::GnbBroadcastManager() {
    // Initialize standard default configurations
    m_mibConfig = {
        .systemFrameNumber = 0,
        .subCarrierSpacingCommon = 0, // 15kHz for FR1
        .ssb_SubcarrierOffset = 0,
        .dmrs_TypeA_Position = 0,     // Pos2
        .pdcch_ConfigSIB1 = 16,
        .cellBarred = false,
        .intraFreqReselection = true
    };

    m_sib1Config = {
        .plmnId = 0x0001,
        .trackingAreaCode = 123,
        .cellIdentity = 0xABCDEF012,
        .si_SchedulingInfoPresent = true
    };
}

void GnbBroadcastManager::registerCallback(LowerLayerCallback cb) {
    m_lowerLayerCallback = cb;
}

std::vector<uint8_t> GnbBroadcastManager::encodeMib(const MibParameters& params) {
    // MIB fixed to 3 bytes layout simulation
    std::vector<uint8_t> mibPayload(3, 0);
    mibPayload[0] = static_cast<uint8_t>((params.systemFrameNumber >> 4) & 0x3F);
    mibPayload[1] = static_cast<uint8_t>((params.subCarrierSpacingCommon << 7) | 
                                         ((params.ssb_SubcarrierOffset & 0x0F) << 3) | 
                                         (params.dmrs_TypeA_Position & 0x01));
    mibPayload[2] = static_cast<uint8_t>(params.pdcch_ConfigSIB1 & 0xFF);
    return mibPayload;
}

std::vector<uint8_t> GnbBroadcastManager::encodeSib1(const Sib1Parameters& params) {
    std::vector<uint8_t> sib1Payload;
    sib1Payload.push_back(static_cast<uint8_t>((params.plmnId >> 8) & 0xFF));
    sib1Payload.push_back(static_cast<uint8_t>(params.plmnId & 0xFF));
    sib1Payload.push_back(static_cast<uint8_t>((params.trackingAreaCode >> 8) & 0xFF));
    sib1Payload.push_back(static_cast<uint8_t>(params.trackingAreaCode & 0xFF));
    return sib1Payload;
}

void GnbBroadcastManager::triggerBroadcastTransmission(uint32_t currentSfn, uint8_t currentSlot) {
    // MIB transmission schedule: every 80ms (SFN mod 8 == 0, Slot 0)
    if (currentSlot == 0 && (currentSfn % 8) == 0) {
        m_mibConfig.systemFrameNumber = currentSfn;
        auto mibBytes = encodeMib(m_mibConfig);

        if (m_lowerLayerCallback) {
            TxDataRequestPrimitive req{currentSfn, currentSlot, BroadcastChannel::BCH, mibBytes};
            m_lowerLayerCallback(req); // Dispatched to lower layer
        }
    }

    // SIB1 transmission schedule: every 160ms (SFN mod 16 == 0, Slot 0)
    if (currentSlot == 0 && (currentSfn % 16) == 0) {
        auto sib1Bytes = encodeSib1(m_sib1Config);

        if (m_lowerLayerCallback) {
            TxDataRequestPrimitive req{currentSfn, currentSlot, BroadcastChannel::DL_SCH, sib1Bytes};
            m_lowerLayerCallback(req); // Dispatched to lower layer
        }
    }
}
