// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2014 ShadowCoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef SPEC_CORE_H
#define SPEC_CORE_H

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <inttypes.h>

#include "util.h"
#include "serialize.h"
#include "script.h"
#include "ringsig.h"

#include <random>
#include <boost/random/mersenne_twister.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

enum GetMinFee_mode
{
    GMF_BLOCK,
    GMF_RELAY,
    GMF_SEND,
};

class CTransaction;

/** An outpoint - a combination of a transaction hash and an index n into its vout */
class COutPoint
{
public:
    uint256 hash;
    unsigned int n;

    COutPoint() { SetNull(); }
    COutPoint(uint256 hashIn, unsigned int nIn) { hash = hashIn; n = nIn; }
    IMPLEMENT_SERIALIZE( READWRITE(FLATDATA(*this)); )
    void SetNull() { hash = 0; n = (unsigned int) -1; }
    bool IsNull() const { return (hash == 0 && n == (unsigned int) -1); }

    friend bool operator<(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash < b.hash || (a.hash == b.hash && a.n < b.n));
    }

    friend bool operator==(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash == b.hash && a.n == b.n);
    }

    friend bool operator!=(const COutPoint& a, const COutPoint& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        return strprintf("COutPoint(%s, %u)", hash.ToString().substr(0,10).c_str(), n);
    }

    void print() const
    {
        LogPrintf("%s\n", ToString().c_str());
    }
};

/** An inpoint - a combination of a transaction and an index n into its vin */
class CInPoint
{
public:
    CTransaction* ptx;
    unsigned int n;

    CInPoint() { SetNull(); }
    CInPoint(CTransaction* ptxIn, unsigned int nIn) { ptx = ptxIn; n = nIn; }
    void SetNull() { ptx = NULL; n = (unsigned int) -1; }
    bool IsNull() const { return (ptx == NULL && n == (unsigned int) -1); }
};



/** An input of a transaction.  It contains the location of the previous
 * transaction's output that it claims and a signature that matches the
 * output's public key.
 */
class CTxIn
{
public:
    COutPoint prevout;
    CScript scriptSig;
    unsigned int nSequence;

    CTxIn()
    {
        nSequence = std::numeric_limits<unsigned int>::max();
    }

    explicit CTxIn(COutPoint prevoutIn, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=std::numeric_limits<unsigned int>::max())
    {
        prevout = prevoutIn;
        scriptSig = scriptSigIn;
        nSequence = nSequenceIn;
    }

    CTxIn(uint256 hashPrevTx, unsigned int nOut, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=std::numeric_limits<unsigned int>::max())
    {
        prevout = COutPoint(hashPrevTx, nOut);
        scriptSig = scriptSigIn;
        nSequence = nSequenceIn;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(prevout);
        READWRITE(scriptSig);
        READWRITE(nSequence);
    )

    bool IsFinal() const
    {
        return (nSequence == std::numeric_limits<unsigned int>::max());
    }

    bool IsAnonInput() const
    {
        return (scriptSig.size() >= MIN_ANON_IN_SIZE
            && scriptSig[0] == OP_RETURN
            && scriptSig[1] == OP_ANON_MARKER);
    }

    friend bool operator==(const CTxIn& a, const CTxIn& b)
    {
        return (a.prevout   == b.prevout &&
                a.scriptSig == b.scriptSig &&
                a.nSequence == b.nSequence);
    }

    friend bool operator!=(const CTxIn& a, const CTxIn& b)
    {
        return !(a == b);
    }


    std::string ToString() const
    {
        std::string str;
        str += "CTxIn(";
        str += prevout.ToString();
        if (prevout.IsNull())
            str += strprintf(", coinbase %s", HexStr(scriptSig).c_str());
        else
            str += strprintf(", scriptSig=%s", scriptSig.ToString().substr(0,24).c_str());
        if (nSequence != std::numeric_limits<unsigned int>::max())
            str += strprintf(", nSequence=%u", nSequence);
        str += ")";
        return str;
    }

    void print() const
    {
        LogPrintf("%s\n", ToString().c_str());
    }

    void ExtractKeyImage(ec_point& kiOut) const
    {
        kiOut.resize(EC_COMPRESSED_SIZE);
        memcpy(&kiOut[0], prevout.hash.begin(), 32);
        kiOut[32] = prevout.n & 0xFF;
    };

    int ExtractRingSize() const
    {
        return (prevout.n >> 16) & 0xFFFF;
    };

};




/** An output of a transaction.  It contains the public key that the next input
 * must be able to sign with to claim it.
 */
