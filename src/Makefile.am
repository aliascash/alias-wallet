# SPDX-FileCopyrightText: © 2020 Alias Developers
# SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
#
# SPDX-License-Identifier: MIT

AM_LDFLAGS = -fstack-protector

if OS_LINUX
AM_LDFLAGS += -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,-z,noexecheap -Wl,-z,nodlopen -Wl,-z,nodump
else
if OS_WINDOWS_CROSSCOMPILE
# Default to console subsystem so the daemon's stdout/stderr work in
# cmd.exe. The Qt GUI binary overrides to --subsystem,windows below.
AM_LDFLAGS += -Wl,--dynamicbase -Wl,--nxcompat -Wl,--subsystem,console
else
if OS_MACOS
AM_LDFLAGS += $(MACOS_RPATH)
endif
endif
endif

AM_CPPFLAGS = -pthread -fPIC -fstack-protector -O2 \
	      -D_FORTIFY_SOURCE=1 \
	      -Wall -Wextra -Wno-ignored-qualifiers -Woverloaded-virtual \
	      -Wformat -Wformat-security -Wno-unused-parameter

if OS_MACOS
AM_CPPFLAGS += -std=c++17 -mmacosx-version-min=10.10
endif

common_SOURCES = anon/anonymize.cpp \
		 json/json_spirit_reader.cpp \
		 json/json_spirit_writer.cpp \
		 alert.cpp \
		 util/version.cpp \
		 checkpoints.cpp \
		 net/netbase.cpp \
		 net/addrman.cpp \
		 wallet/crypter.cpp \
		 key/key.cpp \
		 key/eckey.cpp \
		 key/extkey.cpp \
		 wallet/db.cpp \
		 init/init.cpp \
		 key/keystore.cpp \
		 pos/miner.cpp \
		 main.cpp \
		 net/net.cpp \
		 net/protocol.cpp \
		 rpc/rpcprotocol.cpp \
		 rpc/rpcserver.cpp \
		 rpc/rpcclient.cpp \
		 rpc/rpcdump.cpp \
		 rpc/rpcnet.cpp \
		 rpc/rpcmining.cpp \
		 rpc/rpcwallet.cpp \
		 rpc/rpcblockchain.cpp \
		 rpc/rpcrawtransaction.cpp \
		 rpc/rpcextkey.cpp \
		 rpc/rpcmnemonic.cpp \
		 script/script.cpp \
		 util/sync.cpp \
		 util/util.cpp \
		 crypto/hash.cpp \
		 wallet/wallet.cpp \
		 wallet/walletdb.cpp \
		 pos/kernel.cpp \
		 crypto/pbkdf2.cpp \
		 crypto/scrypt.cpp \
		 crypto/scrypt-arm.S \
		 crypto/scrypt-x86.S \
		 crypto/scrypt-x86_64.S \
		 smsg/smessage.cpp \
		 anon/stealth.cpp \
		 anon/ringsig.cpp \
		 primitives/core.cpp \
		 wallet/txdb-leveldb.cpp \
		 wallet/txmempool.cpp \
		 kernel/chainparams.cpp \
		 state.cpp \
		 net/bloom.cpp \
		 shutdown.cpp

bin_PROGRAMS = aliaswalletd
aliaswalletd_SOURCES = $(common_SOURCES) \
		       spectrecoind.cpp \
		       noui.cpp

aliaswalletd_LDADD = ../leveldb/out-static/libmemenv.a \
		     ../leveldb/out-static/libleveldb.a \
		     ../db4.8/build_unix/libdb_cxx.a \
		     $(OPENSSL_LIBS) \
		     $(LIBEVENT_LIBS) \
		     $(LIBSECCOMP_LIBS) \
		     $(LIBCAP_LIBS) \
		     $(ZLIB_LIBS) \
		     $(ZSTD_LIBS) \
		     $(LZMA_LIBS) \
		     $(BOOST_SYSTEM_LIBS) \
		     $(BOOST_CHRONO_LIBS) \
		     $(BOOST_FILESYSTEM_LIBS) \
		     $(BOOST_PROGRAM_OPTIONS_LIBS) \
		     $(BOOST_THREAD_LIBS)

aliaswalletd_CPPFLAGS = $(AM_CPPFLAGS) $(OPENSSL_CFLAGS) $(LIBEVENT_CFLAGS) \
			$(LIBSECCOMP_CFLAGS) $(LIBCAP_CFLAGS) $(ZLIB_CFLAGS) $(ZSTD_CPPFLAGS) $(LZMA_CPPFLAGS) \
			$(BOOST_CPPFLAGS) \
			-I../leveldb/include -I../leveldb/helpers -I../db4.8/build_unix \
			-D_FORTIFY_SOURCE=1 -DBOOST_THREAD_USE_LIB -DBOOST_SPIRIT_THREADSAFE \
			-DBOOST_ASIO_ENABLE_OLD_SERVICES
