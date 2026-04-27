/*
    SPDX-FileCopyrightText: 2021 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "namedelegate.h"

// local
#include "../viewsmodel.h"
#include "../../generic/generictools.h"
#include "../../generic/genericviewtools.h"
#include "../../../data/screendata.h"
#include "../../../data/viewdata.h"

// KDE
#include <KLocalizedString>

namespace NSE {
namespace Settings {
namespace View {
namespace Delegate {

NameDelegate::NameDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void NameDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(NSE::remainedFromScreenDrawing(option, false));
}

void NameDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem myOptions = option;
    //! Remove the focus dotted lines
    myOptions.state = (myOptions.state & ~QStyle::State_HasFocus);
    myOptions.text = index.model()->data(index, Qt::DisplayRole).toString();
    myOptions.displayAlignment = static_cast<Qt::Alignment>(index.model()->data(index, Qt::TextAlignmentRole).toInt());

    bool isEmpty = myOptions.text.isEmpty();
    bool isActive = index.data(Model::Views::ISACTIVEROLE).toBool();
    bool isMoveOrigin = index.data(Model::Views::ISMOVEORIGINROLE).toBool();
    bool isChanged = (index.data(Model::Views::ISCHANGEDROLE).toBool() || index.data(Model::Views::HASCHANGEDVIEWROLE).toBool());

    bool hasErrors = index.data(Model::Views::ERRORSROLE).toBool();
    bool hasWarnings = index.data(Model::Views::WARNINGSROLE).toBool();

    NSE::Data::Screen screen = index.data(Model::Views::SCREENROLE).value<NSE::Data::Screen>();
    NSE::Data::View view = index.data(Model::Views::VIEWROLE).value<NSE::Data::View>();

    float textopacity = 1.0;

    if (isEmpty) {
        myOptions.text = "&lt; " + i18n("optional") + " &gt;";
        textopacity = 0.5;
    }

    if (isActive) {
        myOptions.text = "<b>" + myOptions.text + "</b>";
    }

    if (isChanged || isMoveOrigin) {
        myOptions.text = "<i>" + myOptions.text + "</i>";
    }

    if (isMoveOrigin) {
        textopacity = 0.25;
    }

    NSE::drawBackground(painter, option);

    // draw changes indicator
    QRect remainedrect = NSE::remainedFromChangesIndicator(option);
    if (isChanged) {
        NSE::drawChangesIndicator(painter, option);
    }
    myOptions.rect = remainedrect;

    // draw errors/warnings
    if (hasErrors || hasWarnings) {
        remainedrect = NSE::remainedFromIcon(myOptions, Qt::AlignRight, -1, 2);
        if (hasErrors) {
            NSE::drawIcon(painter, myOptions, "data-error", Qt::AlignRight, -1, 2);
        } else if (hasWarnings) {
            NSE::drawIcon(painter, myOptions, "data-warning", Qt::AlignRight, -1, 2);
        }
        myOptions.rect = remainedrect;
    }

    // draw screen icon
    int maxiconsize = -1; //disabled
    remainedrect = NSE::remainedFromScreenDrawing(myOptions, screen.isScreensGroup(), maxiconsize);
    QRect availableScreenRect = NSE::drawScreen(painter, myOptions, screen.isScreensGroup(), screen.geometry, maxiconsize, textopacity);
    NSE::drawView(painter, myOptions, view, availableScreenRect, textopacity);

    myOptions.rect = remainedrect;
    NSE::drawFormattedText(painter, myOptions, textopacity);
}

}
}
}
}
