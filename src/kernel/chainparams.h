// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_KERNEL_CHAINPARAMS_H
#define BITCOIN_KERNEL_CHAINPARAMS_H

#include <consensus/params.h>
#include <kernel/messagestartchars.h>
#include <core.h>  // Alias uses CBlock from core.h
#include <uint256.h>
#include <util/chaintype.h>
#include <util/hash_type.h>
#include <util/vector.h>

#include <cstdint>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class CBlockIndex;
class CAddress;

struct CDNSSeedData {
    std::string name, host;
    CDNSSeedData(const std::string &strName, const std::string &strHost) : name(strName), host(strHost) {}
};

typedef std::map<int, uint256> MapCheckpoints;

struct CCheckpointData {
    MapCheckpoints mapCheckpoints;

    int GetHeight() const {
        const auto& final_checkpoint = mapCheckpoints.rbegin();
        return final_checkpoint->first /* height */;
    }
};

struct AssumeutxoHash : public BaseHash<uint256> {
    explicit AssumeutxoHash(const uint256& hash) : BaseHash(hash) {}
};

struct AssumeutxoData {
    int height;
    AssumeutxoHash hash_serialized;
    unsigned int nChainTx;
    uint256 blockhash;
};

struct ChainTxData {
    int64_t nTime;
    int64_t nTxCount;
    double dTxRate;
};

/**
 * CChainParams defines various tweakable parameters of a given instance of the
 * Alias blockchain system. Modern structure with Alias-specific extensions.
 */
class CChainParams
{
public:
    enum Network {
        MAIN,
        TESTNET,
        REGTEST,
        MAX_NETWORK_TYPES
    };

    enum Base58Type {
        PUBKEY_ADDRESS,
        SCRIPT_ADDRESS,
        SECRET_KEY,
        STEALTH_ADDRESS,
        EXT_PUBLIC_KEY,
        EXT_SECRET_KEY,
        EXT_KEY_HASH,
        EXT_ACC_HASH,
        EXT_PUBLIC_KEY_BTC,
        EXT_SECRET_KEY_BTC,
        MAX_BASE58_TYPES
    };

    const Consensus::Params& GetConsensus() const { return consensus; }
    const MessageStartChars& MessageStart() const { return pchMessageStart; }
    uint16_t GetDefaultPort() const { return nDefaultPort; }
    
    const CBlock& GenesisBlock() const { return genesis; }
    bool DefaultConsistencyChecks() const { return fDefaultConsistencyChecks; }
    bool IsTestChain() const { return m_chain_type != ChainType::MAIN; }
    bool IsMockableChain() const { return m_is_mockable_chain; }
    uint64_t PruneAfterHeight() const { return nPruneAfterHeight; }
    uint64_t AssumedBlockchainSize() const { return m_assumed_blockchain_size; }
    uint64_t AssumedChainStateSize() const { return m_assumed_chain_state_size; }
    bool MineBlocksOnDemand() const { return consensus.fPowNoRetargeting; }
    std::string GetChainTypeString() const { return ChainTypeToString(m_chain_type); }
    ChainType GetChainType() const { return m_chain_type; }
    const std::vector<CDNSSeedData>& DNSSeeds() const { return vSeeds; }
    const std::vector<unsigned char>& Base58Prefix(Base58Type type) const { return base58Prefixes[type]; }
    const std::vector<CAddress>& FixedSeeds() const { return vFixedSeeds; }
    const CCheckpointData& Checkpoints() const { return checkpointData; }
    const ChainTxData& TxData() const { return chainTxData; }

    // Alias-specific methods (preserved from old structure)
    const uint256& HashGenesisBlock() const { return hashGenesisBlock; }
    const std::vector<unsigned char>& AlertKey() const { return vAlertPubKey; }
    int RPCPort() const { return nRPCPort; }
    int BIP44ID() const { return nBIP44ID; }
    int LastPOWBlock() const { return nLastPOWBlock; }
    std::string NetworkIDString() const { return strNetworkID; }
    const std::string& DataDir() const { return strDataDir; }
    
    // Alias PoS methods
    bool IsProtocolV2(int nHeight) const { return nHeight > nFirstPosv2Block; }
    bool IsProtocolV3(int nHeight) const { return nHeight > nFirstPosv3Block; }
    
