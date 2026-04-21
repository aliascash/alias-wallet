// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/context.h>

#include <util.h>  // For LogPrintf
#include <crypto/sha256.h>  // May not exist in old codebase, will add if needed
#include <random.h>  // May not exist in old codebase, will add if needed

#include <string>

namespace kernel {
Context::Context()
{
    // TODO: Implement SHA256AutoDetect and RandomInit when crypto/random modules are migrated
    // For now, this is a placeholder that allows compilation
    // std::string sha256_algo = SHA256AutoDetect();
    // LogPrintf("Using the '%s' SHA256 implementation\n", sha256_algo);
    // RandomInit();
    LogPrintf("Kernel context initialized\n");
}
} // namespace kernel

