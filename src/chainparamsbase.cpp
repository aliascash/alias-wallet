// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparamsbase.h>

#include <common/args.h>
#include <tinyformat.h>
#include <util/chaintype.h>

#include <assert.h>

// Forward declaration - will be implemented when we migrate ArgsManager
class ArgsManager;
void SetupChainParamsBaseOptions(ArgsManager& argsman)
{
    // TODO: Implement when ArgsManager is migrated
    // For now, this is a placeholder
}

static std::unique_ptr<CBaseChainParams> globalChainBaseParams;

const CBaseChainParams& BaseParams()
{
    assert(globalChainBaseParams);
    return *globalChainBaseParams;
}

/**
 * Port numbers for Alias network
 * Main: RPC 36657, Onion 37347
 * Testnet: RPC 36757, Onion 37111
 */
std::unique_ptr<CBaseChainParams> CreateBaseChainParams(const ChainType chain)
{
    switch (chain) {
    case ChainType::MAIN:
        return std::make_unique<CBaseChainParams>("", 36657, 37347);  // Alias ports
    case ChainType::TESTNET:
        return std::make_unique<CBaseChainParams>("testnet", 36757, 37111);  // Alias testnet ports
    case ChainType::SIGNET:
        return std::make_unique<CBaseChainParams>("signet", 36757, 37111);  // Use testnet ports for signet
    case ChainType::REGTEST:
        return std::make_unique<CBaseChainParams>("regtest", 18444, 18444);
    }
    assert(false);
}

void SelectBaseParams(const ChainType chain)
{
    globalChainBaseParams = CreateBaseChainParams(chain);
    // TODO: Implement gArgs.SelectConfigNetwork when ArgsManager is migrated
    // gArgs.SelectConfigNetwork(ChainTypeToString(chain));
}

