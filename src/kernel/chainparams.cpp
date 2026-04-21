// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <chainparamsseeds.h>
#include <state.h>  // For COIN, CENT, COIN_YEAR_REWARD, GENESIS_BLOCK_TIME
#include <bignum.h>  // For CBigNum
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <hash.h>
#include <kernel/messagestartchars.h>
#include <core.h>
#include <script.h>  // For CScript
#include <uint256.h>
#include <util/chaintype.h>
// ChainTypeToString is implemented in util/chaintype.cpp
#include <util/strencodings.h>
#include <util.h>  // For ARRAYLEN, GetTime, GetRand, ParseHex, etc.
#include <netbase.h>  // For CService, CAddress
#include <main.h>  // For CBlockIndex, nStakeReward, nAnonStakeReward

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

// External references to stake rewards (set in state.cpp)
extern int64_t nStakeReward;
extern int64_t nAnonStakeReward;
extern bool fDebug;
extern bool fTestNet;

// Forward declarations
static void convertSeeds(std::vector<CAddress> &vSeedsOut, const unsigned int *data, unsigned int count, int port);

// Helper function for uint256 from hex string (compatibility)
inline uint256 uint256S(const char* str) {
    return uint256(str);
}
inline uint256 uint256S(const std::string& str) {
    return uint256(str);
}

// Convert the pnSeeds array into usable address objects.
static void convertSeeds(std::vector<CAddress> &vSeedsOut, const unsigned int *data, unsigned int count, int port)
{
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int k = 0; k < count; ++k)
    {
        struct in_addr ip;
        unsigned int i = data[k], t;

        // Convert to big endian
        t =   (i & 0x000000ff) << 24u
            | (i & 0x0000ff00) << 8u
            | (i & 0x00ff0000) >> 8u
            | (i & 0xff000000) >> 24u;

        memcpy(&ip, &t, sizeof(ip));

        CAddress addr(CService(ip, port));
        addr.nTime = GetTime()-GetRand(nOneWeek)-nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

// Alias-specific reward functions
int64_t CChainParams::GetProofOfWorkReward(int nHeight, int64_t nFees) const
{
    int64_t nSubsidy = 0;

    if(nHeight == 1)
        nSubsidy = (NetworkID() == CChainParams::TESTNET ? 2000000 : 20000000) * COIN;

    else if(nHeight <= nLastPOWBlock)
        nSubsidy = 0;

    if (fDebug && GetBoolArg("-printcreation"))
        LogPrintf("GetProofOfWorkReward() : create=%s nSubsidy=%d\n", FormatMoney(nSubsidy).c_str(), nSubsidy);

    return nSubsidy;
}

int64_t CChainParams::GetProofOfStakeReward(const CBlockIndex* pindexPrev, int64_t nCoinAge, int64_t nFees) const
{
    int64_t nSubsidy;

    if (IsForkV4SupplyIncrease(pindexPrev))
        nSubsidy = (NetworkID() == CChainParams::TESTNET ? 300000 : 3000000) * COIN;
    else if (IsProtocolV3(pindexPrev->nHeight))
        nSubsidy = IsForkV3(pindexPrev->GetBlockTime()) ?
                    nStakeReward :
                    (pindexPrev->nMoneySupply / COIN) * COIN_YEAR_REWARD / (365 * 24 * (60 * 60 / 64));
    else
        nSubsidy = nCoinAge * COIN_YEAR_REWARD * 33 / (365 * 33 + 8);

    if (fDebug && GetBoolArg("-printcreation"))
    {
        if (IsProtocolV3(pindexPrev->nHeight))
            LogPrintf("GetProofOfStakeReward(): create=%s\n", FormatMoney(nSubsidy).c_str());
        else
            LogPrintf("GetProofOfStakeReward(): create=%s nCoinAge=%d\n", FormatMoney(nSubsidy).c_str(), nCoinAge);
    }

    return nSubsidy + nFees;
}

int64_t CChainParams::GetProofOfAnonStakeReward(const CBlockIndex* pindexPrev, int64_t nFees) const
{
    int64_t nSubsidy = nAnonStakeReward;
    if (IsForkV4SupplyIncrease(pindexPrev))
        nSubsidy = (NetworkID() == CChainParams::TESTNET ? 300000 : 3000000) * COIN;

    if (fDebug && GetBoolArg("-printcreation"))
        LogPrintf("GetProofOfAnonStakeReward(): create=%s\n", FormatMoney(nSubsidy).c_str());

    return nSubsidy + nFees;
}

bool CChainParams::IsForkV4SupplyIncrease(const CBlockIndex* pindexPrev) const
{
    return pindexPrev->GetBlockTime() >= nForkV4Time && pindexPrev->pprev->GetBlockTime() < nForkV4Time;
}

// Create Alias genesis block
static CBlock CreateAliasGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits)
{
    const char* pszTimestamp = "https://www.cryptocoinsnews.com/encrypted-services-exec-bitcoins-price-history-follows-gartners-hype-cycle/";
    CTransaction txNew;
    txNew.nTime = GENESIS_BLOCK_TIME;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 0 << CBigNum(42) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].SetEmpty();
    
    CBlock genesis;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock = 0;
    genesis.hashMerkleRoot = genesis.BuildMerkleTree();
    genesis.nVersion = 1;
    genesis.nTime = GENESIS_BLOCK_TIME;
    genesis.nBits = nBits;
    genesis.nNonce = nNonce;
    
    return genesis;
}

