/*
    SPDX-FileCopyrightText: 2020 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "layoutcmbitemdelegate.h"

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

LayoutCmbItemDelegate::LayoutCmbItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void LayoutCmbItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem myOptions = option;

    //! background
    NSE::drawBackground(painter, option);
    NSE::Data::LayoutIcon icon = index.data(Model::Layouts::BACKGROUNDUSERROLE).value<NSE::Data::LayoutIcon>();

    //! icon
    QRect remained = NSE::remainedFromLayoutIcon(myOptions, Qt::AlignLeft, 4, 2);
    NSE::drawLayoutIcon(painter, myOptions, icon.isBackgroundFile, icon.name, Qt::AlignLeft, 5, 2); //+1px in order to take into account popup window border
    myOptions.rect = remained;

    //!
    QStyledItemDelegate::paint(painter, myOptions, index);
}

}
}
}
}

