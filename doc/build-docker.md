# Building the modernized Alias wallet in Docker

The cleanest way to compile the modernized tree without fighting your
system's dependency versions. Docker provides Ubuntu 20.04 which ships
exactly the stack Alias was built against in 2020.

## Prerequisites

  - Docker Desktop installed and running
    ([Windows](https://www.docker.com/products/docker-desktop/),
    [Linux](https://docs.docker.com/engine/install/),
    [macOS](https://docs.docker.com/desktop/install/mac-install/))
  - About 4 GB free disk space for the image
  - First build: ~30 minutes. Subsequent builds: a few minutes (Docker
    caches the apt + leveldb + db4.8 layers).

## One-shot build

From the repository root:

```sh
# Linux / macOS / WSL
docker build -t alias-modernized -f Dockerfile .
mkdir -p out
docker run --rm -v "$PWD/out:/host-out" alias-modernized \
    sh -c "cp -v /build-out/* /host-out/"
ls -la out/
```

```powershell
# Windows PowerShell
pwsh -File scripts/build-in-docker.ps1
```

Output binaries land in `./out/`:
  - `aliaswalletd` — the daemon
  - `aliaswallet` — the Qt GUI wallet

## What the Dockerfile actually does

1. Starts from `ubuntu:20.04`.
2. `apt install`s the exact build chain Alias needs: gcc, autoconf,
   pkg-config, libssl-dev (1.1.1), libevent-dev, Boost 1.71, Qt 5.12.8,
   libsnappy, libzstd, liblzma.
3. Clones `aliascash/leveldb` and `aliascash/db4.8` and builds them in
   the layout Alias's `src/Makefile.am` expects (sibling directories
   to the wallet source).
4. Copies the modernized source into `/build/alias-modernized` (the
   `.dockerignore` excludes the `depends/`, `external.deprecated/`,
   `test/` directories which aren't needed inside the container).
5. Runs `./autogen.sh && ./configure --enable-gui && make`.

## What's expected to go wrong on first build

The Phase 2 reorganization moved 74 files across 15 subdirs and
rewrote 356 `#include` paths. Verification has only been at the grep
level — nothing has been compiled yet. The first build is likely to
hit one of:

  - A transitive include that wasn't surfaced because the source still
    pulled it in via another header. Surfaced by GCC's
    `error: 'X' was not declared in this scope`.
  - A Makefile.am path that wasn't updated (only `.cpp` paths were
    updated; if any header is explicitly listed there for `BUILT_SOURCES`
    or `EXTRA_DIST` it may still reference the old location).
  - Qt MOC failing to find a header for `qt/moc_*.cpp` generation
    because of the include reorg.
  - `db4.8` upstream patches not applying on the version of GCC that
    Ubuntu 20.04 ships.

These are paper cuts that need fix-iterate. Capture the failing
command + error and we'll patch it.

## Reproducibility

The Dockerfile clones `aliascash/leveldb` and `aliascash/db4.8` at
`HEAD` of `master` rather than pinning to specific commits. For a
fully reproducible build, replace the `git clone --depth 1` lines with
explicit SHAs once the build is verified working.

## Cross-compiling to Windows (alias.exe)

Not implemented yet. The path is: in the Dockerfile, add the
`g++-mingw-w64-x86-64` apt package, then build `depends/` for
`HOST=x86_64-w64-mingw32` (which produces all dependencies as
MinGW-cross static libraries), and configure with the mingw triplet.
This is on the Phase 1 wiring follow-up.

For now, the produced Linux ELF binaries run inside the container; to
get a `.exe` you currently still need Alias's existing windows builder
infrastructure under [external.deprecated/](../external.deprecated/) —
those scripts cross-compile via the same mingw toolchain.
