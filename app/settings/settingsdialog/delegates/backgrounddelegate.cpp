/*
    SPDX-FileCopyrightText: 2017-2018 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "backgrounddelegate.h"

// local
#include "../layoutsmodel.h"
#include "../../generic/generictools.h"

// Qt
#include <QDebug>
#include <QModelIndex>
#include <QPainter>
#include <QString>


namespace NSE {
namespace Settings {
namespace Layout {
namespace Delegate {

const int MARGIN = 2;

BackgroundDelegate::BackgroundDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void BackgroundDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem myOptions = option;
    NSE::Data::LayoutIcon icon = index.data(Qt::UserRole).value<NSE::Data::LayoutIcon>();

    //! background
    NSE::drawBackground(painter, option);
    NSE::drawLayoutIcon(painter, option, icon.isBackgroundFile, icon.name, Qt::AlignHCenter, -1, 3);
}

}
}
}
}

