#include "LeftPanel.h"
#include "delegates/DesktopFileDelegate.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QItemSelectionModel>

LeftPanel::LeftPanel(QWidget* parent) : QWidget(parent) {
    setObjectName("LeftPanel");
    setMinimumWidth(240);
    setMaximumWidth(340);

    auto* vl = new QVBoxLayout(this);
    vl->setContentsMargins(8, 12, 8, 8);
    vl->setSpacing(8);

    // 范围选择
    m_scopeCombo = new QComboBox;
    m_scopeCombo->setObjectName("ScopeCombo");
    m_scopeCombo->addItem("🌐  全体用户");
    m_scopeCombo->addItem("👤  当前用户");
    vl->addWidget(m_scopeCombo);

    // 搜索框
    m_searchBar = new QLineEdit;
    m_searchBar->setObjectName("SearchBar");
    m_searchBar->setPlaceholderText("🔍  搜索应用…");
    m_searchBar->setClearButtonEnabled(true);
    vl->addWidget(m_searchBar);

    // 列表
    m_model = new DesktopFileModel(this);
    m_listView = new QListView;
    m_listView->setObjectName("AppListView");
    m_listView->setModel(m_model);
    m_listView->setItemDelegate(new DesktopFileDelegate(this));
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setMouseTracking(true);
    m_listView->setUniformItemSizes(true);
    m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vl->addWidget(m_listView, 1);

    // 连接
    connect(m_scopeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LeftPanel::scopeChanged);

    connect(m_searchBar, &QLineEdit::textChanged,
            m_model, &DesktopFileModel::setFilter);

    connect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [this](const QModelIndex& idx, const QModelIndex&){
        if (!idx.isValid()) return;
        emit selectionChanged(m_model->fileAt(idx.row()));
    });
}

void LeftPanel::setFiles(const QList<DesktopFile>& files) {
    m_model->setFiles(files);
}