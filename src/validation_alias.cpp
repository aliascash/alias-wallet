// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <validation_alias.h>

#include <chainparams.h>
#include <kernel/chainparams.h>
#include <consensus/validation.h>
#include <logging.h>
#include <ringsig.h>
#include <base58.h>
#include <script.h>
#include <util/strencodings.h>
#include <tinyformat.h>

namespace AliasValidation {

bool IsProofOfAnonStake(const CBlock& block)
{
    if (!block.IsProofOfStake() || block.vtx.size() < 2)
        return false;
    
    // Alias-specific: Check if coinstake transaction is anon version
    return (block.vtx[1]->nVersion == ANON_TXN_VERSION);
}

bool CheckAnonInputsForBlock(const CTransaction& tx, CTxDB& txdb, int64_t& nAnonIn, bool& fInvalid)
{
    if (tx.nVersion != ANON_TXN_VERSION)
        return true; // Not an anon transaction, skip
    
    int64_t nTxAnonIn = 0;
    // Note: CheckAnonInputs is a method on CTransaction, but we need non-const reference
    // For now, this is a placeholder - actual implementation will be integrated into ConnectBlock
    // where we have access to the transaction object
    (void)tx;
    (void)txdb;
    (void)nAnonIn;
    (void)fInvalid;
    
    // TODO: Integrate actual CheckAnonInputs call when migrating ConnectBlock
    // The actual call would be: tx.CheckAnonInputs(txdb, nTxAnonIn, fInvalid, true)
    
    return true;
}

int64_t CountAnonOutputs(const CTransaction& tx)
{
    int64_t nAnonOut = 0;
    for (const auto& txout : tx.vout)
    {
        if (txout.IsAnonOutput())
            nAnonOut += txout.nValue;
    }
    return nAnonOut;
}

bool ValidateStakeReward(const CBlock& block, const CBlockIndex* pindexPrev, 
                         int64_t nStakeReward, int64_t nFees, int64_t nCoinAge,
                         BlockValidationState& state)
{
    if (!block.IsProofOfStake() || block.vtx.size() < 2)
        return true; // Not a PoS block
    
    const CTransaction& coinstake = *block.vtx[1];
    
    // Alias-specific: Calculate stake reward based on type
    int64_t nCalculatedStakeReward;
    if (IsProofOfAnonStake(block))
    {
        nCalculatedStakeReward = Params().GetProofOfAnonStakeReward(pindexPrev, nFees);
    }
    else
    {
        nCalculatedStakeReward = Params().GetProofOfStakeReward(pindexPrev, nCoinAge, nFees);
    }
    
    // Check reward amount
    if (nStakeReward > nCalculatedStakeReward)
    {
        LogPrintf("ERROR: ValidateStakeReward() : coinstake pays too much (actual=%d vs calculated=%d)\n", 
                 nStakeReward, nCalculatedStakeReward);
        return state.Invalid(BlockValidationResult::DOS_100, "bad-cs-amount", 
                            strprintf("coinstake pays too much (actual=%d vs calculated=%d)", 
                                     nStakeReward, nCalculatedStakeReward));
    }
    
    // Alias-specific: Check dev contribution or supply increase address
    bool fSupplyIncrease = Params().IsForkV4SupplyIncrease(pindexPrev);
    if (fSupplyIncrease || (Params().IsForkV2(block.nTime) && pindexPrev->nHeight % 6 == 0))
    {
        std::string strAddress = fSupplyIncrease ? Params().GetSupplyIncreaseAddress() : 
                                                Params().GetDevContributionAddress();
        CBitcoinAddress address(strAddress);
        if (!address.IsValid())
        {
            LogPrintf("ValidateStakeReward() : invalid address %s\n", strAddress.c_str());
            return state.Invalid(BlockValidationResult::DOS_100, "bad-cs-address",
                               strprintf("invalid address %s", strAddress.c_str()));
        }
        
        CScript scriptPubKey;
        scriptPubKey.SetDestination(address.Get());
        
        bool containsDonation = false;
        
        // The donation can be at i = 2 or above
        for (size_t i = 2; i < coinstake.vout.size(); i++)
        {
            if (coinstake.vout[i].scriptPubKey == scriptPubKey)
            {
                if (coinstake.vout[i].nValue >= nCalculatedStakeReward)
                {
                    containsDonation = true;
                    break;
                }
            }
        }
        
        if (!containsDonation)
        {
            LogPrintf("ValidateStakeReward() : stake does not pay to address %s\n", 
                     address.ToString().c_str());
            return state.Invalid(BlockValidationResult::DOS_100, "bad-cs-donation",
                               strprintf("stake does not pay to address %s", 
                                        address.ToString().c_str()));
        }
    }
    
    return true;
}

void UpdateAnonSupply(CBlockIndex* pindex, int64_t nAnonOut, int64_t nAnonIn)
{
    // Alias-specific: Track anon coin supply separately
    pindex->nAnonSupply = (pindex->pprev ? pindex->pprev->nAnonSupply : 0) + nAnonOut - nAnonIn;
}

bool CheckAndRebuildAnonCache(CBlockIndex* pindex)
{
    // Alias-specific: Check if anon cache needs rebuilding
    // Note: fStaleAnonCache and pwalletMain are global variables defined elsewhere
    // This function should be called from ConnectBlock when needed
    // For now, return true - actual implementation will be integrated into ConnectBlock
    (void)pindex; // Suppress unused parameter warning
    return true;
}

} // namespace AliasValidation
