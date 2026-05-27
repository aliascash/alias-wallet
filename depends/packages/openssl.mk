package=openssl
$(package)_version=1.1.1w
$(package)_download_path=https://github.com/openssl/openssl/releases/download/OpenSSL_1_1_1w/
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_sha256_hash=cf3098950cb4d853ad95c0841f1f9c6d3dc102dccfcacd521d93925208b76ac8

define $(package)_set_vars
$(package)_config_env=AR="$($(package)_ar)" RANLIB="$($(package)_ranlib)" CC="$($(package)_cc)"
$(package)_config_opts=--prefix=$(host_prefix) --openssldir=$(host_prefix)/etc/openssl
$(package)_config_opts+=no-shared no-dso no-engine no-tests no-zlib no-dynamic-engine
# no-asm: skip inline assembly. The x86_64-gcc.c bn asm fails to parse
# under the mingw32 cross-compile (token errors). Costs runtime perf
# but is required for a reliable build.
$(package)_config_opts+=no-asm
$(package)_config_opts+=$($(package)_cflags) $($(package)_cppflags)
$(package)_config_opts_linux=-fPIC -Wa,--noexecstack
$(package)_config_opts_x86_64_linux=linux-x86_64
$(package)_config_opts_i686_linux=linux-generic32
$(package)_config_opts_arm_linux=linux-generic32
$(package)_config_opts_aarch64_linux=linux-aarch64
$(package)_config_opts_x86_64_darwin=darwin64-x86_64-cc
$(package)_config_opts_aarch64_darwin=darwin64-arm64-cc
$(package)_config_opts_x86_64_mingw32=mingw64 -static
$(package)_config_opts_i686_mingw32=mingw -static
endef

define $(package)_config_cmds
  ./Configure $($(package)_config_opts)
endef

define $(package)_build_cmds
  $(MAKE) build_libs
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install_dev
endef
