#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QX11Info>
#include <QIcon>
#include <QPushButton>

#include <KF5/KWindowSystem/KWindowSystem>
#include <KF5/KWindowSystem/NETWM>
#include <KF5/KWindowSystem/KWindowInfo>

#include "winctrl.h"
#include "model.h"

void toggleMaximize()
{
    NETWinInfo info(QX11Info::connection(), KWindowSystem::activeWindow(), QX11Info::appRootWindow(), NET::WMState, 0);
    if(info.state() & NET::Max)
        KWindowSystem::clearState(KWindowSystem::activeWindow(), NET::Max);
    else
        KWindowSystem::setState(KWindowSystem::activeWindow(), NET::Max);
};

// ----------

Q::WinTitle::WinTitle(QWidget *parent) : QLabel(parent)
{
};

void Q::WinTitle::mouseDoubleClickEvent(QMouseEvent *)
{
    toggleMaximize();
};

// ----------

Q::WinCtrl::WinCtrl(const QString& name, Shell* shell) : QWidget(), Model(name, shell)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
    
    closeBtn = new QPushButton();
    closeBtn->setIcon(QIcon::fromTheme("window-close"));
    boxLayout()->addWidget(closeBtn);
    
    minimizeBtn = new QPushButton();
    minimizeBtn->setIcon(QIcon::fromTheme("window-minimize"));
    boxLayout()->addWidget(minimizeBtn);
    
    maximizeBtn = new QPushButton();
    maximizeBtn->setIcon(QIcon::fromTheme("window-restore"));
    boxLayout()->addWidget(maximizeBtn);
    
    label = new WinTitle();
    boxLayout()->addWidget(label);
    update();
    
    connect(KWindowSystem::self(), SIGNAL(activeWindowChanged(WId)), this, SLOT(update(WId)));
    connect(KWindowSystem::self(), SIGNAL(windowChanged(WId, const unsigned long*)), this, SLOT(update()));
    connect(closeBtn, &QPushButton::clicked, [this]() {
        NETRootInfo(QX11Info::connection(), NET::CloseWindow).closeWindowRequest(KWindowSystem::activeWindow());
    });
    connect(minimizeBtn, &QPushButton::clicked, [this]() {
        KWindowSystem::minimizeWindow(KWindowSystem::activeWindow());
        update();
    });
    connect(maximizeBtn, &QPushButton::clicked, []() {
        toggleMaximize();
    });
};

void Q::WinCtrl::update(WId wid)
{
    KWindowInfo info(wid, NET::WMName|NET::WMState);
    label->setText(info.name());
    if(info.state() & NET::Max)
    {
        closeBtn->show();
        minimizeBtn->show();
        maximizeBtn->show();
    }
    else
    {
        closeBtn->hide();
        minimizeBtn->hide();
        maximizeBtn->hide();
    }
};
