#ifndef RRC_BROADCAST_H
#define RRC_BROADCAST_H

#include <cstdint>
#include <vector>

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

class GnbBroadcastManager {
public:
    GnbBroadcastManager();
    ~GnbBroadcastManager() = default;

    // Encodes MIB into a byte payload ready for physical layer transmission (BCH)
    std::vector<uint8_t> encodeMib(const MibParameters& params);

    // Encodes SIB1 into a byte payload ready for transport channel mapping (DL-SCH)
    std::vector<uint8_t> encodeSib1(const Sib1Parameters& params);

    // Triggers the broadcast schedule for a transmission time interval (TTI)
    void triggerBroadcastTransmission(uint32_t currentSfn, uint8_t currentSlot);

private:
    MibParameters m_mibConfig;
    Sib1Parameters m_sib1Config;

    // Internal helper for bit-packing operations
    void packBits(uint32_t value, int numBits, std::vector<uint8_t>& buffer, int& bitOffset);
};

#endif // RRC_BROADCAST_H