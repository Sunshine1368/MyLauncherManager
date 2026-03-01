#include "RightPanel.h"
#include <QVBoxLayout>

RightPanel::RightPanel(QWidget* parent) : QWidget(parent) {
    setObjectName("RightPanel");

    auto* vl = new QVBoxLayout(this);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(0);

    // 导航 TabBar
    m_tabBar = new QTabBar;
    m_tabBar->setObjectName("NavTabBar");
    m_tabBar->addTab("  信息  ");
    m_tabBar->addTab("  编辑  ");
    m_tabBar->addTab("  新建  ");
    m_tabBar->setExpanding(false);
    m_tabBar->setDrawBase(false);

    vl->addWidget(m_tabBar);

    // 堆叠视图
    m_stack = new QStackedWidget;
    m_infoPage = new InfoPage;
    m_editPage = new EditPage;
    m_addPage  = new AddPage;

    m_stack->addWidget(m_infoPage);
    m_stack->addWidget(m_editPage);
    m_stack->addWidget(m_addPage);

    vl->addWidget(m_stack, 1);

    connect(m_tabBar, &QTabBar::currentChanged,
            m_stack,  &QStackedWidget::setCurrentIndex);
}

void RightPanel::showFile(const DesktopFile& df) {
    m_infoPage->showFile(df);
    m_editPage->loadFile(df);
}