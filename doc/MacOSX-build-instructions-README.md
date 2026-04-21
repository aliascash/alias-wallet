# Aliaswallet Building from Source for macOS

First, you need to clone the Git repository.

## Build with Qt Creator

### Install Qt SDK 5.12.2 (QtWebEngine)

- **Qt SDK**: [Download Qt Installer](https://www.qt.io/download-qt-installer)

Now you can open `<path-to-your-alias-wallet-git-repo-clone>/src/src.pro` in Qt Creator.

## Build on Command Line

### Export Path to Qt

```bash
export QT_PATH=~/Qt/5.12.2/clang_64
```

### Setup Required Libraries and Environment Variables

#### Boost

1. Determine the number of available cores to improve build speed:

```bash
system_profiler | grep "Total Number of Cores"
export CORES=<insert-value-from-above-cmd>
```

2. Download, extract and build Boost:

```bash
cd ~
mkdir Boost
cd Boost
wget https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.gz
tar xzf boost_1_68_0.tar.gz
cd boost_1_68_0
./bootstrap.sh
./b2 \
    cxxflags="-std=c++0x" \
    address-model=64 \
    -j ${CORES} \
    install \
    --prefix=$(pwd) \
    --build-type=complete \
    --layout=tagged
```

3. Export path to Boost libraries and headers:

```bash
export BOOST_PATH=$(pwd)
```

#### OpenSSL

1. Install OpenSSL 1.1.1
2. Export path to OpenSSL libraries and headers (example):

```bash
export OPENSSL_PATH=/usr/local/Cellar/openssl@1.1/1.1.1d
```

#### Tor

Download prepared Tor archive:

```bash
cd ~
mkdir Tor
cd Tor
wget https://github.com/aliascash/resources/raw/master/resources/Aliaswallet.Tor.libraries.macOS.zip
```

### Build Using Helper Scripts

```bash
cd <path-to-your-alias-wallet-git-repo-clone>
./scripts/mac-build.sh
rm -f Aliaswallet*.dmg
unzip ~/Tor/Tor.zip
# rm -rf src/bin/debug
./scripts/mac-deployqt.sh
```