/**
 * Main network
 */
class CMainParams : public CChainParams {
public:
    CMainParams() 
        : m_chain_type(ChainType::MAIN)
        , strNetworkID("main")
        , strDataDir("")
    {

        // Message start
        pchMessageStart[0] = 0xb5;
        pchMessageStart[1] = 0x22;
        pchMessageStart[2] = 0x5c;
        pchMessageStart[3] = 0xd3;

        vAlertPubKey = ParseHex("04f7bbad03208ea942e292080854d422d046d457949ea70ad3306438fc8357343dccaa73e52291ebe07de85c6701d88d87af2d29c2e3b024fb0ad53f045a6d3ad6");

        nDefaultPort = 37347;
        nRPCPort = 36657;
        nBIP44ID = 0x800000d5;

        // PoS parameters
        nLastPOWBlock = 17000;
        nFirstPosv2Block = 17001;
        nFirstPosv3Block = 17010;

        // Proof limits
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 20);
        bnProofOfStakeLimit = CBigNum(~uint256(0) >> 20);
        bnProofOfStakeLimitV2 = CBigNum(~uint256(0) >> 48);

        nStakeMinConfirmationsLegacy = 288;
        nStakeMinConfirmations = 450; // block time 96 seconds * 450 = 12 hours

        // Consensus parameters
        consensus.hashGenesisBlock = uint256();
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 0;
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 0;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("0000000000000fffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetSpacing = 96;  // Alias: 96 seconds
        consensus.nPowTargetTimespan = 24 * 60 * 60; // 24 hours (moving average)
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016;
        
        // Alias-specific consensus (stored in consensus params)
        consensus.m_alias_mode = true;
        consensus.nForkV2Time = 1534888800; // MAINNET V2 chain fork
        consensus.nForkV3Time = 1558123200; // MAINNET V3 chain fork
        consensus.nForkV4Time = 1569614400; // MAINNET V4 chain fork
        consensus.nLastPOWBlock = 17000;
        consensus.nFirstPosv2Block = 17001;
        consensus.nFirstPosv3Block = 17010;
        consensus.m_min_ringsize = 10;
        consensus.m_max_ringsize = 10;
        consensus.m_max_anon_inputs = 32;
        consensus.m_max_anon_output = 1000 * COIN;
        consensus.nMinRCTOutputDepth = 12;

        // Genesis block
        genesis = CreateAliasGenesisBlock(GENESIS_BLOCK_TIME, 715015, bnProofOfWorkLimit.GetCompact());
        hashGenesisBlock = genesis.GetHash();
        consensus.hashGenesisBlock = hashGenesisBlock;
        
        assert(hashGenesisBlock == uint256("0x000001fd6111f0d71d90b7d8c827c6028dbc867f6c527d90794a0d22f68fecd4"));
        assert(genesis.hashMerkleRoot == uint256("0x48d79d88cdf7d5c84dbb2ffb4fcaab253cebe040a4e7b46cdd507fbb93623e3f"));

        // Base58 prefixes
        base58Prefixes[PUBKEY_ADDRESS]      = std::vector<unsigned char>(1, 63);
        base58Prefixes[SCRIPT_ADDRESS]      = std::vector<unsigned char>(1, 136);
        base58Prefixes[SECRET_KEY]          = std::vector<unsigned char>(1, 179);
        base58Prefixes[STEALTH_ADDRESS]     = std::vector<unsigned char>(1, 40);
        base58Prefixes[EXT_PUBLIC_KEY]      = {0x2C, 0x51, 0x3B, 0xD7};
        base58Prefixes[EXT_SECRET_KEY]      = {0x2C, 0x51, 0xC1, 0x5A};
        base58Prefixes[EXT_KEY_HASH]        = std::vector<unsigned char>(1, 137);
        base58Prefixes[EXT_ACC_HASH]        = std::vector<unsigned char>(1, 83);
        base58Prefixes[EXT_PUBLIC_KEY_BTC]  = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY_BTC]  = {0x04, 0x88, 0xAD, 0xE4};

