// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CHAINPARAMS_MIGRATION_H
#define BITCOIN_CHAINPARAMS_MIGRATION_H

#include <chainparams.h>
#include <kernel/chainparams.h>
#include <kernel/messagestartchars.h>
#include <util/chaintype.h>

class CBlockIndex;

/**
 * Migration helpers for gradually moving from old chainparams to new kernel/chainparams
 * 
 * These functions help bridge the gap between old and new chainparams structures
 * during the migration process
 */

namespace ChainParamsMigration {

/**
 * Get chain params using new structure
 * Returns the active chain params from the new kernel/chainparams structure
 */
const CChainParams& GetParams();

/**
 * Convert old Network enum to new ChainType
 */
ChainType ConvertNetworkToChainType(CChainParams::Network network);

/**
 * Convert new ChainType to old Network enum
 */
CChainParams::Network ConvertChainTypeToNetwork(ChainType chain);

/**
 * Check if we should use new chainparams structure
 * Returns true if migration to new structure is complete
 */
bool UseNewChainParams();

/**
 * Get message start characters from new structure
 */
MessageStartChars GetMessageStart();

/**
 * Get default port from new structure
 */
uint16_t GetDefaultPort();

/**
 * Get BIP44 ID from new structure
 */
int GetBIP44ID();

/**
 * Get RPC port from chain params
 */
int GetRPCPort();

/**
 * Check if RPC password is required
 */
bool RequireRPCPassword();

/**
 * Get network ID string
 */
std::string GetNetworkIDString();

/**
 * Get dev contribution address
 */
std::string GetDevContributionAddress();

/**
 * Get supply increase address
 */
std::string GetSupplyIncreaseAddress();

/**
 * Get Base58 prefix for a given type
 */
const std::vector<unsigned char>& GetBase58Prefix(CChainParams::Base58Type type);

/**
 * Get proof of work reward
 */
int64_t GetProofOfWorkReward(int nHeight, int64_t nFees);

/**
 * Get proof of stake reward
 */
int64_t GetProofOfStakeReward(const CBlockIndex* pindexPrev, int64_t nCoinAge, int64_t nFees);

/**
 * Get last PoW block
 */
int GetLastPOWBlock();

/**
 * Get stake min confirmations
 */
int GetStakeMinConfirmations(int64_t nTime);

} // namespace ChainParamsMigration

#endif // BITCOIN_CHAINPARAMS_MIGRATION_H