aliaswalletd_CFLAGS = $(aliaswalletd_CPPFLAGS) -std=c11
aliaswalletd_LDFLAGS = $(AM_LDFLAGS) $(OPENSSL_LDFLAGS) $(LIBEVENT_LDFLAGS) \
		       $(LIBSECCOMP_LDFLAGS) $(LIBCAP_LDFLAGS) $(ZLIB_LDFLAGS) $(ZSTD_LDFLAGS) $(LZMA_LDFLAGS) \
		       $(BOOST_SYSTEM_LDFLAGS) $(BOOST_CHRONO_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) \
		       $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_THREAD_LDFLAGS)
aliaswalletd_LIBTOOLFLAGS = --tag CXX

if OS_WINDOWS_CROSSCOMPILE
aliaswalletd_LDADD += -lmswsock -lshlwapi -luuid -lole32 -lgdi32
aliaswalletd_CPPFLAGS += -DWIN32 -D_WIN32
# Force a fully-static .exe (no libwinpthread-1.dll runtime dep).
# -all-static is a libtool flag; only libtool sees it here.
aliaswalletd_LDFLAGS += -all-static
else
if OS_MACOS
aliaswalletd_CPPFLAGS += -DMAC_OSX
else
aliaswalletd_LDFLAGS += -pie
aliaswalletd_LDADD += -lcap -lseccomp
endif
endif

if ENABLE_GUI
qt/locale/%.qm: qt/locale/%.ts
		@LRELEASE@ $< -qm $@

qt/qrc_%.cpp: ../%.qrc
		@RCC@ $< -o $@

if OS_MACOS
  MOC_CPPFLAGS = -DQ_OS_MACOS
endif

qt/moc_%.cpp: qt/%.h
		@MOC@ -o $@ $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(MOC_CPPFLAGS) $<

qt/ui_%.h: qt/forms/%.ui
		@UIC@ -o $@ $<

BUILT_SOURCES = qt/ui_aboutdialog.h \
		qt/ui_askpassphrasedialog.h \
		qt/ui_coincontroldialog.h \
		qt/ui_editaddressdialog.h \
		qt/ui_rpcconsole.h \
		qt/ui_transactiondescdialog.h \
		qt/locale/alias_af_ZA.qm \
		qt/locale/alias_ar.qm \
		qt/locale/alias_be.qm \
		qt/locale/alias_bg.qm \
		qt/locale/alias_bs.qm \
		qt/locale/alias_ca_ES.qm \
		qt/locale/alias_ca.qm \
		qt/locale/alias_cs.qm \
		qt/locale/alias_cy.qm \
		qt/locale/alias_da.qm \
		qt/locale/alias_de.qm \
		qt/locale/alias_el_GR.qm \
		qt/locale/alias_el.qm \
		qt/locale/alias_en.qm \
		qt/locale/alias_eo.qm \
		qt/locale/alias_es_MX.qm \
		qt/locale/alias_es.qm \
		qt/locale/alias_et.qm \
		qt/locale/alias_eu_ES.qm \
		qt/locale/alias_fa_IR.qm \
		qt/locale/alias_fa.qm \
		qt/locale/alias_fi.qm \
		qt/locale/alias_fr_CA.qm \
		qt/locale/alias_fr.qm \
		qt/locale/alias_gl.qm \
		qt/locale/alias_he.qm \
		qt/locale/alias_hi_IN.qm \
		qt/locale/alias_hr.qm \
		qt/locale/alias_hu.qm \
		qt/locale/alias_id_ID.qm \
		qt/locale/alias_it.qm \
		qt/locale/alias_ja.qm \
		qt/locale/alias_ka.qm \
		qt/locale/alias_kk.qm \
		qt/locale/alias_ko_KR.qm \
		qt/locale/alias_ky.qm \
		qt/locale/alias_la.qm \
		qt/locale/alias_lt.qm \
		qt/locale/alias_lv_LV.qm \
		qt/locale/alias_ms_MY.qm \
		qt/locale/alias_nb.qm \
		qt/locale/alias_nl.qm \
		qt/locale/alias_pl.qm \
		qt/locale/alias_pt_BR.qm \
		qt/locale/alias_pt_PT.qm \
		qt/locale/alias_ro_RO.qm \
		qt/locale/alias_ru.qm \
		qt/locale/alias_sk.qm \
		qt/locale/alias_sl_SI.qm \
		qt/locale/alias_sq.qm \
		qt/locale/alias_sr.qm \
		qt/locale/alias_sv.qm \
		qt/locale/alias_th.qm \
		qt/locale/alias_tr.qm \
		qt/locale/alias_uk.qm \
		qt/locale/alias_ur_PK.qm \
		qt/locale/alias_vi.qm \
		qt/locale/alias_vi_VN.qm \
		qt/locale/alias_zh_CN.qm \
		qt/locale/alias_zh_HK.qm \
		qt/locale/alias_zh_TW.qm