        // DNS seeds
        vSeeds.push_back(CDNSSeedData("node1.spectreproject.io", "node1.spectreproject.io"));
        vSeeds.push_back(CDNSSeedData("node2.spectreproject.io", "node2.spectreproject.io"));
        vSeeds.push_back(CDNSSeedData("node3.spectreproject.io", "node3.spectreproject.io"));
        vSeeds.push_back(CDNSSeedData("node4.spectreproject.io", "node4.spectreproject.io"));

        convertSeeds(vFixedSeeds, pnSeed, ARRAYLEN(pnSeed), nDefaultPort);

        // Fork times
        nForkV2Time = 1534888800; // MAINNET V2 chain fork (GMT: Tuesday, 21. August 2018 22.00)
        nForkV3Time = 1558123200; // MAINNET V3 chain fork (GMT: Friday, 17. May 2019 20:00:00)
        nForkV4Time = 1569614400; // MAINNET V4 chain fork (GMT: Friday, 27. September 2019 20:00:00)

        devContributionAddress = "SdrdWNtjD7V6BSt3EyQZKCnZDkeE28cZhr";
        supplyIncreaseAddress = "SSGCEMb6xESgmuGXkx7yozGDxhVSXzBP3a";

        // Modern Bitcoin Core parameters
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 4;
        m_assumed_chain_state_size = 3;
        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
        
        checkpointData = {
            {
                // Add checkpoints as needed
            }
        };
        
        chainTxData = ChainTxData{
            0, // nTime
            0, // nTxCount
            0.0 // dTxRate
        };
    }

    Network NetworkID() const override { return CChainParams::MAIN; }
};

