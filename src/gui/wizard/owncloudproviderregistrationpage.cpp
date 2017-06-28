#include "owncloudproviderregistrationpage.h"
#include "ui_owncloudproviderregistrationpage.h"
#include "wizard/owncloudwizardcommon.h"
#include <QDebug>
namespace OCC
{

OwncloudProviderRegistrationPage::OwncloudProviderRegistrationPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::OwncloudProviderRegistrationPage)
{
    ui->setupUi(this);
    setTitle(WizardCommon::titleTemplate().arg(tr("Hosting providers")));
    setSubTitle(WizardCommon::subTitleTemplate().arg(tr("find a provider to create a new account")));
    connect(ui->fieldUsername, SIGNAL(editingFinished()), this, SLOT(verifyData()));
    connect(ui->fieldDisplayname, SIGNAL(editingFinished()), this, SLOT(verifyData()));
    connect(ui->fieldEmail, SIGNAL(editingFinished()), this, SLOT(verifyData()));
    connect(ui->fieldPassword, SIGNAL(editingFinished()), this, SLOT(verifyData()));
}

OwncloudProviderRegistrationPage::~OwncloudProviderRegistrationPage()
{
    delete ui;
    delete _ocsRegistrationJob;
}

void OwncloudProviderRegistrationPage::setProvider(QString url)
{
    _registrationUrl = url;
    _ocsRegistrationJob = new OcsRegistrationJob(url);
    wizard()->setButtonText(QWizard::NextButton, "Create new account");
}

bool OwncloudProviderRegistrationPage::hasProvider()
{
    if(_registrationUrl != NULL)
        return true;
    return false;
}

void OwncloudProviderRegistrationPage::verifyData()
{
    QString username = ui->fieldUsername->text();
    QString displayname = ui->fieldDisplayname->text();
    QString email = ui->fieldEmail->text();

    OcsRegistrationJob *job = new OcsRegistrationJob(_registrationUrl);
    connect(job, SIGNAL(shareJobFinished(QJsonDocument, QVariant)), SLOT(updateVerifiedData(QJsonDocument,QVariant)));
    connect(job, SIGNAL(ocsError(int, QString)), SLOT(slotOcsError(int, QString)));
    job->verify(username, displayname, email);
    emit completeChanged();
}
void OwncloudProviderRegistrationPage::updateVerifiedData(const QJsonDocument &jsonDocument, const QVariant &value)
{
    qDebug() << "SUCCESS";
    qDebug() << jsonDocument;
    qDebug() << value;
    _isValid = true;
    emit completeChanged();
}

void OwncloudProviderRegistrationPage::slotOcsError(int statusCode, const QString &message)
{
    qDebug() << "ERRROR";
    qDebug() << statusCode << " " << message;
    _isValid = false;
    emit completeChanged();
}

bool OwncloudProviderRegistrationPage::isComplete() const
{
    return _isValid && !ui->fieldPassword->text().isEmpty();
}

}
