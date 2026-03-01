#pragma once
#include "DesktopFile.h"
#include <QObject>
#include <QList>
#include <QString>

class DesktopFileManager : public QObject {
    Q_OBJECT
public:
    enum class Scope { AllUsers, CurrentUser };

    explicit DesktopFileManager(QObject* parent = nullptr);

    void setScope(Scope scope);
    Scope scope() const { return m_scope; }

    void refresh();

    const QList<DesktopFile>& files() const { return m_files; }

    // 增删改
    bool addFile(const DesktopFile& df);
    bool updateFile(const DesktopFile& df);   // 以 filePath 为 key
    bool removeFile(const QString& filePath);

signals:
    void filesChanged();

private:
    QStringList scanDirs() const;
    DesktopFile parseFile(const QString& path) const;

    Scope             m_scope = Scope::AllUsers;
    QList<DesktopFile> m_files;
};