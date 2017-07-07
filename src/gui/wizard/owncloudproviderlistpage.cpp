#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QStringListModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QListWidgetItem>
#include <QObject>

#include "owncloudproviderlistpage.h"
#include "owncloudproviderregistrationpage.h"
#include "owncloudwizard.h"
#include "ui_owncloudproviderlistpage.h"
#include "theme.h"
#include "config.h"
#include "wizard/owncloudwizardcommon.h"
#include "QProgressIndicator.h"
#include "ui_providerwidget.h"
#include "providerwidget.h"

namespace OCC
{
OwncloudProviderListPage::OwncloudProviderListPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui_OwncloudProviderListPage),
    countryModel(new QStringListModel(this)),
    _progressIndicator(new QProgressIndicator(this)),
    showFreeOnly(false)
{
    ui->setupUi(this);
    setTitle(WizardCommon::titleTemplate().arg(tr("Hosting providers")));
    setSubTitle(WizardCommon::subTitleTemplate().arg(tr("find a provider to create a new account")));
    setupCustomization();
}

OwncloudProviderListPage::~OwncloudProviderListPage()
{
    delete countryModel;
}

void OwncloudProviderListPage::setupCustomization()
{
    ui->horizontalLayout->addWidget(_progressIndicator, 0, Qt::AlignLeft);
    this->setLayout(ui->verticalLayout);
    ui->listWidget->setAlternatingRowColors(true);
    startSpinner();
}

void OwncloudProviderListPage::initializePage() {
    loadProviders();
    filterProviders();
}

void OwncloudProviderListPage::startSpinner()
{
    //_ui.resultLayout->setEnabled(true);
    _progressIndicator->setVisible(true);
    _progressIndicator->startAnimation();
}

void OwncloudProviderListPage::stopSpinner()
{
    ui->bottomLabel->setText("");
    ui->topLabel->setText("");
    _progressIndicator->setVisible(false);
    _progressIndicator->stopAnimation();
}
void OwncloudProviderListPage::toggleFreePlans(bool state)
{
    showFreeOnly = state;
    filterProviders();
}

void OwncloudProviderListPage::setCountry(QString current)
{
    showCountryOnly = new QString(current);
    filterProviders();
}

void OwncloudProviderListPage::loadProviders()
{

    _nam = new QNetworkAccessManager(this);
    QObject::connect(_nam, SIGNAL(finished(QNetworkReply*)),
                     this,
                     SLOT(serviceRequestFinished(QNetworkReply*)));
    QUrl url(APPLICATION_PROVIDERS);
    _nam->get(QNetworkRequest(url));
    qDebug() << "Loading providers from" << url;

}

void OwncloudProviderListPage::filterProviders()
{
    const int itemCount = ui->listWidget->count();
    for ( int index = 0; index < itemCount; index++)
    {
          QListWidgetItem *item = ui->listWidget->item(index);
          QJsonArray countries = item->data(ProviderWidget::DataRole::countryRole).toJsonArray();
          bool free = item->data(ProviderWidget::DataRole::freeRole).toBool();
          bool countryMatches = false;
          foreach (const QJsonValue & value, countries) {
              QString country = value.toString();
              if(country.contains(ui->country->currentText())) {
                  countryMatches = true;
              }
          }

          if((!free && showFreeOnly) || !countryMatches) {
              item->setHidden(true);
          } else {
              item->setHidden(false);
          }
    }
}

void OwncloudProviderListPage::serviceRequestFinished(QNetworkReply* reply)
{
    if(reply->error() == QNetworkReply::NoError) {
        QString strReply = (QString)reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QStringList countryList;

        if (!jsonResponse.isArray())
            return;
        foreach (const QJsonValue & value, jsonResponse.array()) {
            QJsonObject object = value.toObject();
            foreach(const QJsonValue & flag, object["flags"].toArray()) {
                countryList << flag.toString();
            }
            ProviderWidget *widget = new ProviderWidget(this);
            QListWidgetItem *witem = new QListWidgetItem();
            witem->setData(ProviderWidget::DataRole::headerTextRole, object["title"].toString());
            witem->setData(ProviderWidget::DataRole::subHeaderTextRole, object["specializes"].toArray()[0].toString());
            witem->setData(ProviderWidget::DataRole::registrationRole, object["registration"].toString());
            witem->setData(ProviderWidget::DataRole::providerUrlRole, object["url"].toString());
            witem->setData(ProviderWidget::DataRole::imageRole, object["imagename"].toString());
            witem->setData(ProviderWidget::DataRole::freeRole, object["freeplans"].toBool());
            witem->setData(ProviderWidget::DataRole::countryRole, object["flags"].toArray());

            ui->listWidget->addItem(witem);
            ui->listWidget->setItemWidget(witem, qobject_cast<QWidget*>(widget));
            widget->updateProvider(witem);
            witem->setSizeHint(QSize(ui->listWidget->sizeHint().width(), widget->sizeHint().height()));
        }
        countryList.removeDuplicates();
        countryList.sort();
        countryModel->setStringList(countryList);
        ui->country->setModel(countryModel);
        reply->deleteLater();
        stopSpinner();
    } else {
        ui->bottomLabel->setText(tr("Failed to fetch provider list."));
    }
}

void OwncloudProviderListPage::openRegistration(QString url)
{
    qDebug() << url;
    OwncloudProviderRegistrationPage *page = qobject_cast<OwncloudProviderRegistrationPage *>(wizard()->page(WizardCommon::Page_ProviderRegistration));
    page->setProvider(url);
    wizard()->next();
}

int OwncloudProviderListPage::nextId() const
{
    return WizardCommon::Page_ProviderRegistration;
}

bool OwncloudProviderListPage::isComplete() const
{
    OwncloudProviderRegistrationPage *page = qobject_cast<OwncloudProviderRegistrationPage *>(wizard()->page(WizardCommon::Page_ProviderRegistration));
    if(page->hasProvider())
        return true;
    return false;
}

}
