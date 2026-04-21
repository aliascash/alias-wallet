// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2014 ShadowCoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef ALIAS_STATE_H
#define ALIAS_STATE_H

#include <atomic>
#include <cstdint>
#include <limits>
#include <string>

#include "sync.h"

namespace alias {

enum class NodeType : int {
    Full = 1,
    Thin,
    Unknown
};

enum class NodeState : int {
    Startup = 1,
    GetHeaders,
    GetFilteredBlocks,
    Ready,
    Unknown
};

enum BlockFlags : uint32_t {
    BLOCK_PROOF_OF_STAKE = (1 << 0),
    BLOCK_STAKE_ENTROPY  = (1 << 1),
    BLOCK_STAKE_MODIFIER = (1 << 2),
};

enum ServiceFlags : uint64_t {
    NODE_NETWORK = (1 << 0),
    THIN_SUPPORT = (1 << 1),
    THIN_STAKE   = (1 << 2),  // deprecated
    THIN_STEALTH = (1 << 3),
    SMSG_RELAY   = (1 << 4),
};

constexpr int64_t GENESIS_BLOCK_TIME = 1479594600;
constexpr int64_t COIN = 100000000;
constexpr int64_t CENT = 1000000;
constexpr int64_t COIN_YEAR_REWARD = 5 * CENT;
constexpr int64_t MBLK_RECEIVE_TIMEOUT = 60;
constexpr int UNSPENT_ANON_BALANCE_MIN = 100;
constexpr int UNSPENT_ANON_BALANCE_MAX = 200;
constexpr int UNSPENT_ANON_SELECT_MIN = 20;
constexpr int TRY_STAKE_MEMPOOL_TIMEOUT = 5 * 60;
constexpr int TRY_STAKE_MEMPOOL_MAX_ASK = 16;

struct NodeSettings {
    int nMaxThinPeers{8};
    int nBloomFilterElements{1536};
    int nMinStakeInterval{0};
    int nStakingDonation{0};
    int nThinIndexWindow{4096};
    
    uint64_t nLocalServices{NODE_NETWORK | THIN_SUPPORT | THIN_STEALTH | SMSG_RELAY};
    uint32_t nLocalRequirements{NODE_NETWORK};
    
    unsigned int nNodeLifespan{0};
    unsigned int nDerivationMethodIndex{0};
    unsigned int nMinerSleep{0};
    unsigned int nBlockMaxSize{0};
    unsigned int nBlockPrioritySize{0};
    unsigned int nBlockMinSize{0};
    
    int64_t nMinTxFee{10000};
    int64_t nMinRelayTxFee{10000};
    int64_t nStakeReward{2 * COIN};
    int64_t nAnonStakeReward{3 * COIN};
    
    unsigned int nStakeSplitAge{24 * 60 * 60};
    int64_t nStakeCombineThreshold{1000 * COIN};
    int64_t nStakeSplitThreshold{2000 * COIN};
    int64_t nMaxAnonOutput{1000 * COIN};
    int64_t nMaxAnonStakeOutput{1000 * COIN};
    
    uint32_t nExtKeyLookAhead{10};
};

struct DebugFlags {
    std::atomic<bool> fDebug{false};
    std::atomic<bool> fDebugNet{false};
    std::atomic<bool> fDebugChain{false};
    std::atomic<bool> fDebugRingSig{false};
    std::atomic<bool> fDebugPoS{false};
    std::atomic<bool> fPrintToConsole{false};
    std::atomic<bool> fPrintToDebugLog{true};
    std::atomic<bool> fLogTimestamps{false};
    std::atomic<bool> fReopenDebugLog{false};
};

struct RuntimeState {
    std::atomic<int> nNodeMode{static_cast<int>(NodeType::Full)};
    std::atomic<int> nNodeState{static_cast<int>(NodeState::Startup)};
    std::atomic<bool> fTestNet{false};
    std::atomic<bool> fDaemon{false};
    std::atomic<bool> fServer{false};
    std::atomic<bool> fCommandLine{false};
    std::atomic<bool> fNoListen{false};
    std::atomic<bool> fThinFullIndex{false};
    std::atomic<bool> fReindexing{false};
    std::atomic<bool> fHaveGUI{false};
    std::atomic<bool> fIsStaking{false};
    std::atomic<bool> fIsStakingEnabled{false};
    std::atomic<bool> fMakeExtKeyInitials{false};
    std::atomic<bool> fPassGuiAddresses{false};
    std::atomic<bool> fConfChange{false};
    std::atomic<bool> fEnforceCanonical{false};
    std::atomic<int64_t> nTimeLastMblkRecv{0};
    std::string strMiscWarning;
};

NodeSettings& GetNodeSettings();
DebugFlags& GetDebugFlags();
RuntimeState& GetRuntimeState();

inline const char* GetNodeStateName(NodeState state) {
    switch (state) {
        case NodeState::Startup: return "Startup";
        case NodeState::GetHeaders: return "GetHeaders";
        case NodeState::GetFilteredBlocks: return "GetFilteredBlocks";
        case NodeState::Ready: return "Ready";
        default: return "Unknown";
    }
}

} // namespace alias

