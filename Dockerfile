# Build the modernized Alias wallet inside a clean Ubuntu 20.04 container.
#
# Why 20.04: matches Alias's expected stack (Qt 5.12.8, OpenSSL 1.1.1,
# Boost 1.71). Ubuntu 22.04+ ships OpenSSL 3 which breaks Alias's
# OpenSSL 1.1 API usage in ringsig.cpp and others.
#
# Build:   docker build -t alias-modernized -f Dockerfile .
# Extract: docker run --rm -v "$PWD/dist/linux-x86_64:/host-out" alias-modernized sh -c "cp -v /build-out/* /host-out/"
#
# First build takes ~30 min (deps + leveldb + bdb + wallet). Subsequent
# builds reuse cached layers.

FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
ARG JOBS=4

# Layer 1: all build tools and pkg-config-able libraries Alias's
# configure.ac expects. Pinned to Ubuntu 20.04's stock versions which
# happen to match Alias's 5-year-old expected versions.
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        autoconf \
        automake \
        libtool \
        autoconf-archive \
        autotools-dev \
        bsdmainutils \
        pkg-config \
        python3 \
        git \
        ca-certificates \
        curl \
        wget \
        bison \
        flex \
    # Crypto / compression / event
        libssl-dev \
        libevent-dev \
        zlib1g-dev \
        liblz4-dev \
        libzstd-dev \
        liblzma-dev \
        libsnappy-dev \
    # Boost (Ubuntu 20.04 ships 1.71, Alias requires >= 1.62)
        libboost-system-dev \
        libboost-filesystem-dev \
        libboost-program-options-dev \
        libboost-thread-dev \
        libboost-chrono-dev \
        libboost-test-dev \
        libboost-iostreams-dev \
        libboost-date-time-dev \
    # Qt 5.12 (Ubuntu 20.04 stock is 5.12.8)
        qtbase5-dev \
        qttools5-dev \
        qttools5-dev-tools \
        qtdeclarative5-dev \
        qtwebengine5-dev \
        libqt5webchannel5-dev \
        libqt5websockets5-dev \
        libqt5svg5-dev \
        libqt5webview5-dev \
        qml-module-qtwebview \
        qml-module-qtwebengine \
    && rm -rf /var/lib/apt/lists/*

# Layer 1b: additional packages discovered during the iterative build
# (kept as a separate RUN so the big apt install above stays cached
# across iterations).
RUN apt-get update && apt-get install -y --no-install-recommends \
        libcap-dev \
        libseccomp-dev \
    && rm -rf /var/lib/apt/lists/*

# Alias's top-level Makefile.am declares `SUBDIRS = db4.8/build_unix
# leveldb src`, so leveldb and db4.8 must live INSIDE the wallet source
# tree, not as siblings. We clone them in place first (cacheable layers
# unaffected by source changes), build them, then COPY the wallet
# source on top (.dockerignore excludes db4.8 and leveldb so the COPY
# doesn't disturb them).
WORKDIR /build/alias-modernized

# Layer 2: leveldb (aliascash fork)
RUN git clone --depth 1 https://github.com/aliascash/leveldb.git

# Layer 3: db4.8 (aliascash fork)
RUN git clone --depth 1 https://github.com/aliascash/db4.8.git

# Layer 4: Build leveldb. Default `all` target produces
# out-static/libleveldb.a and out-static/libmemenv.a, which is exactly
# what src/Makefile.am references via ../leveldb/out-static/.
WORKDIR /build/alias-modernized/leveldb
RUN make -j${JOBS}

# Layer 5: Build db4.8. Produces libdb_cxx-4.8.a; src/Makefile.am
# expects libdb_cxx.a, so symlink it.
WORKDIR /build/alias-modernized/db4.8/build_unix
RUN ../dist/configure \
        --disable-shared \
        --enable-static \
        --enable-cxx \
        --disable-replication \
        --disable-compression \
        --with-pic \
    && make -j${JOBS} libdb_cxx-4.8.a \
    && ln -sf libdb_cxx-4.8.a libdb_cxx.a

# Layer 6: Copy the modernized wallet source ON TOP. This is the layer
# that invalidates on every source code change; everything above is
# cached. The .dockerignore excludes leveldb/ and db4.8/ so this COPY
# does not disturb the freshly built libraries above.
WORKDIR /build/alias-modernized
COPY . .

# Layer 7: Configure and build.
RUN ./autogen.sh
RUN ./configure --enable-gui --with-boost-libdir=/usr/lib/x86_64-linux-gnu
RUN make -j${JOBS}

# Final layer: stash the build outputs at a stable path for extraction.
RUN mkdir -p /build-out \
    && cp -v src/aliaswalletd /build-out/ 2>/dev/null || true \
    && cp -v src/aliaswallet /build-out/ 2>/dev/null || true

# Default command lists artifacts.
CMD ["ls", "-la", "/build-out"]
