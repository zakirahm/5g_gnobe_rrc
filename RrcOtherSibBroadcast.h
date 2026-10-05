#ifndef RRC_OTHER_SIB_BROADCAST_H
#define RRC_OTHER_SIB_BROADCAST_H

#include <cstdint>
#include <vector>
#include <functional>

// Struct representing SIB2 parameters (Common timers and constants for cell reselection)
struct Sib2Parameters {
    bool t_ReselectionTimerPresent;
    uint8_t t_Reselection;          // Evaluation timer for cell reselection
    int32_t freqPriorityOffset;     // Frequency specific offset
};

// Struct representing SIB3 parameters (Intra-frequency neighbor cell info)
struct Sib3Parameters {
    uint32_t pci;                   // Physical Cell ID of neighbor
    int32_t q_RxLevMin;             // Minimum required RX level in the cell
};

// Structure for lower-layer transmission primitive (DL-SCH)
struct OtherSibTxPrimitive {
    uint32_t sfn;
    uint8_t slotNumber;
    uint8_t siMessageType;          // Identifier for which SI message is being sent
    std::vector<uint8_t> pduPayload;
};

class GnbOtherSibBroadcastManager {
public:
    using LowerLayerCallback = std::function<void(const OtherSibTxPrimitive&)>;

    GnbOtherSibBroadcastManager();
    ~GnbOtherSibBroadcastManager() = default;

    // Register callback to pass encoded SI payloads down to MAC/PHY
    void registerCallback(LowerLayerCallback cb);

    // Encodes SIB2 and packages into an SI message payload
    std::vector<uint8_t> encodeSib2(const Sib2Parameters& params);

    // Encodes SIB3 and packages into an SI message payload
    std::vector<uint8_t> encodeSib3(const Sib3Parameters& params);

    // Trigger periodic scheduling for Other SIBs
    void triggerOtherSibTransmission(uint32_t currentSfn, uint8_t currentSlot);

private:
    Sib2Parameters m_sib2Config;
    Sib3Parameters m_sib3Config;
    LowerLayerCallback m_lowerLayerCallback;
};

#endif // RRC_OTHER_SIB_BROADCAST_H
