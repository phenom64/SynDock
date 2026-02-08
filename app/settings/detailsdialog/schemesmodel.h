/*
    SPDX-FileCopyrightText: 2021 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SETTINGSSCHEMESMODEL_H
#define SETTINGSSCHEMESMODEL_H

#include <QAbstractListModel>

namespace NSE {
namespace WindowSystem {
class SchemeColors;
}
}

namespace NSE {
namespace Settings {
namespace Model {

class Schemes : public QAbstractListModel
{
    Q_OBJECT

public:
    enum SchemesRoles
    {
        IDROLE = Qt::UserRole + 1,
        NAMEROLE,
        TEXTCOLORROLE,
        BACKGROUNDCOLORROLE
    };

    explicit Schemes(QObject *parent = nullptr);
    virtual ~Schemes();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int row(const QString &id);

private slots:
    void initSchemes();

private:
    void insertSchemeInList(QString file);

private:
    QList<WindowSystem::SchemeColors *> m_schemes;
};

}
}
}

#endif
