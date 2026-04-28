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
    SPDX-FileCopyrightText: 2019 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "indicator.h"

// local
#include <coretypes.h>
#include "indicatorinfo.h"
#include "../containmentinterface.h"
#include "../view.h"
#include "../../lattecorona.h"
#include "../../indicator/factory.h"

// Qt
#include <QFileDialog>
#include <QFileInfo>
#include <QLatin1String>
#include <QQmlComponent>

// KDE
#include <KLocalizedString>
#include <KPluginMetaData>
#include <PlasmaQuick/SharedQmlEngine>

namespace NSE {
namespace ViewPart {

namespace {
QString indicatorMetadataValue(const KPluginMetaData &metadata, const QString &key)
{
    const QString value = metadata.value(key);
    if (!value.isEmpty()) {
        return value;
    }

    if (key.startsWith(QLatin1String("X-SynDock-"))) {
        const QString legacyKey = QString(key).replace(QStringLiteral("X-SynDock-"), QStringLiteral("X-Latte-"));
        return metadata.value(legacyKey);
    }

    return QString();
}
}

Indicator::Indicator(NSE::View *parent)
    : QObject(parent),
      m_view(parent),
      m_info(new IndicatorPart::Info(this)),
      m_resources(new IndicatorPart::Resources(this))
{
    m_corona = qobject_cast<NSE::Corona *>(m_view->corona());
    loadConfig();

    connect(this, &Indicator::enabledChanged, this, &Indicator::saveConfig);
    connect(this, &Indicator::pluginChanged, this, &Indicator::saveConfig);

    connect(m_view->extendedInterface(), &ContainmentInterface::hasLatteTasksChanged, this, &Indicator::latteTasksArePresentChanged);

    connect(m_view, &NSE::View::indicatorPluginChanged, [this](const QString &indicatorId) {
        if (m_corona && m_corona->indicatorFactory()->isCustomType(indicatorId)) {
            emit customPluginsChanged();
        }
    });

    connect(m_view, &NSE::View::indicatorPluginRemoved, [this](const QString &indicatorId) {
        if (m_corona && m_type == indicatorId && !m_corona->indicatorFactory()->pluginExists(indicatorId)) {
            setType("org.syndromatic.syndock.default");
        }

        if (m_corona && m_corona->indicatorFactory()->isCustomType(indicatorId)) {
            emit customPluginsChanged();
        }
    });

    load(m_type);

    loadPlasmaComponent();
}

Indicator::~Indicator()
{
    unloadIndicators();

    if (m_component) {
        m_component->deleteLater();
    }

    if (m_configLoader) {
        m_configLoader->deleteLater();
    }

    if (m_configuration) {
        m_configuration->deleteLater();
    }

    if (m_info) {
        m_info->deleteLater();
    }
}

bool Indicator::enabled() const
{
    return m_enabled;
}

void Indicator::setEnabled(bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }

