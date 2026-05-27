package=tor
$(package)_version=0.4.1.6
$(package)_download_path=https://dist.torproject.org/
$(package)_file_name=tor-$($(package)_version).tar.gz
$(package)_sha256_hash=ee7adbbc5e30898bc35d9658bbf6a67e4242977175f7bad11c5f1ee0c1010d43
$(package)_dependencies=libevent xz zlib
$(package)_patches=tor-001-disable-deprecated-android-log.patch tor-002-fix-openssl-checks.patch

# Tor still requires OpenSSL at build time. Bitcoin Core / Particl depends/ no
# longer ships an openssl recipe, so until an openssl.mk is added to depends/
# Tor will pick up the host system's OpenSSL via --with-openssl-dir.
# Override TOR_OPENSSL_DIR at make time if needed:
#   make -C depends NO_QT=1 tor TOR_OPENSSL_DIR=/path/to/openssl
TOR_OPENSSL_DIR ?= /usr

define $(package)_set_vars
  $(package)_config_opts=--prefix=$(host_prefix)
  $(package)_config_opts+=--enable-lzma
  $(package)_config_opts+=--enable-pic
  $(package)_config_opts+=--enable-static-tor
  $(package)_config_opts+=--enable-restart-debugging
  $(package)_config_opts+=--with-libevent-dir=$(host_prefix)
  $(package)_config_opts+=--with-openssl-dir=$(TOR_OPENSSL_DIR)
  $(package)_config_opts+=--disable-zstd
  $(package)_config_opts+=--disable-module-dirauth
  $(package)_config_opts+=--disable-tool-name-check
  $(package)_config_opts+=--disable-asciidoc
  $(package)_config_opts+=--disable-manpage
  $(package)_config_opts+=--disable-html-manual
endef

define $(package)_preprocess_cmds
  patch -p1 --forward -r - < $($(package)_patch_dir)/tor-001-disable-deprecated-android-log.patch || true && \
  patch -p1 --forward -r - < $($(package)_patch_dir)/tor-002-fix-openssl-checks.patch || true && \
  ./autogen.sh
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef
