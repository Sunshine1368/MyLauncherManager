#include "AddPage.h"
#include "../utils/IconLoader.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>

AddPage::AddPage(QWidget* parent) : QWidget(parent) {
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* content = new QWidget;
    auto* vl = new QVBoxLayout(content);
    vl->setContentsMargins(24, 24, 24, 24);
    vl->setSpacing(16);

    // 图标预览
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
    connect(chooseIconBtn, &QPushButton::clicked, this, &AddPage::onChooseIcon);

    vl->addLayout(iconRow);

    // 表单
    auto* group = new QGroupBox("新建应用属性");
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
    m_categoriesEdit->setPlaceholderText("如：Utility;GTK;");

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

    auto* resetBtn = new QPushButton("重置");
    resetBtn->setFixedWidth(90);

    auto* addBtn = new QPushButton("创建");
    addBtn->setObjectName("PrimaryButton");
    addBtn->setFixedWidth(90);

    btnRow->addWidget(resetBtn);
    btnRow->addWidget(addBtn);

    connect(addBtn,   &QPushButton::clicked, this, &AddPage::onAdd);
    connect(resetBtn, &QPushButton::clicked, this, &AddPage::onReset);

    vl->addLayout(btnRow);
    vl->addStretch();

    scroll->setWidget(content);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(scroll);
}

void AddPage::onAdd() {
    QString name = m_nameEdit->text().trimmed();
    QString exec = m_execEdit->text().trimmed();

    if (name.isEmpty() || exec.isEmpty()) {
        QMessageBox::warning(this, "提示", "名称和启动命令为必填项");
        return;
    }

    DesktopFile df;
    df.type        = "Application";
    df.name        = name;
    df.genericName = m_genericNameEdit->text().trimmed();
    df.comment     = m_commentEdit->text().trimmed();
    df.exec        = exec;
    df.icon        = m_iconEdit->text().trimmed();
    df.categories  = m_categoriesEdit->text().trimmed();

    // 生成文件路径：放到当前用户目录
    QString localDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                       + "/.local/share/applications";
    QDir().mkpath(localDir);

    // 文件名：name 转小写去空格
    QString fname = name.toLower().replace(' ', '-') + ".desktop";
    df.filePath   = localDir + "/" + fname;

    // 加载图标以便立即显示
    if (!df.icon.isEmpty())
        df.resolvedIcon = IconLoader::load(df.icon);

    emit addRequested(df);
    onReset();
    QMessageBox::information(this, "成功",
        QString("已创建 %1\n路径：%2").arg(name, df.filePath));
}

void AddPage::onReset() {
    m_nameEdit->clear();
    m_genericNameEdit->clear();
    m_commentEdit->clear();
    m_execEdit->clear();
    m_iconEdit->clear();
    m_categoriesEdit->clear();
    m_iconPreview->clear();
}

void AddPage::onChooseIcon() {
    QString path = QFileDialog::getOpenFileName(this, "选择图标",
        "/usr/share/pixmaps",
        "图像文件 (*.png *.svg *.xpm *.ico)");
    if (!path.isEmpty())
        m_iconEdit->setText(path);
}