// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams_migration.h>

#include <chainparams.h>
#include <chainparams_compat.h>
#include <kernel/chainparams.h>
#include <util/chaintype.h>
#include <util/system.h>

namespace ChainParamsMigration {

const CChainParams& GetParams()
{
    // For now, use compatibility layer
    // Eventually, this will return from the new kernel/chainparams structure
    return Params();
}

ChainType ConvertNetworkToChainType(CChainParams::Network network)
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

CChainParams::Network ConvertChainTypeToNetwork(ChainType chain)
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

bool UseNewChainParams()
{
    // Check if we should use new chainparams structure
    // For now, return false to use old structure
    // Set to true when migration is complete
    return gArgs.GetBoolArg("-newchainparams", false);
}

MessageStartChars GetMessageStart()
{
    const CChainParams& params = GetParams();
    return params.MessageStart();
}

uint16_t GetDefaultPort()
{
    const CChainParams& params = GetParams();
    return params.GetDefaultPort();
}

int GetBIP44ID()
{
    const CChainParams& params = GetParams();
    return params.BIP44ID();
}

int GetRPCPort()
{
    const CChainParams& params = GetParams();
    return params.RPCPort();
}

bool RequireRPCPassword()
{
    const CChainParams& params = GetParams();
    return params.RequireRPCPassword();
}

std::string GetNetworkIDString()
{
    const CChainParams& params = GetParams();
    return params.NetworkIDString();
}

std::string GetDevContributionAddress()
{
    const CChainParams& params = GetParams();
    return params.GetDevContributionAddress();
}

std::string GetSupplyIncreaseAddress()
{
    const CChainParams& params = GetParams();
    return params.GetSupplyIncreaseAddress();
}

const std::vector<unsigned char>& GetBase58Prefix(CChainParams::Base58Type type)
{
    const CChainParams& params = GetParams();
    return params.Base58Prefix(type);
}

int64_t GetProofOfWorkReward(int nHeight, int64_t nFees)
{
    const CChainParams& params = GetParams();
    return params.GetProofOfWorkReward(nHeight, nFees);
}

int64_t GetProofOfStakeReward(const CBlockIndex* pindexPrev, int64_t nCoinAge, int64_t nFees)
{
    const CChainParams& params = GetParams();
    return params.GetProofOfStakeReward(pindexPrev, nCoinAge, nFees);
}

int GetLastPOWBlock()
{
    const CChainParams& params = GetParams();
    return params.LastPOWBlock();
}

int GetStakeMinConfirmations(int64_t nTime)
{
    const CChainParams& params = GetParams();
    return params.GetStakeMinConfirmations(nTime);
}

} // namespace ChainParamsMigration

