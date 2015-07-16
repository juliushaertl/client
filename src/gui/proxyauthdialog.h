/*
 * Copyright (C) 2015 by Christian Kamm <kamm@incasoftware.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#ifndef OCC_PROXYAUTHDIALOG_H
#define OCC_PROXYAUTHDIALOG_H

#include <QDialog>

namespace OCC {

namespace Ui {
class ProxyAuthDialog;
}

/**
 * @brief Ask for username and password for a given proxy.
 *
 * Used by ProxyAuthHandler.
 */
class ProxyAuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProxyAuthDialog(QWidget *parent = 0);
    ~ProxyAuthDialog();

    void setProxyAddress(const QString& address);

    QString username() const;
    QString password() const;

    /// Resets the username and password.
    void reset();

private:
    Ui::ProxyAuthDialog *ui;
};


} // namespace OCC
#endif // OCC_PROXYAUTHDIALOG_H
