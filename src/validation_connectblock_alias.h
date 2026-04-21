// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_VALIDATION_CONNECTBLOCK_ALIAS_H
#define BITCOIN_VALIDATION_CONNECTBLOCK_ALIAS_H

#include <consensus/validation.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <txdb.h>
#include <validation_alias.h>

class CBlockIndex;
class CTxDB;
class CCoinsViewCache;
class Chainstate;

namespace AliasConnectBlock {

/**
 * Alias-specific ConnectBlock integration helpers
 * These functions integrate Alias validation into the modern ConnectBlock flow
 */

/**
 * Process anon transaction in ConnectBlock
 * Alias-specific: Validates anon inputs and counts anon outputs
 * Returns true on success, false on failure
 */
bool ProcessAnonTransaction(const CTransaction& tx, CTxDB& txdb, 
                           int64_t& nAnonIn, int64_t& nAnonOut, 
                           bool& fInvalid, BlockValidationState& state);

/**
 * Validate stake reward for Alias in ConnectBlock
 * Alias-specific: Validates both regular PoS and Anon PoS rewards
 * Also validates dev contribution and supply increase addresses
 * Should be called after processing all transactions
 */
bool ValidateStakeRewardInConnectBlock(const CBlock& block, 
                                      const CBlockIndex* pindexPrev,
                                      int64_t nStakeReward, int64_t nFees, 
                                      int64_t nCoinAge,
                                      BlockValidationState& state);

/**
 * Update Alias-specific block index fields
 * Alias-specific: Updates nAnonSupply tracking
 * Should be called at the end of ConnectBlock before writing to disk
 */
void UpdateAliasBlockIndex(CBlockIndex* pindex, int64_t nAnonOut, int64_t nAnonIn);

/**
 * Check and rebuild anon cache if needed
 * Alias-specific: Manages anon statistics cache
 * Should be called early in ConnectBlock
 */
bool CheckAnonCacheEarly(CBlockIndex* pindex);

} // namespace AliasConnectBlock

#endif // BITCOIN_VALIDATION_CONNECTBLOCK_ALIAS_H
