// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <net_node_alias.h>
#include <net.h>
#include <netaddress.h>
#include <net_alias.h>
#include <logging.h>

namespace AliasNode {

bool IsTorVerified(const CNode* pnode)
{
    if (!pnode)
        return false;
    
    // Access fVerified flag from CNode (already exists in alias-wallet)
    return pnode->fVerified;
}

void SetTorVerified(CNode* pnode, bool fVerified)
{
    if (!pnode)
        return;
    
    // Set fVerified flag on CNode (already exists in alias-wallet)
    pnode->fVerified = fVerified;
    
    if (fVerified && pnode->addr.IsTor()) {
        LogPrintf("Marking Tor connection as verified: %s\n", 
                 pnode->addr.ToString().c_str());
    }
}

bool IsTorConnection(const CNode* pnode)
{
    if (!pnode)
        return false;
    
    // Check if node is connected via Tor
    bool isTor = pnode->addr.IsTor();
    bool isVerified = IsTorVerified(pnode);
    
    return isTor && isVerified;
}

} // namespace AliasNode

