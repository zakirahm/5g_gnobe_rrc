#ifndef RRC_BROADCAST_H
#define RRC_BROADCAST_H

#include <cstdint>
#include <vector>
#include <functional>

// Struct representing MIB parameters according to 3GPP TS 38.331
struct MibParameters {
    uint32_t systemFrameNumber;     // SFN highest 6 bits (out of 10)
    uint8_t subCarrierSpacingCommon;// Subcarrier spacing (e.g., 15kHz, 30kHz)
    uint8_t ssb_SubcarrierOffset;   // Offset between SS/PBCH block and resource block
    uint8_t dmrs_TypeA_Position;    // DMRS position (Pos2 or Pos3)
    uint32_t pdcch_ConfigSIB1;      // Control Resource Set (CORESET) and search space for SIB1
    bool cellBarred;                // Indicates if the cell is barred
    bool intraFreqReselection;      // Allowed intra-frequency reselection
};

// Struct representing basic SIB1 parameters
struct Sib1Parameters {
    uint16_t plmnId;                // Public Land Mobile Network ID
    uint16_t trackingAreaCode;      // TAC
    uint64_t cellIdentity;          // 36-bit Cell Identity
    bool si_SchedulingInfoPresent;  // Flag for other SIBs scheduling
};

// Target transport channel enum
enum class BroadcastChannel {
    BCH,    // Used for MIB
    DL_SCH  // Used for SIB1
};

// Primitive structure sent down to MAC/PHY layer
struct TxDataRequestPrimitive {
    uint32_t sfn;
    uint8_t slotNumber;
    BroadcastChannel channel;
    std::vector<uint8_t> pduPayload;
};

class GnbBroadcastManager {
public:
    // Define callback signature for lower-layer data delivery
    using LowerLayerCallback = std::function<void(const TxDataRequestPrimitive&)>;

    GnbBroadcastManager();
    ~GnbBroadcastManager() = default;

    // Register the lower-layer callback function
    void registerCallback(LowerLayerCallback cb);

    // Encodes MIB and SIB1 payload bytes
    std::vector<uint8_t> encodeMib(const MibParameters& params);
    std::vector<uint8_t> encodeSib1(const Sib1Parameters& params);

    // Triggers broadcast scheduling and pushes data to the registered callback
    void triggerBroadcastTransmission(uint32_t currentSfn, uint8_t currentSlot);

private:
    MibParameters m_mibConfig;
    Sib1Parameters m_sib1Config;
    LowerLayerCallback m_lowerLayerCallback;
};

#endif // RRC_BROADCAST_H
