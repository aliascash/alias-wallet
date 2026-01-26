// SPDX-FileCopyrightText: © 2025 ALIAS Developers
// SPDX-FileCopyrightText: © 2020 Alias Developers
// SPDX-FileCopyrightText: © 2016 SpectreCoin Developers
//
// SPDX-License-Identifier: MIT

#ifndef SETUPWALLETWIZARD_H
#define SETUPWALLETWIZARD_H

#include <QWizard>

#include <vector>
#include <string>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QProgressBar;
class QPushButton;
QT_END_NAMESPACE

class SetupWalletWizard : public QWizard
{
    Q_OBJECT

public:
    enum { Page_Intro,
           Page_ImportWalletDat,
           Page_NewMnemonic_Settings, Page_NewMnemonic_Result, Page_NewMnemonic_Verification,
           Page_RecoverFromMnemonic,
           Page_EncryptWallet};

    SetupWalletWizard(QWidget *parent = nullptr);

private Q_SLOTS:
    void showHelp();
};

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = nullptr);
    int nextId() const override;

private:
    QLabel *topLabel{nullptr};
    QRadioButton *newMnemonicRadioButton{nullptr};
    QRadioButton *recoverFromMnemonicRadioButton{nullptr};
    QRadioButton *importWalletRadioButton{nullptr};
};

class ImportWalletDatPage : public QWizardPage
{
    Q_OBJECT

public:
    ImportWalletDatPage(QWidget *parent = nullptr);
    int nextId() const override;
    bool isComplete() const override;
    bool validatePage() override;

private Q_SLOTS:
    void setOpenFileName();

private:
    QString fileName;
    QLabel *openFileNameLabel{nullptr};
    QPushButton *openFileNameButton{nullptr};
};

class NewMnemonicSettingsPage : public QWizardPage
{
    Q_OBJECT

public:
    NewMnemonicSettingsPage(QWidget *parent = nullptr);
    int nextId() const override;
    bool validatePage() override;
    bool isComplete() const override;
    void cleanupPage() override;

    QStringList mnemonicList;
    std::string sKey;

private:
    QLabel *noteLabel{nullptr};
    QLabel *languageLabel{nullptr};
    QLabel *passwordLabel{nullptr};
    QLabel *passwordVerifyLabel{nullptr};
    QComboBox *languageComboBox{nullptr};
    QLineEdit *passwordEdit{nullptr};
    QLineEdit *passwordVerifyEdit{nullptr};
};

class NewMnemonicResultPage : public QWizardPage
{
    Q_OBJECT

public:
    NewMnemonicResultPage(QWidget *parent = nullptr);
    void initializePage() override;
    int nextId() const override;

    std::string sKey;
    QStringList mnemonicList;

private:
    QLabel *mnemonicLabel{nullptr};
    QLabel *noticeLabel{nullptr};
    std::vector<QLabel*> vMnemonicResultLabel;
};

class NewMnemonicVerificationPage : public QWizardPage
{
    Q_OBJECT

public:
    NewMnemonicVerificationPage(QWidget *parent = nullptr);
    int nextId() const override;
    bool isComplete() const override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void initializePage() override;

private:
    QLabel *mnemonicLabel{nullptr};
    QLabel *passwordLabel{nullptr};
    QLineEdit *passwordEdit{nullptr};
    std::vector<QLineEdit*> vMnemonicEdit;
};

class RecoverFromMnemonicPage : public QWizardPage
{
    Q_OBJECT

public:
    RecoverFromMnemonicPage(QWidget *parent = nullptr);
    int nextId() const override;
    bool validatePage() override;
    bool isComplete() const override;
    bool eventFilter(QObject *obj, QEvent *event) override;

    std::string sKey;

private:
    QLabel *mnemonicLabel{nullptr};
    QLabel *passwordLabel{nullptr};
    QLabel *passwordVerifyLabel{nullptr};
    QLineEdit *passwordEdit{nullptr};
    QLineEdit *passwordVerifyEdit{nullptr};
    std::vector<QLineEdit*> vMnemonicEdit;
};

class EncryptWalletPage : public QWizardPage
{
    Q_OBJECT

public:
    EncryptWalletPage(QWidget *parent = nullptr);
    int nextId() const override;
    void initializePage() override;
    bool isComplete() const override;
    bool validatePage() override;

private:
    int encryptWallet(const QString strWalletFile, const QString sBip44Key, const QString password);

    QLabel *progressLabel{nullptr};
    QProgressBar *progressBar{nullptr};
    QLabel *topLabel{nullptr};
    QLabel *passwordLabel{nullptr};
    QLineEdit *passwordEdit{nullptr};
    QLabel *passwordVerifyLabel{nullptr};
    QLineEdit *passwordVerifyEdit{nullptr};
};

#endif // SETUPWALLETWIZARD_H
