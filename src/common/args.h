// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// Copyright (c) 2023 The Bitcoin Core developers
// SPDX-License-Identifier: MIT

#ifndef ALIAS_COMMON_ARGS_H
#define ALIAS_COMMON_ARGS_H

#include <util/fs.h>

#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>
#include <mutex>

class ArgsManager {
public:
    enum Flags : uint32_t {
        ALLOW_ANY = 0x01,
        NETWORK_ONLY = 0x02,
        SENSITIVE = 0x04,
        COMMAND = 0x08,
        DEBUG_ONLY = 0x10,
        HIDDEN = 0x20,
        DISALLOW_NEGATION = 0x40,
        DISALLOW_ELISION = 0x80,
    };

    ArgsManager();
    ~ArgsManager();

    void SelectConfigNetwork(const std::string& network);
    bool ParseParameters(int argc, const char* const argv[], std::string& error);
    bool ReadConfigFiles(std::string& error, bool ignore_invalid_keys = false);
    
    std::set<std::string> GetUnsuitableSectionOnlyArgs() const;
    std::vector<std::string> GetUnrecognizedSections() const;

    const fs::path& GetBlocksDirPath() const;
    const fs::path& GetDataDirBase() const;
    const fs::path& GetDataDirNet() const;
    void ClearPathCache();

    std::vector<std::string> GetArgs(const std::string& arg) const;
    bool IsArgSet(const std::string& arg) const;
    bool IsArgNegated(const std::string& arg) const;
    std::string GetArg(const std::string& arg, const std::string& default_val) const;
    std::optional<std::string> GetArg(const std::string& arg) const;
    fs::path GetPathArg(const std::string& arg, const fs::path& default_val = {}) const;
    int64_t GetIntArg(const std::string& arg, int64_t default_val) const;
    std::optional<int64_t> GetIntArg(const std::string& arg) const;
    bool GetBoolArg(const std::string& arg, bool default_val) const;
    std::optional<bool> GetBoolArg(const std::string& arg) const;

    bool SoftSetArg(const std::string& arg, const std::string& value);
    bool SoftSetBoolArg(const std::string& arg, bool value);
    void ForceSetArg(const std::string& arg, const std::string& value);

    std::string GetChainTypeString() const;

    void AddArg(const std::string& name, const std::string& help, unsigned int flags, const std::string& category);
    void AddHiddenArgs(const std::vector<std::string>& args);
    void ClearArgs();

    std::string GetHelpMessage() const;

    void LogArgs() const;

protected:
    struct Arg {
        std::string m_help_text;
        unsigned int m_flags;
    };

    mutable std::mutex cs_args;
    std::map<std::string, std::vector<std::string>> m_settings;
    std::map<std::string, std::vector<std::string>> m_config_args;
    std::string m_network;
    std::set<std::string> m_network_only_args;
    std::map<std::string, Arg> m_available_args;
    bool m_accept_any_command{true};
    std::list<std::string> m_command;

    mutable fs::path m_cached_blocks_path;
    mutable fs::path m_cached_datadir_path;
    mutable fs::path m_cached_network_datadir_path;

    [[nodiscard]] bool ReadConfigStream(std::istream& stream, const std::string& filepath, std::string& error, bool ignore_invalid_keys = false);

    std::optional<std::string> GetArgHelper(const std::string& arg) const;
    std::vector<std::string> GetArgsHelper(const std::string& arg) const;
};

extern ArgsManager gArgs;

bool HelpRequested(const ArgsManager& args);
void SetupHelpOptions(ArgsManager& args);

std::string HelpMessageGroup(const std::string& message);
std::string HelpMessageOpt(const std::string& option, const std::string& message);

void PrintExceptionContinue(const std::exception* pex, const std::string& thread_name);

fs::path AbsPathForConfigVal(const ArgsManager& args, const fs::path& path, bool net_specific = true);

#endif // ALIAS_COMMON_ARGS_H
