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

#include "templatesmanager.h"

// local
#include "../layout/abstractlayout.h"
#include "../layout/centrallayout.h"
#include "../layouts/importer.h"
#include "../layouts/manager.h"
#include "../layouts/storage.h"
#include "../tools/commontools.h"
#include "../view/view.h"

// Qt
#include <QDir>
#include <QFileInfo>

// KDE
#include <KDirWatch>
#include <KLocalizedString>
#include <KPackage/Package>

namespace NSE {
namespace Templates {

Manager::Manager(NSE::Corona *corona)
    : QObject(corona),
      m_corona(corona)
{
    KDirWatch::self()->addDir(NSE::dataPath() + "/syndock/templates", KDirWatch::WatchFiles);
    connect(KDirWatch::self(), &KDirWatch::created, this, &Manager::onCustomTemplatesCountChanged);
    connect(KDirWatch::self(), &KDirWatch::deleted, this, &Manager::onCustomTemplatesCountChanged);
    connect(KDirWatch::self(), &KDirWatch::dirty, this, &Manager::onCustomTemplatesCountChanged);
}

Manager::~Manager()
{
}

void Manager::init()
{
    connect(this, &Manager::viewTemplatesChanged, m_corona->layoutsManager(), &NSE::Layouts::Manager::viewTemplatesChanged);

    initLayoutTemplates();
    initViewTemplates();
}

void Manager::initLayoutTemplates()
{
    m_layoutTemplates.clear();
    const QString systemTemplates = m_corona->kPackage().filePath("templates");
    if (systemTemplates.isEmpty()) {
        qWarning() << "SynDock templates: shell package did not resolve a system templates directory";
    } else {
        qDebug() << "SynDock templates: loading layout templates from" << systemTemplates;
    }

    initLayoutTemplates(systemTemplates);
    initLayoutTemplates(NSE::dataPath() + "/syndock/templates");
    emit layoutTemplatesChanged();
}

void Manager::initViewTemplates()
{
    m_viewTemplates.clear();
    const QString systemTemplates = m_corona->kPackage().filePath("templates");
    if (systemTemplates.isEmpty()) {
        qWarning() << "SynDock templates: shell package did not resolve a system templates directory";
    } else {
        qDebug() << "SynDock templates: loading view templates from" << systemTemplates;
    }

    initViewTemplates(systemTemplates);
    initViewTemplates(NSE::dataPath() + "/syndock/templates");
    emit viewTemplatesChanged();
}

void Manager::initLayoutTemplates(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }

    QDir templatesDir(path);
    QStringList filter;
    filter.append(QString("*") + layoutTemplateExtension());
    QStringList templates = templatesDir.entryList(filter, QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    qDebug() << "SynDock templates: found" << templates.count() << "layout templates in" << path;

    for (int i=0; i<templates.count(); ++i) {
        QString templatePath = templatesDir.path() + "/" + templates[i];
        if (!m_layoutTemplates.containsId(templatePath)) {
            CentralLayout layouttemplate(this, templatePath);

            Data::Layout tdata = layouttemplate.data();
            tdata.isTemplate = true;

            if (tdata.name == DEFAULTLAYOUTTEMPLATENAME || tdata.name == EMPTYLAYOUTTEMPLATENAME) {
                QByteArray templateNameChars = tdata.name.toUtf8();
                tdata.name = i18n(templateNameChars);
            }

            m_layoutTemplates << tdata;
        }
    }
}

void Manager::initViewTemplates(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }

    bool istranslated = (m_corona->kPackage().filePath("templates") == path);

