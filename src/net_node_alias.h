// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_NET_NODE_ALIAS_H
#define BITCOIN_NET_NODE_ALIAS_H

#include <net.h>

/**
 * Alias-specific extensions to modern CNode structure
 * Adds fVerified flag for Tor verification
 * 
 * This header provides the interface for adding fVerified to CNode
 * The actual implementation will be integrated into the modern net.h
 */

class CNode;

namespace AliasNode {

/**
 * Get Tor verification status
 * Alias-specific: Returns fVerified flag value
 * 
 * Usage: bool fVerified = AliasNode::IsTorVerified(pnode);
 */
bool IsTorVerified(const CNode* pnode);

/**
 * Set Tor verification status
 * Alias-specific: Sets fVerified flag
 * 
 * Usage: AliasNode::SetTorVerified(pnode, true);
 */
void SetTorVerified(CNode* pnode, bool fVerified);

/**
 * Check if node is connected via Tor
 * Alias-specific: Checks if address is Tor and verified
 */
bool IsTorConnection(const CNode* pnode);

} // namespace AliasNode

/**
 * Instructions for adding fVerified to modern CNode:
 * 
 * In particl-core/src/net.h, add to CNode class:
 * 
 *     // Alias-specific: Tor verification flag
 *     std::atomic_bool fVerified{false};  // tor implementation
 * 
 * Then implement the helper functions in net_node_alias.cpp
 */

#endif // BITCOIN_NET_NODE_ALIAS_H

