// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef CLIENTVERSION_H
#define CLIENTVERSION_H

//
// Client versioning. Hand-maintained for the autotools build path; the
// upstream `clientversion.h.in` template was orphaned when Alias switched
// to CMake. When configure.ac is rewired to consume the .in template,
// delete this file.
//

#define CLIENT_VERSION_MAJOR       4
#define CLIENT_VERSION_MINOR       4
#define CLIENT_VERSION_REVISION    1
#define CLIENT_VERSION_BUILD       0
#define GIT_HASH                   "modernized-main"

// Converts the parameter X to a string after macro replacement on X has been performed.
// Don't merge these into one macro!
#define STRINGIZE(X) DO_STRINGIZE(X)
#define DO_STRINGIZE(X) #X

#endif // CLIENTVERSION_H
