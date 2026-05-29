# Create the super cache so modules will add themselves to it.
cache(, super)

!QTDIR_build: cache(CONFIG, add, $$list(QTDIR_build))

prl = no_install_prl
CONFIG += $$prl
cache(CONFIG, add stash, prl)

TEMPLATE = subdirs
SUBDIRS = qtbase qttools qttranslations qtsvg qtdeclarative qtwebchannel qtwebsockets

qttools.depends = qtbase
qttranslations.depends = qttools
qtsvg.depends = qtbase
qtdeclarative.depends = qtbase qtsvg
qtwebchannel.depends = qtbase qtdeclarative
qtwebsockets.depends = qtbase qtdeclarative

load(qt_configure)
