// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_NET_ALIAS_H
#define BITCOIN_NET_ALIAS_H

#include <net.h>
#include <netaddress.h>

/**
 * Alias-specific networking extensions
 * Preserves Tor integration and fVerified flag
 */

/**
 * Check if address is a Tor V3 address
 * Alias-specific: Tor V3 address validation
 */
bool IsTorV3Address(const CNetAddr& addr);

/**
 * Start embedded Tor service
 * Alias-specific: Embedded Tor integration
 * This runs Tor in a separate thread (THREAD_TORNET)
 */
void StartTor(void* parg);

/**
 * Thread function for Tor network
 * Alias-specific: Tor network thread
 */
void ThreadTorNet(void* parg);

/**
 * Thread function for onion seed
 * Alias-specific: Onion address seeding thread
 */
void ThreadOnionSeed(void* parg);

/**
 * Add fVerified flag support to modern CNode
 * Alias-specific: Tor verification flag
 * This should be added as a member to CNode or Peer structure
 */
namespace AliasNet {
    // Helper to check if a node is Tor-verified
    // This will be integrated into the modern CNode structure
    bool IsTorVerified(const CNode* pnode);
    
    // Set Tor verification status
    // This will be integrated into the modern CNode structure
    void SetTorVerified(CNode* pnode, bool fVerified);
}

#endif // BITCOIN_NET_ALIAS_H

