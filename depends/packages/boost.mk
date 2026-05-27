package=boost
$(package)_version=1.81.0
$(package)_download_path=https://archives.boost.io/release/$($(package)_version)/source/
$(package)_file_name=boost_$(subst .,_,$($(package)_version)).tar.gz
$(package)_sha256_hash=205666dea9f6a7cfed87c7a6dfbeb52a2c1b9de55712c9c1a87735d7181452b6

# Alias requires actual Boost libraries (System, Filesystem, Thread,
# Program_Options, Chrono, Iostreams, Date_Time). Upstream Particl
# only stages headers; we build the libs from source.

$(package)_libraries=system filesystem thread program_options chrono iostreams date_time
$(package)_libs_with=$(foreach lib,$($(package)_libraries),--with-$(lib))
$(package)_libs_csv=system,filesystem,thread,program_options,chrono,iostreams,date_time

define $(package)_set_vars
  $(package)_config_opts_release=variant=release
  $(package)_config_opts_debug=variant=debug
  $(package)_config_opts=--layout=system --user-config=user-config.jam
  $(package)_config_opts+=threading=multi link=static -sNO_BZIP2=1 -sNO_ZLIB=1 -sNO_ZSTD=1 -sNO_LZMA=1
  $(package)_config_opts+=$($(package)_libs_with)
  $(package)_config_opts_linux=runtime-link=shared target-os=linux
  $(package)_config_opts_darwin=runtime-link=shared target-os=darwin
  $(package)_config_opts_mingw32=runtime-link=static target-os=windows address-model=64 binary-format=pe abi=ms
  $(package)_toolset_$(host_os)=gcc
endef

define $(package)_preprocess_cmds
  echo "using $($(package)_toolset_$(host_os)) : : $($(package)_cxx) : <cxxflags>\"$($(package)_cxxflags) $($(package)_cppflags)\" <linkflags>\"$($(package)_ldflags)\" <archiver>\"$($(package)_ar)\" <ranlib>\"$($(package)_ranlib)\" ;" > user-config.jam
endef

define $(package)_config_cmds
  ./bootstrap.sh --without-icu --with-libraries=$($(package)_libs_csv)
endef

define $(package)_build_cmds
  ./b2 -d2 -j$(JOBS) --prefix=$($(package)_staging_prefix_dir) $($(package)_config_opts) stage
endef

define $(package)_stage_cmds
  ./b2 -d0 -j$(JOBS) --prefix=$($(package)_staging_prefix_dir) $($(package)_config_opts) install
endef