class CTxOut
{
public:
    int64_t nValue;
    CScript scriptPubKey;

    CTxOut()
    {
        SetNull();
    }

    CTxOut(int64_t nValueIn, CScript scriptPubKeyIn)
    {
        nValue = nValueIn;
        scriptPubKey = scriptPubKeyIn;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(nValue);
        READWRITE(scriptPubKey);
    )

    void SetNull()
    {
        nValue = -1;
        scriptPubKey.clear();
    }

    bool IsNull()
    {
        return (nValue == -1);
    }

    void SetEmpty()
    {
        nValue = 0;
        scriptPubKey.clear();
    }

    bool IsEmpty() const
    {
        return (nValue == 0 && scriptPubKey.empty());
    }

    bool IsAnonOutput() const
    {
        return (scriptPubKey.size() >= MIN_ANON_OUT_SIZE
            && scriptPubKey[0] == OP_RETURN
            && scriptPubKey[1] == OP_ANON_MARKER);
    }


    uint256 GetHash() const
    {
        return SerializeHash(*this);
    }

    friend bool operator==(const CTxOut& a, const CTxOut& b)
    {
        return (a.nValue       == b.nValue &&
                a.scriptPubKey == b.scriptPubKey);
    }

    friend bool operator!=(const CTxOut& a, const CTxOut& b)
    {
        return !(a == b);
    }

    friend bool operator<(const CTxOut& a, const CTxOut& b)
    {
        return (a.nValue < b.nValue);
    }

    std::string ToString() const
    {
        if (IsEmpty()) return "CTxOut(empty)";
        return strprintf("CTxOut(nValue=%s, scriptPubKey=%s)", FormatMoney(nValue).c_str(), scriptPubKey.ToString().c_str());
    }

    void print() const
    {
        LogPrintf("%s\n", ToString().c_str());
    }

    CPubKey ExtractAnonPk() const
    {
        // always use IsAnonOutput to check length
        return CPubKey(&scriptPubKey[2+1], EC_COMPRESSED_SIZE);
    };
};




class CKeyImageSpent
{
// stored in txdb, key is keyimage
public:
    CKeyImageSpent() {};

    CKeyImageSpent(uint256& txnHash_, uint32_t inputNo_, int64_t nValue_)
    {
        txnHash = txnHash_;
        inputNo = inputNo_;
        nValue  = nValue_;
    };

    uint256 txnHash;    // hash of spending transaction
    uint32_t inputNo;   // keyimage is for inputNo of txnHash
    int64_t nValue;     // reporting only
    int nBlockHeight;   // block which included the spent

    IMPLEMENT_SERIALIZE
    (
        READWRITE(txnHash);
        READWRITE(inputNo);
        READWRITE(nValue);
        READWRITE(nBlockHeight);
    )
};

class CAnonOutput
{
// stored in txdb, key is pubkey
public:

    CAnonOutput() {};

    CAnonOutput(COutPoint& outpoint_, int64_t nValue_, int nBlockHeight_, uint8_t nCompromised_, char fCoinStake_)
    {
        outpoint = outpoint_;
        nValue = nValue_;
        nBlockHeight = nBlockHeight_;
        nCompromised = nCompromised_;
        fCoinStake = fCoinStake_;
    }

    COutPoint outpoint;
    int64_t nValue;         // rather store 2 bytes, digit + power 10 ?
    int nBlockHeight;
    uint8_t nCompromised;   // TODO: mark if output can be identified (spent with ringsig 1)
    char fCoinStake;
    IMPLEMENT_SERIALIZE
    (
        READWRITE(outpoint);
        READWRITE(nValue);
        READWRITE(nBlockHeight);
        READWRITE(nCompromised);
        READWRITE(fCoinStake);
    )
};

class CAnonOutputCount
{ // CountAllAnonOutputs
public:

    CAnonOutputCount()
    {
        nValue = 0;
        nExists = 0;
        nSpends = 0;
        nOwned = 0;
        nLastHeight = 0;
        nCompromised = 0;
        nStakes = 0;
        nMature = 0;
        nMixins = 0;
        nMixinsStaking = 0;
        nCompromisedHeight = 0;
    }