    m_enabled = enabled;
    emit enabledChanged();
}

bool Indicator::enabledForApplets() const
{
    return m_enabledForApplets;
}

void Indicator::setEnabledForApplets(bool enabled)
{
    if (m_enabledForApplets == enabled) {
        return;
    }

    m_enabledForApplets = enabled;
    emit enabledForAppletsChanged();
}

bool Indicator::isCustomIndicator() const
{
    return m_corona->indicatorFactory()->isCustomType(type());
}

bool Indicator::latteTasksArePresent()
{
    return m_view->extendedInterface()->hasLatteTasks();
}

bool Indicator::pluginIsReady()
{
    return m_pluginIsReady;
}

void Indicator::setPluginIsReady(bool ready)
{
    if (m_pluginIsReady == ready) {
        return;
    }

    m_pluginIsReady = ready;
    emit pluginIsReadyChanged();
}

int Indicator::index(const QString &type)
{
    if (type == QLatin1String("org.syndromatic.syndock.default")) {
        return 0;
    } else if (type == QLatin1String("org.syndromatic.syndock.plasma")) {
        return 1;
    } else if (type == QLatin1String("org.syndromatic.syndock.plasmatabstyle")) {
        return 2;
    } else if (customPluginIds().contains(type)){
        return 3 + customPluginIds().indexOf(type);
    }

    return -1;
}

QString Indicator::type() const
{
    return m_type;
}

void Indicator::setType(QString type)
{
    if (m_type == type) {
        return;
    }

    load(type);
}

QString Indicator::customType() const
{
    return m_customType;
}

void Indicator::setCustomType(QString type)
{
    if (m_customType == type) {
        return;
    }

    m_customType = type;
    emit customPluginChanged();
}

int Indicator::customPluginsCount() const
{
    return m_corona->indicatorFactory()->customPluginsCount();
}

QString Indicator::uiPath() const
{
    return m_corona->indicatorFactory()->uiPath(m_type);
}

QStringList Indicator::customPluginIds() const
{
    return m_corona->indicatorFactory()->customPluginIds();
}

QStringList Indicator::customPluginNames() const
{
    return m_corona->indicatorFactory()->customPluginNames();
}

QStringList Indicator::customLocalPluginIds() const
{
    return m_corona->indicatorFactory()->customLocalPluginIds();
}

IndicatorPart::Info *Indicator::info() const
{
    return m_info;
}

IndicatorPart::Resources *Indicator::resources() const
{
    return m_resources;
}

QQmlComponent *Indicator::component() const
{
    return m_component;
}

QQmlComponent *Indicator::plasmaComponent() const
{
    return m_plasmaComponent;
}

QObject *Indicator::configuration() const
{
    return m_configuration;
}

void Indicator::load(QString type)
{
    KPluginMetaData metadata = m_corona->indicatorFactory()->metadata(type);

    if (metadata.isValid()) {
        bool state{m_enabled};
        //! remove all previous indicators
        setPluginIsReady(false);

        m_metadata = metadata;
        m_type = type;
        m_pluginPath = QFileInfo(m_metadata.fileName()).absolutePath();

        if (m_corona && m_corona->indicatorFactory()->isCustomType(type)) {
            setCustomType(type);
        }

        updateScheme();
        updateComponent();

        emit pluginChanged();

        //! create all indicators with the new type
        setPluginIsReady(true);
    } else if (type!="org.syndromatic.syndock.default") {
        qDebug() << " Indicator metadata are not valid : " << type;
        setType("org.syndromatic.syndock.default");
    }
}

void Indicator::updateComponent()
{
    auto prevComponent = m_component;

    QString uiPath = indicatorMetadataValue(m_metadata, QStringLiteral("X-SynDock-MainScript"));

    if (!uiPath.isEmpty()) {
        uiPath = m_pluginPath + "/package/" + uiPath;
        m_component = new QQmlComponent(m_view->engine().get(), uiPath);
    }

    if (prevComponent) {
        prevComponent->deleteLater();
    }
}

void Indicator::loadPlasmaComponent()
{
    auto prevComponent = m_plasmaComponent;

    KPluginMetaData metadata = m_corona->indicatorFactory()->metadata("org.syndromatic.syndock.plasmatabstyle");
    QString uiPath = indicatorMetadataValue(metadata, QStringLiteral("X-SynDock-MainScript"));

    if (!uiPath.isEmpty()) {
        uiPath = QFileInfo(metadata.fileName()).absolutePath() + "/package/" + uiPath;
        m_plasmaComponent = new QQmlComponent(m_view->engine().get(), uiPath);
    }

    if (prevComponent) {
        prevComponent->deleteLater();
    }

    emit plasmaComponentChanged();
}

void Indicator::unloadIndicators()
{
    setPluginIsReady(false);
}

void Indicator::updateScheme()
{
    auto prevConfigLoader = m_configLoader;
    auto prevConfiguration = m_configuration;

    QString xmlPath = indicatorMetadataValue(m_metadata, QStringLiteral("X-SynDock-ConfigXml"));

    if (!xmlPath.isEmpty()) {
        QFile file(m_pluginPath + "/package/" + xmlPath);
        m_configLoader = new KConfigLoader(m_view->containment()->config().group("Indicator").group(m_metadata.pluginId()), &file);
        m_configuration = new KConfigPropertyMap(m_configLoader, this);
    } else {
        m_configLoader = nullptr;
        m_configuration = nullptr;
    }

    if (prevConfigLoader) {
        prevConfigLoader->deleteLater();
    }

    if (prevConfiguration) {
        prevConfiguration->deleteLater();
    }

    emit configurationChanged();
}

void Indicator::loadConfig()
{
    auto config = m_view->containment()->config().group("Indicator");
    m_customType = config.readEntry("customType", QString());
    m_enabled = config.readEntry("enabled", true);
    m_type = config.readEntry("type", "org.syndromatic.syndock.default");
}

void Indicator::saveConfig()
{
    auto config = m_view->containment()->config().group("Indicator");
    config.writeEntry("customType", m_customType);
    config.writeEntry("enabled", m_enabled);
    config.writeEntry("type", m_type);
}

}
}
