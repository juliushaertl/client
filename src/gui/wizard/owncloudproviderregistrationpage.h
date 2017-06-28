#ifndef OWNCLOUDPROVIDERREGISTRATIONPAGE_H
#define OWNCLOUDPROVIDERREGISTRATIONPAGE_H

#include <QWizardPage>

#include "ui_owncloudproviderregistrationpage.h"
#include "ocsregistrationjob.h"

namespace OCC
{

class OwncloudProviderRegistrationPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit OwncloudProviderRegistrationPage(QWidget *parent = 0);
    ~OwncloudProviderRegistrationPage();
    void setProvider(QString url);
    bool hasProvider();
    bool isComplete() const;

public slots:
    void verifyData();
    void updateVerifiedData(const QJsonDocument &jsonDocument, const QVariant &value);
    void slotOcsError(int statusCode, const QString &message);

private:
    Ui_OwncloudProviderRegistrationPage *ui;
    QString _registrationUrl;
    OcsRegistrationJob* _ocsRegistrationJob;
    bool _isValid = false;
};

}
#endif // OWNCLOUDPROVIDERREGISTRATIONPAGE_H