    CAnonOutputCount(int64_t nValue_, int nExists_, int nUnconfirmed_, int nSpends_, int nOwned_, int nLastHeight_, int nCompromised_, int nMature_, int nMixins_, int nMixinsStaking_, int nStakes_, int nCompromisedHeight_)
    {
        nValue = nValue_;
        nExists = nExists_;
        nSpends = nSpends_;
        nUnconfirmed = nUnconfirmed_;
        nOwned = nOwned_;
        nLastHeight = nLastHeight_;
        nCompromised = nCompromised_;
        nMature = nMature_;
        nMixins = nMixins_;
        nMixinsStaking = nMixinsStaking_;
        nStakes = nStakes_;
        nCompromisedHeight = nCompromisedHeight_;
    }

    void set(int64_t nValue_, int nExists_, int nUnconfirmed_, int nSpends_, int nOwned_, int nLastHeight_, int nCompromised_, int nMature_, int nMixins_, int nMixinsStaking_, int nStakes_, int nCompromisedHeight_)
    {
        nValue = nValue_;
        nExists = nExists_;
        nSpends = nSpends_;
        nUnconfirmed = nUnconfirmed_;
        nOwned = nOwned_;
        nLastHeight = nLastHeight_;
        nCompromised = nCompromised_;
        nMature = nMature_;
        nMixins = nMixins_;
        nMixinsStaking = nMixinsStaking_;
        nStakes = nStakes_;
        nCompromisedHeight = nCompromisedHeight_;
    }

    void addCoin(int nBlockHeight, int64_t nCoinValue, bool fStake)
    {
        nExists++;
        nValue = nCoinValue;
        nStakes += fStake;
        if (nBlockHeight > nLastHeight)
            nLastHeight = nBlockHeight;
    }

    void updateDepth(int nBlockHeight, int64_t nCoinValue)
    {
        nValue = nCoinValue;
        if (nBlockHeight > nLastHeight)
            nLastHeight = nBlockHeight;
    }

    void incSpends(int64_t nCoinValue)
    {
        nSpends++;
        nValue = nCoinValue;
    }

    void decSpends(int64_t nCoinValue)
    {
        nSpends--;
        nValue = nCoinValue;
    }

    void incExists(int64_t nCoinValue)
    {
        nExists++;
        nValue = nCoinValue;
    }

    void decExists(int64_t nCoinValue)
    {
        nExists--;
        nValue = nCoinValue;
    }

    int numOfUnspends()
    {
        return nExists - nSpends;
    }

    int numOfMatureUnspends()
    {
        return nMature - nSpends;
    }


    int64_t nValue;
    int nExists;
    int nUnconfirmed;
    int nSpends;
    int nOwned; // todo
    int nLastHeight;
    int nCompromised;
    int nCompromisedHeight;
    int nMature;
    int nMixins;
    int nMixinsStaking;
    int nStakes;
};


class CStakeModifier
{
// for CheckKernel
public:
    CStakeModifier() {};
    CStakeModifier(uint64_t modifier, uint256 modifierv2, int height, int64_t time)
        : nModifier(modifier), bnModifierV2(modifierv2), nHeight(height), nTime(time)
    {};

    uint64_t nModifier;
    uint256 bnModifierV2;
    int nHeight;
    int64_t nTime;
};


struct CTxMixins
{
    CTxMixins(uint256 txHash_)
    {
        txHash = txHash_;
    }

    uint256 txHash;
    mutable std::vector<std::pair<unsigned int, CPubKey>> vOutPubKeys;
};

using namespace boost::multi_index;
// tags
struct TXHASH{};
typedef boost::multi_index_container<
    CTxMixins,
    indexed_by<
        random_access<>,
        ordered_unique<tag<TXHASH>, member<CTxMixins,uint256,&CTxMixins::txHash> >
    >
> txMixins_container;

enum TxMixinsContainerId { OLD, RECENT };
class CTxMixinsContainers
{
private:
    txMixins_container old;
    txMixins_container recent;
public:
    txMixins_container& get(int containerId)
    {
        return containerId == RECENT ? recent : old;
    }
};

class CMixins
{
// for mixin selection
public:
    CMixins() : CMixins(initUrng()) {}
    void AddAnonOutput(CPubKey& pkAo, CAnonOutput& anonOutput, int blockHeight);
    bool Pick(int64_t nValue, uint8_t nMixins, std::vector<CPubKey>& vPickedAnons);
private:
    CMixins(std::mt19937 urng) : urng(urng) {}
    static std::mt19937 initUrng()
    {
        std::random_device rd;
        return std::mt19937(rd());
    }
    std::vector<std::pair<int, uint256>> vUsedTx; // vector with used transaction hashes as pair of containerId and tx hash
    std::map<int64_t, CTxMixinsContainers> mapMixins; // value to CTxMixinsSet
    std::mt19937 urng;
};

#endif  // SPEC_CORE_H

