package=xz
$(package)_version=5.2.4
$(package)_download_path=https://tukaani.org/xz/
$(package)_file_name=xz-$($(package)_version).tar.gz
$(package)_sha256_hash=b512f3b726d3b37b6dc4c8570e137b9311e7552e8ccbab4d39d47ce5f4177145

define $(package)_set_vars
  $(package)_config_opts=--disable-shared --enable-static
  $(package)_config_opts+=--disable-doc --disable-scripts --disable-lzmainfo
  $(package)_config_opts+=--disable-lzma-links --disable-xz --disable-xzdec
  $(package)_config_opts+=--disable-rpath --disable-dependency-tracking
  $(package)_config_opts+=--enable-option-checking
endef

define $(package)_preprocess_cmds
  cp -f $(BASEDIR)/config.guess $(BASEDIR)/config.sub build-aux
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

define $(package)_postprocess_cmds
  rm -f lib/*.la
endef