    // Alias fork detection
    bool IsForkV2(int64_t nTime) const { return nTime > nForkV2Time; }
    bool IsForkV3(int64_t nTime) const { return nTime > nForkV3Time; }
    bool IsForkV4(int64_t nTime) const { return nTime >= nForkV4Time; }
    bool IsForkV4SupplyIncrease(const CBlockIndex* pindexPrev) const;
    int GetForkId(int64_t nTime) const { 
        return (nTime >= nForkV4Time) ? 4 : (nTime > nForkV3Time) ? 3 : (nTime > nForkV2Time) ? 2 : 0; 
    }
    
    // Alias stake confirmation methods
    int GetStakeMinConfirmations(int64_t nTime) const { 
        return IsForkV3(nTime) ? nStakeMinConfirmations : nStakeMinConfirmationsLegacy; 
    }
    int GetAnonStakeMinConfirmations() const { return nStakeMinConfirmations; }
    
    // Alias reward methods
    int64_t GetProofOfWorkReward(int nHeight, int64_t nFees) const;
    int64_t GetProofOfStakeReward(const CBlockIndex* pindexPrev, int64_t nCoinAge, int64_t nFees) const;
    int64_t GetProofOfAnonStakeReward(const CBlockIndex* pindexPrev, int64_t nFees) const;
    
    // Alias development fund addresses
    const std::string GetDevContributionAddress() const { return devContributionAddress; }
    const std::string GetSupplyIncreaseAddress() const { return supplyIncreaseAddress; }
    
    // Alias proof limits (using CBigNum for compatibility)
    const CBigNum& ProofOfWorkLimit() const { return bnProofOfWorkLimit; }
    const CBigNum& ProofOfStakeLimit(int nHeight) const { 
        return IsProtocolV2(nHeight) ? bnProofOfStakeLimitV2 : bnProofOfStakeLimit; 
    }
    const CBigNum BnProofOfWorkLimit() const { return bnProofOfWorkLimit; }
    const CBigNum BnProofOfStakeLimit() const { return bnProofOfStakeLimit; }
    
    // Network selection
    virtual Network NetworkID() const = 0;
    virtual bool RequireRPCPassword() const { return true; }

    static std::unique_ptr<CChainParams> RegTest();
    static std::unique_ptr<CChainParams> Main();
    static std::unique_ptr<CChainParams> TestNet();

protected:
    CChainParams() {}

    Consensus::Params consensus;
    MessageStartChars pchMessageStart;
    uint16_t nDefaultPort;
    uint64_t nPruneAfterHeight;
    uint64_t m_assumed_blockchain_size;
    uint64_t m_assumed_chain_state_size;
    std::vector<CDNSSeedData> vSeeds;
    std::vector<unsigned char> base58Prefixes[MAX_BASE58_TYPES];
    ChainType m_chain_type;
    CBlock genesis;
    std::vector<CAddress> vFixedSeeds;  // Alias uses CAddress
    bool fDefaultConsistencyChecks;
    bool m_is_mockable_chain;
    CCheckpointData checkpointData;
    std::vector<AssumeutxoData> m_assumeutxo_data;
    ChainTxData chainTxData;

    // Alias-specific members
    uint256 hashGenesisBlock;
    std::vector<unsigned char> vAlertPubKey;
    std::string strNetworkID;
    int nRPCPort;
    int nBIP44ID;
    std::string strDataDir;
    
    // Alias PoS parameters
    int nLastPOWBlock;
    int nFirstPosv2Block;
    int nFirstPosv3Block;
    CBigNum bnProofOfWorkLimit;
    CBigNum bnProofOfStakeLimit;
    CBigNum bnProofOfStakeLimitV2;
    int nStakeMinConfirmationsLegacy;
    int nStakeMinConfirmations;
    
    // Alias fork times
    int64_t nForkV2Time;
    int64_t nForkV3Time;
    int64_t nForkV4Time;
    
    // Alias development addresses
    std::string devContributionAddress;
    std::string supplyIncreaseAddress;
    
    // Alias stake rewards (externally set, referenced in reward functions)
    // These are set in state.cpp and referenced via extern
};

// Global functions for backward compatibility
const CChainParams &Params();
const CChainParams &TestNetParams();
const CChainParams &MainNetParams();
void SelectParams(CChainParams::Network network);
bool SelectParamsFromCommandLine();

std::optional<ChainType> GetNetworkForMagic(const MessageStartChars& pchMessageStart);

#endif // BITCOIN_KERNEL_CHAINPARAMS_H