// Legacy compatibility - gradually migrate away from these
enum eNodeType {
    NT_FULL = static_cast<int>(alias::NodeType::Full),
    NT_THIN = static_cast<int>(alias::NodeType::Thin),
    NT_UNKNOWN = static_cast<int>(alias::NodeType::Unknown)
};

enum eNodeState {
    NS_STARTUP = static_cast<int>(alias::NodeState::Startup),
    NS_GET_HEADERS = static_cast<int>(alias::NodeState::GetHeaders),
    NS_GET_FILTERED_BLOCKS = static_cast<int>(alias::NodeState::GetFilteredBlocks),
    NS_READY = static_cast<int>(alias::NodeState::Ready),
    NS_UNKNOWN = static_cast<int>(alias::NodeState::Unknown)
};

enum eBlockFlags {
    BLOCK_PROOF_OF_STAKE = alias::BLOCK_PROOF_OF_STAKE,
    BLOCK_STAKE_ENTROPY  = alias::BLOCK_STAKE_ENTROPY,
    BLOCK_STAKE_MODIFIER = alias::BLOCK_STAKE_MODIFIER,
};

enum {
    NODE_NETWORK = alias::NODE_NETWORK,
    THIN_SUPPORT = alias::THIN_SUPPORT,
    THIN_STAKE   = alias::THIN_STAKE,
    THIN_STEALTH = alias::THIN_STEALTH,
    SMSG_RELAY   = alias::SMSG_RELAY,
};

constexpr int64_t GENESIS_BLOCK_TIME = alias::GENESIS_BLOCK_TIME;
static const int64_t COIN = alias::COIN;
static const int64_t CENT = alias::CENT;
static const int64_t COIN_YEAR_REWARD = alias::COIN_YEAR_REWARD;
static const int64_t MBLK_RECEIVE_TIMEOUT = alias::MBLK_RECEIVE_TIMEOUT;
static const int UNSPENT_ANON_BALANCE_MIN = alias::UNSPENT_ANON_BALANCE_MIN;
static const int UNSPENT_ANON_BALANCE_MAX = alias::UNSPENT_ANON_BALANCE_MAX;
static const int UNSPENT_ANON_SELECT_MIN = alias::UNSPENT_ANON_SELECT_MIN;
static const int nTryStakeMempoolTimeout = alias::TRY_STAKE_MEMPOOL_TIMEOUT;
static const int nTryStakeMempoolMaxAsk = alias::TRY_STAKE_MEMPOOL_MAX_ASK;

extern int nNodeMode;
extern int nNodeState;
extern int nMaxThinPeers;
extern int nBloomFilterElements;
extern int nMinStakeInterval;
extern int nStakingDonation;
extern int nThinIndexWindow;
extern uint64_t nLocalServices;
extern uint32_t nLocalRequirements;
extern bool fTestNet;
extern bool fDebug;
extern bool fDebugNet;
extern bool fDebugChain;
extern bool fDebugRingSig;
extern bool fDebugPoS;
extern bool fPrintToConsole;
extern bool fPrintToDebugLog;
extern bool fDaemon;
extern bool fServer;
extern bool fCommandLine;
extern std::string strMiscWarning;
extern bool fNoListen;
extern bool fLogTimestamps;
extern bool fReopenDebugLog;
extern bool fThinFullIndex;
extern bool fReindexing;
extern bool fHaveGUI;
extern volatile bool fIsStaking;
extern volatile bool fIsStakingEnabled;
extern bool fMakeExtKeyInitials;
extern volatile bool fPassGuiAddresses;
extern bool fConfChange;
extern bool fEnforceCanonical;
extern unsigned int nNodeLifespan;
extern unsigned int nDerivationMethodIndex;
extern unsigned int nMinerSleep;
extern unsigned int nBlockMaxSize;
extern unsigned int nBlockPrioritySize;
extern unsigned int nBlockMinSize;
extern int64_t nMinTxFee;
extern int64_t nMinRelayTxFee;
extern int64_t nStakeReward;
extern int64_t nAnonStakeReward;
extern unsigned int nStakeSplitAge;
extern int64_t nStakeSplitThreshold;
extern int64_t nStakeCombineThreshold;
extern int64_t nMaxAnonOutput;
extern int64_t nMaxAnonStakeOutput;
extern uint32_t nExtKeyLookAhead;
extern int64_t nTimeLastMblkRecv;

inline const char* GetNodeStateName(int state) {
    return alias::GetNodeStateName(static_cast<alias::NodeState>(state));
}

#endif // ALIAS_STATE_H

