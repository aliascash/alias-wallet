// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2014 ShadowCoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef SPEC_RINGSIG_H
#define SPEC_RINGSIG_H

#include "stealth.h"
#include "state.h"
#include "types.h"

class CPubKey;

enum ringsigType
{
    RING_SIG_1 = 1,
    RING_SIG_2,
};

const uint32_t MIN_ANON_OUT_SIZE = 1 + 1 + 1 + 33 + 1 + 33; // OP_RETURN ANON_TOKEN lenPk pkTo lenR R [lenEnarr enarr]
const uint32_t MIN_ANON_IN_SIZE = 2 + (33 + 32 + 32); // 2byte marker (cpubkey + sigc + sigr)
const uint32_t MAX_ANON_NARRATION_SIZE = 48;
const uint32_t MIN_RING_SIZE = 10;
const uint32_t MAX_RING_SIZE = 32; // already overkill

const int MIN_ANON_SPEND_DEPTH = 10;
const int ANON_TXN_VERSION = 1000;

// MAX_MONEY = 200000000000000000; most complex possible value can be represented by 36 outputs

int initialiseRingSigs();
int finaliseRingSigs();

int splitAmount(int64_t nValue, std::vector<int64_t> &vOut, int64_t maxAnonOutput = nMaxAnonOutput);

int getOldKeyImage(const CPubKey &pubkey, ec_point &keyImage);

int generateKeyImage(const ec_point &publicKey, ec_secret secret, ec_point &keyImage);

int generateRingSignature(const data_chunk &keyImage, const uint256 &txnHash, int nRingSize, int nSecretOffset, ec_secret secret, const uint8_t *pPubkeys, uint8_t *pSigc, uint8_t *pSigr);
int verifyRingSignature(const data_chunk &keyImage, const uint256 &txnHash, int nRingSize, const uint8_t *pPubkeys, const uint8_t *pSigc, const uint8_t *pSigr);

int generateRingSignatureAB(const data_chunk &keyImage, int nRingSize, int nSecretOffset, ec_secret secret, const uint8_t *pPubkeys, data_chunk &sigC, uint8_t *pSigS);
int verifyRingSignatureAB(const data_chunk &keyImage, int nRingSize, const uint8_t *pPubkeys, const data_chunk &sigC, const uint8_t *pSigS);


#endif  // SPEC_RINGSIG_H

