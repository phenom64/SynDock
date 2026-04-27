/*
    SPDX-FileCopyrightText: 2019 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "trackedlayoutinfo.h"

//local
#include "windowstracker.h"
#include "../../layout/genericlayout.h"

namespace NSE {
namespace WindowSystem {
namespace Tracker {


TrackedLayoutInfo::TrackedLayoutInfo(Tracker::Windows *tracker, NSE::Layout::GenericLayout *layout)
    : TrackedGeneralInfo(tracker),
      m_layout(layout)
{
    m_activities = m_layout->appliedActivities();

    connect(m_layout, &NSE::Layout::GenericLayout::activitiesChanged, this, [&]() {
        m_activities = m_layout->appliedActivities();
        updateTrackingCurrentActivity();
    });
}

TrackedLayoutInfo::~TrackedLayoutInfo()
{
}

NSE::Layout::GenericLayout *TrackedLayoutInfo::layout() const
{
    return m_layout;
}

}
}
}
