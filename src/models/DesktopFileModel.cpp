#include "DesktopFileModel.h"

DesktopFileModel::DesktopFileModel(QObject* parent)
    : QAbstractListModel(parent)
{}

void DesktopFileModel::setFiles(const QList<DesktopFile>& files) {
    beginResetModel();
    m_all = files;
    applyFilter();
    endResetModel();
}

void DesktopFileModel::setFilter(const QString& text) {
    m_filter = text.toLower();
    beginResetModel();
    applyFilter();
    endResetModel();
}

void DesktopFileModel::applyFilter() {
    if (m_filter.isEmpty()) {
        m_filtered = m_all;
        return;
    }
    m_filtered.clear();
    for (const auto& df : m_all) {
        if (df.name.toLower().contains(m_filter) ||
            df.comment.toLower().contains(m_filter) ||
            df.genericName.toLower().contains(m_filter)) {
            m_filtered.append(df);
        }
    }
}

int DesktopFileModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return m_filtered.size();
}

QVariant DesktopFileModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_filtered.size())
        return {};

    const DesktopFile& df = m_filtered.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
        case NameRole:       return df.name;
        case CommentRole:    return df.comment.isEmpty() ? df.genericName : df.comment;
        case Qt::DecorationRole:
        case IconRole:       return df.resolvedIcon;
        case FilePathRole:   return df.filePath;
        case ExecRole:       return df.exec;
        case GenericNameRole:return df.genericName;
        case CategoriesRole: return df.categories;
        case IconNameRole:   return df.icon;
    }
    return {};
}

const DesktopFile& DesktopFileModel::fileAt(int row) const {
    return m_filtered.at(row);
}