bin_PROGRAMS += aliaswallet

aliaswallet_SOURCES = $(common_SOURCES) \
		  qt/qrc_spectre.cpp \
		  qt/moc_spectregui.cpp \
		  qt/moc_spectrebridge.cpp \
		  qt/moc_rpcconsole.cpp \
		  qt/moc_peertablemodel.cpp \
		  qt/moc_notificator.cpp \
		  qt/moc_editaddressdialog.cpp \
		  qt/moc_askpassphrasedialog.cpp \
		  qt/moc_coincontroldialog.cpp \
		  qt/moc_coincontroltreewidget.cpp \
		  qt/moc_addresstablemodel.cpp \
		  qt/moc_transactiontablemodel.cpp \
		  qt/moc_transactiondesc.cpp \
		  qt/moc_trafficgraphwidget.cpp \
		  qt/moc_paymentserver.cpp \
		  qt/moc_clientmodel.cpp \
		  qt/moc_walletmodel.cpp \
		  qt/moc_optionsmodel.cpp \
		  qt/moc_guiutil.cpp \
		  qt/moc_aboutdialog.cpp \
		  qt/moc_csvmodelwriter.cpp \
		  qt/moc_qvaluecombobox.cpp \
		  qt/moc_qvalidatedlineedit.cpp \
		  qt/moc_bitcoinaddressvalidator.cpp \
		  qt/moc_monitoreddatamapper.cpp \
		  qt/moc_bitcoinamountfield.cpp \
		  qt/moc_setupwalletwizard.cpp \
                  qt/moc_websocketclientwrapper.cpp \
                  qt/moc_websockettransport.cpp \
		  qt/transactiontablemodel.cpp \
		  qt/coincontroldialog.cpp \
		  qt/coincontroltreewidget.cpp \
		  qt/aboutdialog.cpp \
		  qt/addresstablemodel.cpp \
		  qt/editaddressdialog.cpp \
		  qt/bitcoinaddressvalidator.cpp \
		  qt/clientmodel.cpp \
		  qt/guiutil.cpp \
		  qt/transactionrecord.cpp \
		  qt/optionsmodel.cpp \
		  qt/monitoreddatamapper.cpp \
		  qt/transactiondesc.cpp \
		  qt/bitcoinamountfield.cpp \
		  qt/walletmodel.cpp \
		  qt/csvmodelwriter.cpp \
		  qt/qvalidatedlineedit.cpp \
		  qt/bitcoinunits.cpp \
		  qt/qvaluecombobox.cpp \
		  qt/askpassphrasedialog.cpp \
		  qt/notificator.cpp \
		  qt/rpcconsole.cpp \
		  qt/paymentserver.cpp \
		  qt/peertablemodel.cpp \
		  qt/scicon.cpp \
		  qt/trafficgraphwidget.cpp \
		  qt/spectregui.cpp \
		  qt/spectre.cpp \
		  qt/spectrebridge.cpp \
		  qt/winshutdownmonitor.cpp \
		  qt/setupwalletwizard.cpp \
		  qt/websocketclientwrapper.cpp \
		  qt/websockettransport.cpp

aliaswallet_LDADD = $(aliaswalletd_LDADD) $(QT5_LIBS) $(QT5WEBKIT_LIBS)
aliaswallet_CPPFLAGS = $(aliaswalletd_CPPFLAGS) $(QT5_CFLAGS) $(QT5WEBKIT_CFLAGS) -DQT_DISABLE_DEPRECATED_BEFORE=0
aliaswallet_LDFLAGS = $(aliaswalletd_LDFLAGS) $(QT5_LDFLAGS) $(QT5WEBKIT_LDFLAGS)
aliaswallet_LIBTOOLFLAGS = $(aliaswalletd_LIBTOOLFLAGS)

if OS_MACOS
  aliaswallet_SOURCES += qt/moc_macnotificationhandler.cpp \
		     qt/macnotificationhandler.mm \
		     qt/moc_macdockiconhandler.cpp \
		     qt/macdockiconhandler.mm
  aliaswallet_LDFLAGS += -framework Foundation -framework AppKit -framework CoreServices
endif
endif

clean-local:
	rm -f qt/moc_*.cpp
	rm -f qt/ui_*.cpp
	rm -f qt/qrc_*.cpp
