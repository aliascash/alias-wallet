// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_VALIDATION_ALIAS_H
#define BITCOIN_VALIDATION_ALIAS_H

#include <consensus/validation.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <txdb.h>
#include <core.h>
#include <state.h>

class CBlockIndex;
class CTxDB;
class CCoinsViewCache;

/**
 * Alias-specific validation helpers
 * These functions preserve Alias-specific validation logic while using modern structures
 */

namespace AliasValidation {

/**
 * Check if block is proof of anon stake
 * Alias-specific: Checks if coinstake transaction is anon version
 */
bool IsProofOfAnonStake(const CBlock& block);

/**
 * Validate anon inputs for a transaction
 * Alias-specific: Validates ring signatures and key images for anon transactions
 */
bool CheckAnonInputsForBlock(const CTransaction& tx, CTxDB& txdb, int64_t& nAnonIn, bool& fInvalid);

/**
 * Validate anon outputs for a transaction
 * Alias-specific: Counts and validates anon outputs
 */
int64_t CountAnonOutputs(const CTransaction& tx);

/**
 * Validate stake reward for Alias
 * Alias-specific: Validates both regular PoS and Anon PoS rewards
 * Also validates dev contribution and supply increase addresses
 */
bool ValidateStakeReward(const CBlock& block, const CBlockIndex* pindexPrev, 
                         int64_t nStakeReward, int64_t nFees, int64_t nCoinAge,
                         BlockValidationState& state);

/**
 * Update anon supply tracking
 * Alias-specific: Tracks anon coin supply separately
 */
void UpdateAnonSupply(CBlockIndex* pindex, int64_t nAnonOut, int64_t nAnonIn);

/**
 * Check anon cache and rebuild if needed
 * Alias-specific: Manages anon statistics cache
 */
bool CheckAndRebuildAnonCache(CBlockIndex* pindex);

} // namespace AliasValidation

#endif // BITCOIN_VALIDATION_ALIAS_H
