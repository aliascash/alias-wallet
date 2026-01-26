// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2012 PPCoin Developers
// Copyright (c) 2017-2023 The Particl Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ALIAS_POS_KERNEL_H
#define ALIAS_POS_KERNEL_H

#include <consensus/amount.h>
#include <kernel/cs_main.h>
#include <sync.h>

class CScript;
class uint256;
class COutPoint;
class CBlockIndex;
class Chainstate;
class CChainParams;
class CTransaction;
class BlockValidationState;
namespace node {
class BlockManager;
}

static const int MAX_REORG_DEPTH = 1024;

double GetPoSKernelPS(CBlockIndex *pindex);

/**
 * Compute the hash modifier for proof-of-stake
 */
uint256 ComputeStakeModifierV2(const CBlockIndex *pindexPrev, const uint256 &kernel);

/**
 * Check whether stake kernel meets hash target
 * Sets hashProofOfStake on success return
 */
bool CheckStakeKernelHash(const CBlockIndex *pindexPrev,
    uint32_t nBits, uint32_t nBlockFromTime,
    CAmount prevOutAmount, const COutPoint &prevout, uint32_t nTime,
    uint256 &hashProofOfStake, uint256 &targetProofOfStake,
    bool fPrintProofOfStake=false);

/**
 * Get kernel hash and value for blockindex and coinstake tx
 */
bool GetKernelInfo(const node::BlockManager& blockman, const CBlockIndex *blockindex, const CTransaction &tx, uint256 &hash, CAmount &value, CScript &script, uint256 &blockhash);

/**
 * Check kernel hash target and coinstake signature
 * Sets hashProofOfStake on success return
 * Alias-specific: Supports both regular PoS and Anon PoS
 */
bool CheckProofOfStake(Chainstate &chain_state, BlockValidationState &state, const CBlockIndex *pindexPrev, const CTransaction &tx, int64_t nTime, unsigned int nBits, uint256 &hashProofOfStake, uint256 &targetProofOfStake) EXCLUSIVE_LOCKS_REQUIRED(cs_main);

/**
 * Check whether the coinstake timestamp meets protocol
 */
bool CheckCoinStakeTimestamp(int nHeight, int64_t nTimeBlock);

/**
 * Wrapper around CheckStakeKernelHash()
 * Also checks existence of kernel input and min age
 * Convenient for searching a kernel
 */
bool CheckKernel(Chainstate &chain_state, const CBlockIndex *pindexPrev, unsigned int nBits, int64_t nTime, const COutPoint &prevout, int64_t* pBlockTime = nullptr);

int64_t GetProofOfStakeReward(const CChainParams &chainparams, const CBlockIndex *pindexPrev, int64_t nFees);

#endif // ALIAS_POS_KERNEL_H

