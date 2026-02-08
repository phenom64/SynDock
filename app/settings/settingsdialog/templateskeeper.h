/*
    SPDX-FileCopyrightText: 2021 Michail Vourlakos <mvourlakos@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SETTINGSPARTTEMPLATESKEEPER_H
#define SETTINGSPARTTEMPLATESKEEPER_H

// local
#include "../../data/viewdata.h"
#include "../../data/viewstable.h"

// Qt
#include <QObject>

namespace NSE {
class CentralLayout;
class Corona;
namespace Settings {
namespace Controller {
class Layouts;
}
}
}

namespace NSE {
namespace Settings {
namespace Part {

class TemplatesKeeper : public QObject
{
    Q_OBJECT

public:
    explicit TemplatesKeeper(Settings::Controller::Layouts *parent, NSE::Corona *corona);
    ~TemplatesKeeper();

    QString storedView(const QString &layoutCurrentId, const QString &viewId);

    bool hasClipboardContents() const;

    NSE::Data::ViewsTable clipboardContents() const;
    void setClipboardContents(const NSE::Data::ViewsTable &views);

public slots:
    void clear();

signals:
    void clipboardContentsChanged();

private:
    QString viewKeeperId(const QString &layoutCurrentId, const QString &viewId);

private:
    NSE::Data::ViewsTable m_storedViews;
    NSE::Data::ViewsTable m_clipboardViews;

    NSE::Corona *m_corona{nullptr};
    Settings::Controller::Layouts *m_layoutsController{nullptr};

    QList<CentralLayout *> m_garbageLayouts;
};

}
}
}

#endif
