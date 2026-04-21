// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2014 ShadowCoin Developers
//
// SPDX-License-Identifier: MIT

#include "state.h"

namespace alias {

static NodeSettings g_node_settings;
static DebugFlags g_debug_flags;
static RuntimeState g_runtime_state;

NodeSettings& GetNodeSettings() { return g_node_settings; }
DebugFlags& GetDebugFlags() { return g_debug_flags; }
RuntimeState& GetRuntimeState() { return g_runtime_state; }

} // namespace alias

// Legacy global variables - these wrap the new structures for backward compatibility
int nNodeMode = NT_FULL;
int nNodeState = NS_STARTUP;

int nMaxThinPeers = 8;
int nBloomFilterElements = 1536;
int nMinStakeInterval = 0;
int nStakingDonation = 0;
int nThinIndexWindow = 4096;

uint64_t nLocalServices     = NODE_NETWORK | THIN_SUPPORT | THIN_STEALTH | SMSG_RELAY;
uint32_t nLocalRequirements = NODE_NETWORK;

bool fTestNet = false;
bool fDebug = false;
bool fDebugNet = false;
bool fDebugChain = false;
bool fDebugRingSig = false;
bool fDebugPoS = false;
bool fPrintToConsole = false;
bool fPrintToDebugLog = true;
bool fDaemon = false;
bool fServer = false;
bool fCommandLine = false;
std::string strMiscWarning;
bool fNoListen = false;
bool fLogTimestamps = false;
bool fReopenDebugLog = false;
bool fThinFullIndex = false;
bool fReindexing = false;
bool fHaveGUI = false;
volatile bool fIsStaking = false;
volatile bool fIsStakingEnabled = false;
bool fMakeExtKeyInitials = false;
volatile bool fPassGuiAddresses = false;

bool fConfChange = false;
bool fEnforceCanonical = false;
bool fUseFastIndex = false;
unsigned int nNodeLifespan = 0;
unsigned int nDerivationMethodIndex = 0;
unsigned int nMinerSleep = 0;
unsigned int nBlockMaxSize = 0;
unsigned int nBlockPrioritySize = 0;
unsigned int nBlockMinSize = 0;

int64_t nMinTxFee = 10000;
int64_t nMinRelayTxFee = nMinTxFee;

int64_t nStakeReward = 2 * alias::COIN;
int64_t nAnonStakeReward = 3 * alias::COIN;

unsigned int nStakeSplitAge = 1 * 24 * 60 * 60;
int64_t nStakeCombineThreshold = 1000 * alias::COIN;
int64_t nStakeSplitThreshold = 2 * nStakeCombineThreshold;
int64_t nMaxAnonOutput = 1000 * alias::COIN;
int64_t nMaxAnonStakeOutput = nMaxAnonOutput;

uint32_t nExtKeyLookAhead = 10;

int64_t nTimeLastMblkRecv = 0;

