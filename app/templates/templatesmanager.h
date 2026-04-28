/* This file is a part of the Atmo Desktop Dock project 'SynDock' for SynOS.
 * Copyright (C) 2026 Syndromatic Ltd. All rights reserved
 * Designed by Kavish Krishnakumar in Manchester.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITH ABSOLUTELY NO WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Based on Latte Dock.
 */

/*
    SPDX-FileCopyrightText: 2020 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef TEMPLATESMANAGER_H
#define TEMPLATESMANAGER_H

// local
#include "../lattecorona.h"
#include "../data/appletdata.h"
#include "../data/layoutdata.h"
#include "../data/layoutstable.h"
#include "../data/genericbasictable.h"

// Qt
#include <QObject>

// KDE
#include <KLocalizedString>

namespace NSE {
class Corona;
class View;
}

namespace NSE {
namespace Templates {

const char DEFAULTLAYOUTTEMPLATENAME[] = "Default";
const char EMPTYLAYOUTTEMPLATENAME[] = "Empty";
const char SYNOSLAYOUTTEMPLATENAME[] = "SynOS";

class Manager : public QObject
{
    Q_OBJECT

public:
    Manager(NSE::Corona *corona = nullptr);
    ~Manager() override;

    NSE::Corona *corona();
    void init();

    bool hasCustomLayoutTemplate(const QString &templateName) const;
    bool hasLayoutTemplate(const QString &templateName) const;
    bool hasViewTemplate(const QString &templateName) const;

    bool exportTemplate(const QString &originFile, const QString &destinationFile, const Data::AppletsTable &approvedApplets);
    bool exportTemplate(const NSE::View *view, const QString &destinationFile, const Data::AppletsTable &approvedApplets);

    Data::Layout layoutTemplateForName(const QString &layoutName);

    Data::LayoutsTable layoutTemplates();
    Data::GenericBasicTable viewTemplates();

    //! creates a new layout with layoutName based on specific layout template and returns the new layout path
    QString newLayout(QString layoutName, QString layoutTemplate = i18n(DEFAULTLAYOUTTEMPLATENAME));

    QString proposedTemplateAbsolutePath(QString templateFilename);

    QString viewTemplateFilePath(const QString templateName) const;

    static QString templateName(const QString &filePath);
    static QString layoutTemplateExtension();
    static QString viewTemplateExtension();

    void importSystemLayouts();
    void installCustomLayoutTemplate(const QString &templateFilePath);

signals:
    void newLayoutAdded(const QString &path);
    void layoutTemplatesChanged();
    void viewTemplatesChanged();

private slots:
    void onCustomTemplatesCountChanged(const QString &file);

private:
    void initLayoutTemplates();
    void initViewTemplates();

    void initLayoutTemplates(const QString &path);
    void initViewTemplates(const QString &path);

    void exposeTranslatedTemplateNames();

    QString uniqueLayoutTemplateName(QString name) const;
    QString uniqueViewTemplateName(QString name) const;

private:
    NSE::Corona *m_corona;

    Data::LayoutsTable m_layoutTemplates;
    Data::GenericBasicTable m_viewTemplates;

};

}
}

#endif //TEMPLATESMANAGER_H
