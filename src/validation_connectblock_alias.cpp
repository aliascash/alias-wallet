// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <validation_connectblock_alias.h>

#include <chainparams.h>
#include <kernel/chainparams.h>
#include <consensus/validation.h>
#include <logging.h>
#include <ringsig.h>
#include <base58.h>
#include <script.h>
#include <util/strencodings.h>
#include <tinyformat.h>
#include <main.h>
#include <state.h>
#include <wallet.h>

namespace AliasConnectBlock {

bool ProcessAnonTransaction(const CTransaction& tx, CTxDB& txdb, 
                           int64_t& nAnonIn, int64_t& nAnonOut, 
                           bool& fInvalid, BlockValidationState& state)
{
    if (tx.nVersion != ANON_TXN_VERSION)
        return true; // Not an anon transaction, skip
    
    // Count anon outputs
    nAnonOut = AliasValidation::CountAnonOutputs(tx);
    
    // Validate anon inputs
    int64_t nTxAnonIn = 0;
    if (!tx.CheckAnonInputs(txdb, nTxAnonIn, fInvalid, true))
    {
        if (fInvalid)
        {
            LogPrintf("ProcessAnonTransaction() : CheckAnonInputs found invalid tx %s\n", 
                     tx.GetHash().ToString().substr(0,10).c_str());
            return state.Invalid(BlockValidationResult::BLOCK_CONSENSUS, "bad-anon-inputs",
                               strprintf("CheckAnonInputs found invalid tx %s", 
                                        tx.GetHash().ToString().substr(0,10).c_str()));
        }
        return state.Invalid(BlockValidationResult::BLOCK_CONSENSUS, "bad-anon-inputs",
                           "CheckAnonInputs failed");
    }
    
    nAnonIn = nTxAnonIn;
    return true;
}

bool ValidateStakeRewardInConnectBlock(const CBlock& block, 
                                      const CBlockIndex* pindexPrev,
                                      int64_t nStakeReward, int64_t nFees, 
                                      int64_t nCoinAge,
                                      BlockValidationState& state)
{
    return AliasValidation::ValidateStakeReward(block, pindexPrev, nStakeReward, 
                                                nFees, nCoinAge, state);
}

void UpdateAliasBlockIndex(CBlockIndex* pindex, int64_t nAnonOut, int64_t nAnonIn)
{
    AliasValidation::UpdateAnonSupply(pindex, nAnonOut, nAnonIn);
}

bool CheckAnonCacheEarly(CBlockIndex* pindex)
{
    // Alias-specific: Check if anon cache needs rebuilding
    extern bool fStaleAnonCache;
    extern bool fDebugRingSig;
    extern CWallet* pwalletMain;
    
    if (fStaleAnonCache && pindex && pindex->pprev)
    {
        LogPrintf("CheckAnonCacheEarly() : Stale anon cache => rebuild.\n");
        if (pwalletMain && !pwalletMain->CacheAnonStats(pindex->pprev->nHeight))
        {
            LogPrintf("CheckAnonCacheEarly() : CacheAnonStats() failed.\n");
            // Don't fail block validation, just log
        }
    }
    
    if (fDebugRingSig && pindex && pindex->pprev)
    {
        // TODO: Implement validateAnonCache if needed
        // validateAnonCache(pindex->pprev->nHeight);
    }
    
    return true;
}

} // namespace AliasConnectBlock
