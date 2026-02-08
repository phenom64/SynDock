/*
    SPDX-FileCopyrightText: 2021 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef VIEWSDIALOGHANDLER_H
#define VIEWSDIALOGHANDLER_H

// local
#include "../generic/generichandler.h"
#include "../../data/layoutdata.h"
#include "../../layout/abstractlayout.h"

// Qt
#include <QAction>
#include <QButtonGroup>
#include <QMenu>
#include <QSortFilterProxyModel>

// KDE
#include <KMessageBox>

namespace Ui {
class ViewsDialog;
}

namespace NSE{
class CentralLayout;
class Corona;
namespace Settings{
namespace Controller{
class Layouts;
class Views;
}
namespace Dialog{
class ViewsDialog;
}
}
}


namespace NSE {
namespace Settings {
namespace Handler {

//! Handlers are objects to handle the UI elements that semantically associate with specific
//! ui::tabs or different windows. They are responsible also to handle the user interaction
//! between controllers and views

class ViewsHandler : public Generic
{
    Q_OBJECT
public:
    ViewsHandler(Dialog::ViewsDialog *dialog);
    ~ViewsHandler();

    bool hasChangedData() const override;
    bool inDefaultValues() const override;

    bool isSelectedLayoutOriginal() const;

    NSE::Data::Layout currentData() const;
    NSE::Data::Layout originalData() const;

    Ui::ViewsDialog *ui() const;
    NSE::Corona *corona() const;
    Settings::Controller::Layouts *layoutsController() const;

public slots:
    void reset() override;
    void resetDefaults() override;
    void save() override;

signals:
    void currentLayoutChanged();

private slots:
    void initViewTemplatesSubMenu();
    void initViewExportSubMenu();
    void removeSelectedViews();
    void updateWindowTitle();

    void exportViewForBackup();
    void exportViewAsTemplate();
    void importView();

    void onCurrentLayoutIndexChanged(int row);
    void onSelectionChanged();

    void newView(const Data::Generic &templateData);

private:
    void init();

    void reload();

    void loadLayout(const NSE::Data::Layout &data);

    QString storedView(const QString &viewId);

    KMessageBox::ButtonCode saveChangesConfirmation();
    KMessageBox::ButtonCode removalConfirmation(const int &count);

private:
    Dialog::ViewsDialog *m_dialog{nullptr};
    Ui::ViewsDialog *m_ui{nullptr};
    Settings::Controller::Views *m_viewsController{nullptr};

    QSortFilterProxyModel *m_layoutsProxyModel{nullptr};

    NSE::Data::Layout o_data;

    int m_lastConfirmedLayoutIndex{-1};

    //! Actions
    QAction *m_newViewAction{nullptr};
    QAction *m_duplicateViewAction{nullptr};
    QAction *m_removeViewAction{nullptr};
    QAction *m_exportViewAction{nullptr};
    QAction *m_importViewAction{nullptr};

    //! Menus
    QMenu *m_viewTemplatesSubMenu{nullptr};
    QMenu *m_viewExportSubMenu{nullptr};
};

}
}
}

#endif
