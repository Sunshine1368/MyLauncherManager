#include "EditPage.h"
#include "../utils/IconLoader.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QFileDialog>
#include <QMessageBox>

EditPage::EditPage(QWidget* parent) : QWidget(parent) {
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* content = new QWidget;
    auto* vl = new QVBoxLayout(content);
    vl->setContentsMargins(24, 24, 24, 24);
    vl->setSpacing(16);

    // 图标预览 + 选择
    auto* iconRow = new QHBoxLayout;
    m_iconPreview = new QLabel;
    m_iconPreview->setFixedSize(64, 64);
    m_iconPreview->setAlignment(Qt::AlignCenter);
    m_iconPreview->setStyleSheet(
        "QLabel { background:#f0f0f5; border-radius:12px; border:1px solid #d1d1d6; }");

    auto* chooseIconBtn = new QPushButton("选择图标文件…");
    iconRow->addWidget(m_iconPreview);
    iconRow->addWidget(chooseIconBtn);
    iconRow->addStretch();
    connect(chooseIconBtn, &QPushButton::clicked, this, &EditPage::onChooseIcon);

    vl->addLayout(iconRow);

    // 表单
    auto* group = new QGroupBox("编辑属性");
    auto* form  = new QFormLayout(group);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setHorizontalSpacing(14);
    form->setVerticalSpacing(10);

    auto makeLabel = [](const QString& t) {
        auto* l = new QLabel(t);
        l->setObjectName("SectionLabel");
        return l;
    };

    m_nameEdit        = new QLineEdit;
    m_genericNameEdit = new QLineEdit;
    m_commentEdit     = new QLineEdit;
    m_execEdit        = new QLineEdit;
    m_iconEdit        = new QLineEdit;
    m_categoriesEdit  = new QLineEdit;

    form->addRow(makeLabel("名称 *"),    m_nameEdit);
    form->addRow(makeLabel("通用名称"),   m_genericNameEdit);
    form->addRow(makeLabel("简介"),       m_commentEdit);
    form->addRow(makeLabel("启动命令 *"), m_execEdit);
    form->addRow(makeLabel("图标"),       m_iconEdit);
    form->addRow(makeLabel("分类"),       m_categoriesEdit);

    connect(m_iconEdit, &QLineEdit::textChanged, this, [this](const QString& t){
        QIcon ic = IconLoader::load(t);
        m_iconPreview->setPixmap(ic.pixmap(64,64));
    });

    vl->addWidget(group);

    // 按钮行
    auto* btnRow = new QHBoxLayout;
    btnRow->addStretch();

    auto* deleteBtn = new QPushButton("删除");
    deleteBtn->setObjectName("DangerButton");
    deleteBtn->setFixedWidth(90);

    auto* saveBtn = new QPushButton("保存");
    saveBtn->setObjectName("PrimaryButton");
    saveBtn->setFixedWidth(90);

    btnRow->addWidget(deleteBtn);
    btnRow->addWidget(saveBtn);

    connect(saveBtn,   &QPushButton::clicked, this, &EditPage::onSave);
    connect(deleteBtn, &QPushButton::clicked, this, &EditPage::onDelete);

    vl->addLayout(btnRow);
    vl->addStretch();

    scroll->setWidget(content);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(scroll);
}

void EditPage::loadFile(const DesktopFile& df) {
    m_current = df;
    m_nameEdit->setText(df.name);
    m_genericNameEdit->setText(df.genericName);
    m_commentEdit->setText(df.comment);
    m_execEdit->setText(df.exec);
    m_iconEdit->setText(df.icon);
    m_categoriesEdit->setText(df.categories);
    m_iconPreview->setPixmap(df.resolvedIcon.pixmap(64, 64));
}

void EditPage::clear() {
    m_current = {};
    m_nameEdit->clear();
    m_genericNameEdit->clear();
    m_commentEdit->clear();
    m_execEdit->clear();
    m_iconEdit->clear();
    m_categoriesEdit->clear();
    m_iconPreview->clear();
}

void EditPage::onSave() {
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "名称不能为空");
        return;
    }
    if (m_execEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "启动命令不能为空");
        return;
    }

    DesktopFile df = m_current;
    df.name        = m_nameEdit->text().trimmed();
    df.genericName = m_genericNameEdit->text().trimmed();
    df.comment     = m_commentEdit->text().trimmed();
    df.exec        = m_execEdit->text().trimmed();
    df.icon        = m_iconEdit->text().trimmed();
    df.categories  = m_categoriesEdit->text().trimmed();
    df.type        = "Application";

    emit saveRequested(df);
}

void EditPage::onDelete() {
    if (m_current.filePath.isEmpty()) return;
    auto ret = QMessageBox::question(this, "确认删除",
        QString("确定要删除 %1 的启动项吗？\n文件将被永久删除。").arg(m_current.name),
        QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
        emit deleteRequested(m_current.filePath);
}

void EditPage::onChooseIcon() {
    QString path = QFileDialog::getOpenFileName(this, "选择图标",
        "/usr/share/pixmaps",
        "图像文件 (*.png *.svg *.xpm *.ico)");
    if (!path.isEmpty()) {
        m_iconEdit->setText(path);
    }
}