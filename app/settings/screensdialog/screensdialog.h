/*
    SPDX-FileCopyrightText: 2021 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SCREENSDIALOG_H
#define SCREENSDIALOG_H

// local
#include "../generic/genericdialog.h"
#include "../settingsdialog/settingsdialog.h"

// Qt
#include <QDialog>
#include <QObject>
#include <QPushButton>
#include <QWindow>

namespace Ui {
class ScreensDialog;
}

namespace NSE {
namespace Settings {
namespace Controller {
class Layouts;
}
namespace Handler {
class ScreensHandler;
}
}
}


namespace NSE {
namespace Settings {
namespace Dialog {

class ScreensDialog : public GenericDialog
{
    Q_OBJECT

public:
    ScreensDialog(SettingsDialog *parent, Controller::Layouts *controller);
    ~ScreensDialog();

    Ui::ScreensDialog *ui() const;
    NSE::Corona *corona() const;
    Controller::Layouts *layoutsController() const;

    QPushButton *removeNowButton() const;

protected:
    void accept() override;

private slots:
    void onCancel();
    void onDataChanged();
    void onReset();

    void initButtons();
    void initRemoveNowButton();
    void initSignals();

private:
    void init();

private:
    QPushButton *m_removeNowButton{nullptr};

    NSE::Corona *m_corona{nullptr};

    Ui::ScreensDialog *m_ui;
    Controller::Layouts *m_layoutsController{nullptr};

    Handler::ScreensHandler *m_handler;
};

}
}
}

#endif
