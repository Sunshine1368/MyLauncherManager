#include "InfoPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QScrollArea>

InfoPage::InfoPage(QWidget* parent) : QWidget(parent) {
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* content = new QWidget;
    auto* vl = new QVBoxLayout(content);
    vl->setContentsMargins(24, 24, 24, 24);
    vl->setSpacing(16);

    // 顶部：图标 + 名称 + 简介
    auto* topLayout = new QHBoxLayout;
    topLayout->setSpacing(18);

    m_iconLabel = new QLabel;
    m_iconLabel->setFixedSize(72, 72);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    topLayout->addWidget(m_iconLabel, 0, Qt::AlignTop);

    auto* nameBlock = new QVBoxLayout;
    nameBlock->setSpacing(4);
    m_nameLabel = new QLabel;
    m_nameLabel->setObjectName("AppNameLabel");
    m_nameLabel->setWordWrap(true);

    m_commentLabel = new QLabel;
    m_commentLabel->setObjectName("AppCommentLabel");
    m_commentLabel->setWordWrap(true);

    nameBlock->addWidget(m_nameLabel);
    nameBlock->addWidget(m_commentLabel);
    nameBlock->addStretch();
    topLayout->addLayout(nameBlock, 1);

    vl->addLayout(topLayout);

    // 详情 GroupBox
    auto* group = new QGroupBox("详细信息");
    auto* form  = new QFormLayout(group);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setHorizontalSpacing(14);
    form->setVerticalSpacing(10);

    auto makeValue = [&]() {
        auto* l = new QLabel;
        l->setWordWrap(true);
        l->setTextInteractionFlags(Qt::TextSelectableByMouse);
        return l;
    };

    m_execValue       = makeValue();
    m_pathValue       = makeValue();
    m_categoriesValue = makeValue();

    auto makeSection = [](const QString& t) {
        auto* l = new QLabel(t);
        l->setObjectName("SectionLabel");
        return l;
    };

    form->addRow(makeSection("启动命令"), m_execValue);
    form->addRow(makeSection("文件路径"), m_pathValue);
    form->addRow(makeSection("分类"),     m_categoriesValue);

    vl->addWidget(group);
    vl->addStretch();

    scroll->setWidget(content);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(scroll);
}

void InfoPage::showFile(const DesktopFile& df) {
    m_iconLabel->setPixmap(df.resolvedIcon.pixmap(72, 72));
    m_nameLabel->setText(df.name);
    m_commentLabel->setText(df.comment.isEmpty() ? df.genericName : df.comment);
    m_execValue->setText(df.exec);
    m_pathValue->setText(df.filePath);
    m_categoriesValue->setText(df.categories);
}

void InfoPage::clear() {
    m_iconLabel->clear();
    m_nameLabel->clear();
    m_commentLabel->clear();
    m_execValue->clear();
    m_pathValue->clear();
    m_categoriesValue->clear();
}