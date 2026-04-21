// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams_compat.h>

#include <chainparams.h>
#include <kernel/chainparams.h>
#include <util/chaintype.h>
#include <util/system.h>
#include <logging.h>

// Note: The actual implementation of Params(), TestNetParams(), MainNetParams(),
// SelectParams(), and SelectParamsFromCommandLine() is in chainparams.cpp
// This compatibility layer provides additional helper functions and will serve
// as a bridge when fully migrating to the new kernel/chainparams structure.

// Forward declarations for static instances in chainparams.cpp
// These are defined in chainparams.cpp as static instances
extern CMainParams mainParams;
extern CTestNetParams testNetParams;
extern CRegTestParams regTestParams;

bool SelectParamsFromCommandLine()
{
    // Select chain params from command line arguments
    // This maintains backward compatibility with old argument parsing
    
    std::string chain = gArgs.GetChainName();
    if (chain == CBaseChainParams::MAIN) {
        SelectParams(CChainParams::MAIN);
        return true;
    } else if (chain == CBaseChainParams::TESTNET) {
        SelectParams(CChainParams::TESTNET);
        return true;
    } else if (chain == CBaseChainParams::REGTEST) {
        SelectParams(CChainParams::REGTEST);
        return true;
    }
    
    return false;
}

namespace ChainParamsCompat {

const CChainParams& GetParams()
{
    return Params();
}

ChainType NetworkToChainType(CChainParams::Network network)
{
    switch (network) {
        case CChainParams::MAIN:
            return ChainType::MAIN;
        case CChainParams::TESTNET:
            return ChainType::TESTNET;
        case CChainParams::REGTEST:
            return ChainType::REGTEST;
        default:
            return ChainType::MAIN;
    }
}

CChainParams::Network ChainTypeToNetwork(ChainType chain)
{
    switch (chain) {
        case ChainType::MAIN:
            return CChainParams::MAIN;
        case ChainType::TESTNET:
            return CChainParams::TESTNET;
        case ChainType::REGTEST:
            return CChainParams::REGTEST;
        default:
            return CChainParams::MAIN;
    }
}

} // namespace ChainParamsCompat