    QDir templatesDir(path);
    QStringList filter;
    filter.append(QString("*") + viewTemplateExtension());
    QStringList templates = templatesDir.entryList(filter, QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    qDebug() << "SynDock templates: found" << templates.count() << "view templates in" << path;

    for (int i=0; i<templates.count(); ++i) {
        QString templatePath = templatesDir.path() + "/" + templates[i];

        if (!m_viewTemplates.containsId(templatePath)) {
            Data::Generic vdata;
            vdata.id = templatePath;
            QString tname = QFileInfo(templatePath).baseName();

            if (istranslated) {
                QByteArray tnamechars = tname.toUtf8();
                vdata.name = i18nc("view template name", tnamechars);
            } else {
                vdata.name = tname;
            }

            m_viewTemplates << vdata;
        }
    }
}

Data::Layout Manager::layoutTemplateForName(const QString &layoutName)
{
    if (m_layoutTemplates.containsName(layoutName)) {
        QString layoutid = m_layoutTemplates.idForName(layoutName);
        return m_layoutTemplates[layoutid];
    }

    return Data::Layout();
}

Data::LayoutsTable Manager::layoutTemplates()
{
    Data::LayoutsTable templates;

    QString id = m_layoutTemplates.idForName(i18n(DEFAULTLAYOUTTEMPLATENAME));
    templates << m_layoutTemplates[id];
    id = m_layoutTemplates.idForName(i18n(EMPTYLAYOUTTEMPLATENAME));
    templates << m_layoutTemplates[id];

    for (int i=0; i<m_layoutTemplates.rowCount(); ++i) {
        if ( m_layoutTemplates[i].name != i18n(DEFAULTLAYOUTTEMPLATENAME)
             && m_layoutTemplates[i].name != i18n(EMPTYLAYOUTTEMPLATENAME)
             && m_layoutTemplates[i].name != Layout::MULTIPLELAYOUTSHIDDENNAME) {
            templates << m_layoutTemplates[i];
        }
    }

    return templates;
}

Data::GenericBasicTable Manager::viewTemplates()
{
    return m_viewTemplates;
}

QString Manager::newLayout(QString layoutName, QString layoutTemplate)
{
    if (!m_layoutTemplates.containsName(layoutTemplate)) {
        qWarning() << "SynDock templates: requested layout template is missing:" << layoutTemplate;

        if (layoutTemplate != i18n(DEFAULTLAYOUTTEMPLATENAME) && m_layoutTemplates.containsName(i18n(DEFAULTLAYOUTTEMPLATENAME))) {
            qWarning() << "SynDock templates: falling back to Default layout template";
            layoutTemplate = i18n(DEFAULTLAYOUTTEMPLATENAME);
        } else {
            return QString();
        }
    }

    if (layoutName.isEmpty()) {
        layoutName = Layouts::Importer::uniqueLayoutName(layoutTemplate);
    } else {
        layoutName = Layouts::Importer::uniqueLayoutName(layoutName);
    }

    QString newLayoutPath = Layouts::Importer::layoutUserFilePath(layoutName);

    Data::Layout dlayout = layoutTemplateForName(layoutTemplate);
    if (dlayout.id.isEmpty() || !QFileInfo::exists(dlayout.id)) {
        qWarning() << "SynDock templates: layout template file is missing:" << dlayout.id << "for template" << layoutTemplate;
        return QString();
    }

    if (!QFile(dlayout.id).copy(newLayoutPath)) {
        qWarning() << "SynDock templates: failed to copy layout template" << dlayout.id << "to" << newLayoutPath;
        return QString();
    }

    qDebug() << "SynDock templates: added layout" << layoutName << "from template" << layoutTemplate << "at" << newLayoutPath;

    emit newLayoutAdded(newLayoutPath);

    return newLayoutPath;
}

bool Manager::exportTemplate(const QString &originFile, const QString &destinationFile, const Data::AppletsTable &approvedApplets)
{
    return NSE::Layouts::Storage::self()->exportTemplate(originFile, destinationFile, approvedApplets);
}

bool Manager::exportTemplate(const NSE::View *view, const QString &destinationFile, const Data::AppletsTable &approvedApplets)
{
    return NSE::Layouts::Storage::self()->exportTemplate(view->layout(), view->containment(), destinationFile, approvedApplets);
}

void Manager::onCustomTemplatesCountChanged(const QString &file)
{
    if (file.startsWith(NSE::dataPath() + "/syndock/templates")) {
        if (file.endsWith(layoutTemplateExtension())) {
            initLayoutTemplates();
        } else if (file.endsWith(viewTemplateExtension())) {
            initViewTemplates();
        }
    }
}

void Manager::importSystemLayouts()
{
    for (int i=0; i<m_layoutTemplates.rowCount(); ++i) {
        if (m_layoutTemplates[i].isSystemTemplate()) {
            QString userLayoutPath = Layouts::Importer::layoutUserFilePath(m_layoutTemplates[i].name);

            if (!QFile(userLayoutPath).exists()) {
                QFile(m_layoutTemplates[i].id).copy(userLayoutPath);
                qDebug() << "adding layout : " << userLayoutPath << " based on layout template:" << m_layoutTemplates[i].name;
            }
        }
    }
}

QString Manager::proposedTemplateAbsolutePath(QString templateFilename)
{
    QString tempfilename = templateFilename;

    if (tempfilename.endsWith(layoutTemplateExtension())) {
        QString clearedname = tempfilename.chopped(layoutTemplateExtension().size());
        tempfilename = uniqueLayoutTemplateName(clearedname) + layoutTemplateExtension();
    } else if (tempfilename.endsWith(viewTemplateExtension())) {
        QString clearedname = tempfilename.chopped(viewTemplateExtension().size());
        tempfilename = uniqueViewTemplateName(clearedname) + viewTemplateExtension();
    }

    return QString(NSE::dataPath() + "/syndock/templates/" + tempfilename);
}

bool Manager::hasCustomLayoutTemplate(const QString &templateName) const
{
    for (int i=0; i<m_layoutTemplates.rowCount(); ++i) {
        if (m_layoutTemplates[i].name == templateName && !m_layoutTemplates[i].isSystemTemplate()) {
            return true;
        }
    }

    return false;
}

bool Manager::hasLayoutTemplate(const QString &templateName) const
{
    return m_layoutTemplates.containsName(templateName);
}

bool Manager::hasViewTemplate(const QString &templateName) const
{
    return m_viewTemplates.containsName(templateName);
}

QString Manager::viewTemplateFilePath(const QString templateName) const
{
    if (m_viewTemplates.containsName(templateName)) {
        return m_viewTemplates.idForName(templateName);
    }

    return QString();
}

void Manager::installCustomLayoutTemplate(const QString &templateFilePath)
{
    if (!templateFilePath.endsWith(layoutTemplateExtension())) {
        return;
    }

    QString layoutName = QFileInfo(templateFilePath).baseName();

    QString destinationFilePath = NSE::dataPath() + "/syndock/templates/" + layoutName + layoutTemplateExtension();

    if (hasCustomLayoutTemplate(layoutName)) {
        QFile(destinationFilePath).remove();
    }

    QFile(templateFilePath).copy(destinationFilePath);
}

QString Manager::uniqueLayoutTemplateName(QString name) const
{
    int pos_ = name.lastIndexOf(QRegularExpression(QString(" - [0-9]+")));

    if (hasLayoutTemplate(name) && pos_ > 0) {
        name = name.left(pos_);
    }

    int i = 2;

    QString namePart = name;

    while (hasLayoutTemplate(name)) {
        name = namePart + " - " + QString::number(i);
        i++;
    }

    return name;
}

QString Manager::uniqueViewTemplateName(QString name) const
{
    int pos_ = name.lastIndexOf(QRegularExpression(QString(" - [0-9]+")));

    if (hasViewTemplate(name) && pos_ > 0) {
        name = name.left(pos_);
    }

    int i = 2;

    QString namePart = name;

    while (hasViewTemplate(name)) {
        name = namePart + " - " + QString::number(i);
        i++;
    }

    return name;
}

QString Manager::templateName(const QString &filePath)
{
    int lastSlash = filePath.lastIndexOf("/");
    QString tempFilePath = filePath;
    QString templatename = tempFilePath.remove(0, lastSlash + 1);

    QString extension(layoutTemplateExtension());
    int ext = templatename.lastIndexOf(extension);
    if (ext>0) {
        templatename = templatename.remove(ext, extension.size());
    } else {
        extension = viewTemplateExtension();
        ext = templatename.lastIndexOf(extension);
        templatename = templatename.remove(ext,extension.size());
    }

    return templatename;
}

QString Manager::layoutTemplateExtension()
{
    return QStringLiteral(".layout.latte");
}

QString Manager::viewTemplateExtension()
{
    return QStringLiteral(".view.latte");
}

//! it is used in order to provide translations for system templates
void Manager::exposeTranslatedTemplateNames()
{
    //! layout templates default names
    i18nc("synos layout template name", "SynOS");
    i18nc("default layout template name", "Default");
    i18nc("empty layout template name", "Empty");

    //! dock/panel templates default names
    i18nc("view template name", "Default Dock");
    i18nc("view template name", "Default Panel");
    i18nc("view template name", "Empty Panel");
}

}
}
