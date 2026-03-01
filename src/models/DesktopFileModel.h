#pragma once
#include <QAbstractListModel>
#include "../core/DesktopFile.h"

class DesktopFileModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        CommentRole,
        IconRole,
        FilePathRole,
        ExecRole,
        GenericNameRole,
        CategoriesRole,
        IconNameRole,
    };

    explicit DesktopFileModel(QObject* parent = nullptr);

    void setFiles(const QList<DesktopFile>& files);
    void setFilter(const QString& text);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    const DesktopFile& fileAt(int row) const;

private:
    QList<DesktopFile> m_all;
    QList<DesktopFile> m_filtered;
    QString            m_filter;

    void applyFilter();
};