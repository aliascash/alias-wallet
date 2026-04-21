// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CHAINPARAMS_COMPAT_H
#define BITCOIN_CHAINPARAMS_COMPAT_H

#include <chainparams.h>
#include <kernel/chainparams.h>
#include <bignum.h>
#include <uint256.h>

/**
 * Compatibility layer for old CChainParams interface
 * Provides backward compatibility for code using the old chainparams.h interface
 */

/**
 * Global functions for backward compatibility
 * These maintain the old interface while using the new kernel/chainparams structure
 */
const CChainParams &Params();
const CChainParams &TestNetParams();
const CChainParams &MainNetParams();
void SelectParams(CChainParams::Network network);
bool SelectParamsFromCommandLine();

/**
 * Compatibility wrapper for old CChainParams methods
 * This allows old code to continue working while using the new structure
 */
namespace ChainParamsCompat {
    // Get the active chain params (old interface)
    const CChainParams& GetParams();
    
    // Convert old Network enum to new ChainType
    ChainType NetworkToChainType(CChainParams::Network network);
    
    // Convert new ChainType to old Network enum
    CChainParams::Network ChainTypeToNetwork(ChainType chain);
}

#endif // BITCOIN_CHAINPARAMS_COMPAT_H