/**
 * Testnet
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() 
        : m_chain_type(ChainType::TESTNET)
        , strNetworkID("test")
        , strDataDir("testnet")
    {

        // Message start
        pchMessageStart[0] = 0xa3;
        pchMessageStart[1] = 0x2c;
        pchMessageStart[2] = 0x44;
        pchMessageStart[3] = 0xb4;

        vAlertPubKey = ParseHex("04e564bc9bf28e6d395cd89c4d2bdb235c3873f59b1330d2e6a30c6fa85d8a8637693ae367ce39c2fe0f4e8e3c7c3a34feb82305388f19030aa4fcd4955abeb810");

        nDefaultPort = 37111;
        nRPCPort = 36757;
        nBIP44ID = 0x80000001;

        // PoS parameters
        nLastPOWBlock = 20;
        nFirstPosv2Block = 20;
        nFirstPosv3Block = 500;

        // Proof limits
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 1);
        bnProofOfStakeLimit = CBigNum(~uint256(0) >> 20);
        bnProofOfStakeLimitV2 = CBigNum(~uint256(0) >> 46);

        nStakeMinConfirmationsLegacy = 28;
        nStakeMinConfirmations = 30;

        // Consensus parameters
        consensus.hashGenesisBlock = uint256();
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.BIP34Height = 0;
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 0;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("0000000000000fffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetSpacing = 96;
        consensus.nPowTargetTimespan = 24 * 60 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512;
        consensus.nMinerConfirmationWindow = 2016;
        
        // Alias-specific consensus (stored in consensus params)
        consensus.m_alias_mode = true;
        consensus.nForkV2Time = 1532466000; // TESTNET V2 chain fork
        consensus.nForkV3Time = 1546470000; // TESTNET V3 chain fork
        consensus.nForkV4Time = 1567972800; // TESTNET V4 chain fork
        consensus.nLastPOWBlock = 20;
        consensus.nFirstPosv2Block = 20;
        consensus.nFirstPosv3Block = 500;
        consensus.m_min_ringsize = 10;
        consensus.m_max_ringsize = 10;
        consensus.m_max_anon_inputs = 32;
        consensus.m_max_anon_output = 1000 * COIN;
        consensus.nMinRCTOutputDepth = 12;

        // Genesis block
        genesis = CreateAliasGenesisBlock(GENESIS_BLOCK_TIME, 20, bnProofOfWorkLimit.GetCompact());
        hashGenesisBlock = genesis.GetHash();
        consensus.hashGenesisBlock = hashGenesisBlock;
        
        assert(hashGenesisBlock == uint256("0x0a3e03a153b1713ebc1f03fefa5d013bba4d2677ae189fcb727396b98043d95c"));

        // Base58 prefixes
        base58Prefixes[PUBKEY_ADDRESS]      = std::vector<unsigned char>(1, 127);
        base58Prefixes[SCRIPT_ADDRESS]      = std::vector<unsigned char>(1, 196);
        base58Prefixes[SECRET_KEY]          = std::vector<unsigned char>(1, 255);
        base58Prefixes[STEALTH_ADDRESS]     = std::vector<unsigned char>(1, 40);
        base58Prefixes[EXT_PUBLIC_KEY]      = {0x76, 0xC0, 0xFD, 0xFB};
        base58Prefixes[EXT_SECRET_KEY]      = {0x76, 0xC1, 0x07, 0x7A};
        base58Prefixes[EXT_KEY_HASH]        = std::vector<unsigned char>(1, 75);
        base58Prefixes[EXT_ACC_HASH]        = std::vector<unsigned char>(1, 23);
        base58Prefixes[EXT_PUBLIC_KEY_BTC]  = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY_BTC]  = {0x04, 0x35, 0x83, 0x94};

        convertSeeds(vFixedSeeds, pnTestnetSeed, ARRAYLEN(pnTestnetSeed), nDefaultPort);

        // Fork times
        nForkV2Time = 1532466000; // TESTNET V2 chain fork
        nForkV3Time = 1546470000; // TESTNET V3 chain fork
        nForkV4Time = 1567972800; // TESTNET V4 chain fork

        devContributionAddress = "tSJoPZoXumJyDmGKYo9Y7SZkJvymESFYkD";
        supplyIncreaseAddress = devContributionAddress;

        // Modern Bitcoin Core parameters
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 1;
        m_assumed_chain_state_size = 1;
        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = true;
        
        checkpointData = {
            {
                // Add checkpoints as needed
            }
        };
        
        chainTxData = ChainTxData{
            0, // nTime
            0, // nTxCount
            0.0 // dTxRate
        };
    }

    Network NetworkID() const override { return CChainParams::TESTNET; }
};

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() 
        : m_chain_type(ChainType::REGTEST)
        , strNetworkID("regtest")
        , strDataDir("regtest")
    {

        nFirstPosv2Block = -1;
        nFirstPosv3Block = -1;

        pchMessageStart[0] = 0x05;
        pchMessageStart[1] = 0xc5;
        pchMessageStart[2] = 0x04;
        pchMessageStart[3] = 0x3a;
        
        bnProofOfWorkLimit = CBigNum(~uint256(0) >> 1);
        genesis = CreateAliasGenesisBlock(1479594600, 2, bnProofOfWorkLimit.GetCompact());
        hashGenesisBlock = genesis.GetHash();
        consensus.hashGenesisBlock = hashGenesisBlock;
        
        nDefaultPort = 18444;
        consensus.fPowNoRetargeting = true;
        consensus.fPowAllowMinDifficultyBlocks = true;

        assert(hashGenesisBlock == uint256("0x562dba63b74b056329585b9779306f3d3caf447b5df40fb088cebbfb31fd5d5d"));

        vSeeds.clear();
        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;
    }

    bool RequireRPCPassword() const override { return false; }
    Network NetworkID() const override { return CChainParams::REGTEST; }
};

// Global instances
static std::unique_ptr<CChainParams> globalChainParams;

static CMainParams mainParams;
static CTestNetParams testNetParams;
static CRegTestParams regTestParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

const CChainParams &TestNetParams() {
    return testNetParams;
}

const CChainParams &MainNetParams() {
    return mainParams;
}

void SelectParams(CChainParams::Network network)
{
    switch (network)
    {
        case CChainParams::MAIN:
            globalChainParams = std::make_unique<CMainParams>();
            break;
        case CChainParams::TESTNET:
            globalChainParams = std::make_unique<CTestNetParams>();
            break;
        case CChainParams::REGTEST:
            globalChainParams = std::make_unique<CRegTestParams>();
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    }
}

bool SelectParamsFromCommandLine()
{
    bool fRegTest = GetBoolArg("-regtest", false);
    bool fTestNet = GetBoolArg("-testnet", false);

    if (fTestNet && fRegTest)
    {
        return false;
    }

    if (fRegTest)
    {
        SelectParams(CChainParams::REGTEST);
    } else if (fTestNet)
    {
        SelectParams(CChainParams::TESTNET);
    } else
    {
        SelectParams(CChainParams::MAIN);
    }

    return true;
}

std::unique_ptr<CChainParams> CChainParams::Main()
{
    return std::make_unique<CMainParams>();
}

std::unique_ptr<CChainParams> CChainParams::TestNet()
{
    return std::make_unique<CTestNetParams>();
}

std::unique_ptr<CChainParams> CChainParams::RegTest()
{
    return std::make_unique<CRegTestParams>();
}

std::optional<ChainType> GetNetworkForMagic(const MessageStartChars& pchMessageStart)
{
    if (pchMessageStart[0] == 0xb5 && pchMessageStart[1] == 0x22 && 
        pchMessageStart[2] == 0x5c && pchMessageStart[3] == 0xd3) {
        return ChainType::MAIN;
    } else if (pchMessageStart[0] == 0xa3 && pchMessageStart[1] == 0x2c && 
               pchMessageStart[2] == 0x44 && pchMessageStart[3] == 0xb4) {
        return ChainType::TESTNET;
    } else if (pchMessageStart[0] == 0x05 && pchMessageStart[1] == 0xc5 && 
               pchMessageStart[2] == 0x04 && pchMessageStart[3] == 0x3a) {
        return ChainType::REGTEST;
    }
    return std::nullopt;
}

