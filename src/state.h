// SPDX-FileCopyrightText: © 2025 ALIAS Developer
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2014 ShadowCoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef COIN_STATE_H
#define COIN_STATE_H

#include <string>
#include <limits>
#include "util/sync.h"

enum eNodeType
{
    NT_FULL = 1,
    NT_THIN,
    NT_UNKNOWN // end marker
};

enum eNodeState
{
    NS_STARTUP = 1,
    NS_GET_HEADERS,
    NS_GET_FILTERED_BLOCKS,
    NS_READY,

    NS_UNKNOWN // end marker
};

enum eBlockFlags
{
    BLOCK_PROOF_OF_STAKE = (1 << 0), // is proof-of-stake block
    BLOCK_STAKE_ENTROPY  = (1 << 1), // entropy bit for stake modifier
    BLOCK_STAKE_MODIFIER = (1 << 2), // regenerated stake modifier
    // Operator-set or auto-detected: the node will never accept a block built
    // on a parent carrying this flag and will not re-extend its main chain
    // through a tip carrying this flag. Persisted in nFlags via
    // CDiskBlockIndex so the marker survives restarts and peers re-serving
    // the same chain. Set/cleared via invalidateblock / reconsiderblock RPCs
    // or auto-set by the deep-divergence detector in SendMessages.
    BLOCK_FAILED_VALID   = (1 << 3),
};

// Auto-recovery: if the IBD stall watchdog fires this many times in a row
// (BLOCK_ACCEPT_STALL_TIMEOUT each), with no chain advance and a large
// orphan backlog, treat it as evidence we are on a wrong fork that orphan
// resolution cannot reach back through, and auto-invalidate the current tip.
static const int AUTOREORG_STALL_FIRINGS = 3;
// Minimum orphan count required before auto-invalidate fires. Cheap blocks
// drop in steadily during normal operation; we only act on a real pile-up.
static const size_t AUTOREORG_MIN_ORPHANS = 200;


/*  nServices flags
    top 32 bits of CNode::nServices are used to mark services required
*/

enum
{
    NODE_NETWORK = (1 << 0),
    THIN_SUPPORT = (1 << 1),
    THIN_STAKE   = (1 << 2),  // deprecated
    THIN_STEALTH = (1 << 3),
    SMSG_RELAY   = (1 << 4),
};

const int64_t GENESIS_BLOCK_TIME = 1479594600;

static const int64_t COIN = 100000000;
static const int64_t CENT = 1000000;

static const int64_t COIN_YEAR_REWARD = 5 * CENT; // 5% per year

static const int64_t MBLK_RECEIVE_TIMEOUT = 60; // seconds
// IBD stall watchdog: if no block has been ACCEPTED for this many
// seconds while the chain is still well behind peers, force a fresh
// getblocks. Without this, the daemon can sit in a "orphans keep
// arriving so MBLK_RECEIVE_TIMEOUT never fires, but nothing connects"
// loop indefinitely.
static const int64_t BLOCK_ACCEPT_STALL_TIMEOUT = 5 * 60; // seconds

static const int UNSPENT_ANON_BALANCE_MIN = 100;
static const int UNSPENT_ANON_BALANCE_MAX = 200;
static const int UNSPENT_ANON_SELECT_MIN = 20;

extern int nNodeMode;
extern int nNodeState;

extern int nMaxThinPeers;
extern int nBloomFilterElements;

extern int nMinStakeInterval;
extern int nStakingDonation;

extern int nThinIndexWindow;

static const int nTryStakeMempoolTimeout = 5 * 60; // seconds
static const int nTryStakeMempoolMaxAsk = 16;

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
//extern bool fShutdown;
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

/** Fees smaller than this (in satoshi) are considered zero fee (for transaction creation) */
extern int64_t nMinTxFee;
/** Fees smaller than this (in satoshi) are considered zero fee (for relaying) */
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
// Updated in SetBestChain() when the chain genuinely advances. Distinct
// from nTimeLastMblkRecv so the IBD stall watchdog can detect the
// "orphans arriving but nothing connecting" case.
extern int64_t nTimeLastBlockAccepted;


#endif /* COIN_STATE_H */

