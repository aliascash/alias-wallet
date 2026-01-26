// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
// SPDX-FileCopyrightText: © 2014 ShadowCoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef SPECTREGUI_H
#define SPECTREGUI_H

#include <QMainWindow>
#include <QtWebView>
#include <QWebChannel>
#include <QSystemTrayIcon>
#include <QLabel>
#include <QModelIndex>
#include <QSplashScreen>

#include "spectrebridge.h"
#include "walletmodel.h"
#include "askpassphrasedialog.h"
#include "rpcconsole.h"

#include <stdint.h>

class TransactionTableModel;
class ClientModel;
class WalletModel;
class MessageModel;
class Notificator;

QT_BEGIN_NAMESPACE
class QLabel;
class QMenuBar;
class QToolBar;
class QUrl;
QT_END_NAMESPACE

static const int WEBSOCKETPORT = 52471;
static const int WEBSOCKETPORT_TESTNET = 52472;

class SpectreGUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit SpectreGUI(QWebChannel *webChannel, QWidget *parent = nullptr);
    ~SpectreGUI();

    void setClientModel(ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);
    void setMessageModel(MessageModel *messageModel);
    void setSplashScreen(QSplashScreen* splash);
    void loadIndex(QString webSocketToken);
    void runJavaScript(QString javascriptCode);

protected:
    void changeEvent(QEvent *e) override;
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    SpectreBridge *bridge{nullptr};
    QWebChannel *webChannel{nullptr};
    QObject *qmlWebView{nullptr};
    bool uiReady{false};

    ClientModel *clientModel{nullptr};
    WalletModel *walletModel{nullptr};
    MessageModel *messageModel{nullptr};

    QMenuBar *appMenuBar{nullptr};
    QSplashScreen *splashScreen{nullptr};

    QAction *quitAction{nullptr};
    QAction *aboutAction{nullptr};
    QAction *optionsAction{nullptr};
    QAction *toggleHideAction{nullptr};
    QAction *exportAction{nullptr};
    QAction *encryptWalletAction{nullptr};
    QAction *backupWalletAction{nullptr};
    QAction *changePassphraseAction{nullptr};
    QAction *unlockWalletAction{nullptr};
    QAction *lockWalletAction{nullptr};
    QAction *aboutQtAction{nullptr};
    QAction *openRPCConsoleAction{nullptr};

    QSystemTrayIcon *trayIcon{nullptr};
    Notificator *notificator{nullptr};
    RPCConsole *rpcConsole{nullptr};
    QTimer *pollTimer{nullptr};

    uint64_t nWeight{0};
    bool fConnectionInit{true};

    void createActions();
    void createMenuBar();
    void createTrayIcon();
    void pageLoaded(bool ok);
    bool initialized{false};

    friend class SpectreBridge;

    class WebElement {
    public:
        enum SelectorType {ID, CLASS};
        WebElement(SpectreGUI* spectreGUI, QString name, SelectorType type = SelectorType::ID);
        void setAttribute(QString attribute, QString value);
        void removeAttribute(QString attribute);
        void addClass(QString className);
        void removeClass(QString className);
        void setContent(QString value);
    private:
        SpectreGUI* spectreGUI;
        QString name;
        QString getElementJS;
    };

private Q_SLOTS:
    void addJavascriptObjects(const QString &id, QObject *object);
    void urlClicked(const QUrl & link);
    void setNumConnections(int count);
    void setNumBlocks(int count, int nTotalBlocks);
    void setEncryptionStatus(int status);
    void error(const QString &title, const QString &message, bool modal);
    void askFee(qint64 nFeeRequired, bool *payFee);
    void handleURI(QString strURI);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void incomingTransaction(const QModelIndex & parent, int start, int end);
    void optionsClicked();
    void aboutClicked();
    void unlockWallet(WalletModel::UnlockMode unlockMode = WalletModel::UnlockMode::standard);
    void lockWallet();
    void toggleLock();
    void encryptWallet(bool status);
    void backupWallet();
    void changePassphrase();
    void showNormalIfMinimized(bool fToggleHidden = false);
    void toggleHidden();
    void updateWeight();
    void updateStakingIcon();
    void detectShutdown();
    void requestShutdown();
};

#endif
