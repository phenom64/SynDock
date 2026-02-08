/*
    SPDX-FileCopyrightText: 2016 Smith AR <audoban@openmailbox.org>
    SPDX-FileCopyrightText: 2016 Michail Vourlakos <mvourlakos@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SYNDOCKPACKAGE_H
#define SYNDOCKPACKAGE_H

// Qt
#include <QObject>

// KDE
#include <KPackage/PackageStructure>

namespace NSE {
class Package : public KPackage::PackageStructure
{
    Q_OBJECT

public:
    explicit Package(QObject *parent = 0, const QVariantList &args = QVariantList());

    ~Package() override;
    void initPackage(KPackage::Package *package) override;
    void pathChanged(KPackage::Package *package) override;
};

}
#endif // SYNDOCKPACKAGE_H
