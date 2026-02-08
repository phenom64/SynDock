/*
    SPDX-FileCopyrightText: 2020 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef COLORSMODEL_H
#define COLORSMODEL_H

// local
#include "../../lattecorona.h"
#include "../../data/layoutcolordata.h"

// Qt
#include <QAbstractTableModel>
#include <QModelIndex>


namespace NSE {
namespace Settings {
namespace Model {

class Colors : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ColorsRoles
    {
        IDROLE = Qt::UserRole + 1,
        NAMEROLE,
        PATHROLE,
        TEXTCOLORROLE
    };

    explicit Colors(QObject *parent, NSE::Corona *corona);
    ~Colors();

    int rowCount() const;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    int row(const QString &id);

    QString colorPath(const QString &color);

    QVariant data(const QModelIndex &index, int role) const override;

private:
    void init();
    void add(const QString &newid, const QString &newname, const QString &newpath, const QString &newtextcolor);  

private:
    QString m_colorsPath;

    QList<NSE::Data::LayoutColor> m_colorsTable;

    NSE::Corona *m_corona{nullptr};
};

}
}
}

#endif
