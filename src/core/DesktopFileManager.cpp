#include "DesktopFileManager.h"
#include "../utils/IconLoader.h"

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>

DesktopFileManager::DesktopFileManager(QObject* parent)
    : QObject(parent)
{}

void DesktopFileManager::setScope(Scope scope) {
    if (m_scope != scope) {
        m_scope = scope;
        refresh();
    }
}

QStringList DesktopFileManager::scanDirs() const {
    QStringList dirs;
    if (m_scope == Scope::AllUsers) {
        dirs << "/usr/share/applications"
             << "/usr/local/share/applications";
        QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        dirs << home + "/.local/share/applications";
    } else {
        QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        dirs << home + "/.local/share/applications";
    }
    return dirs;
}

DesktopFile DesktopFileManager::parseFile(const QString& path) const {
    DesktopFile df;
    df.filePath = path;

    QSettings ini(path, QSettings::IniFormat);
    ini.beginGroup("Desktop Entry");

    df.type        = ini.value("Type", "Application").toString();
    df.name        = ini.value("Name").toString();
    df.genericName = ini.value("GenericName").toString();
    df.comment     = ini.value("Comment").toString();
    df.exec        = ini.value("Exec").toString();
    df.icon        = ini.value("Icon").toString();
    df.categories  = ini.value("Categories").toString();
    df.noDisplay   = ini.value("NoDisplay", false).toBool();
    df.hidden      = ini.value("Hidden", false).toBool();

    ini.endGroup();

    if (df.type != "Application" || df.hidden || df.noDisplay || df.name.isEmpty()) {
        return {};
    }

    if (!df.icon.isEmpty()) {
        df.resolvedIcon = IconLoader::load(df.icon);
    }

    return df;
}

void DesktopFileManager::refresh() {
    m_files.clear();

    QSet<QString> seen;
    for (const QString& dirPath : scanDirs()) {
        QDir dir(dirPath);
        if (!dir.exists()) continue;

        const auto entries = dir.entryInfoList({"*.desktop"}, QDir::Files);
        for (const QFileInfo& fi : entries) {
            QString path = fi.absoluteFilePath();
            if (seen.contains(path)) continue;
            seen.insert(path);

            DesktopFile df = parseFile(path);
            if (df.isValid()) {
                m_files.append(df);
            }
        }
    }

    std::sort(m_files.begin(), m_files.end(), [](const DesktopFile& a, const DesktopFile& b){
        return a.name.toLower() < b.name.toLower();
    });

    emit filesChanged();
}

static void writeDesktopFile(const DesktopFile& df) {
    QSettings ini(df.filePath, QSettings::IniFormat);
    ini.beginGroup("Desktop Entry");
    ini.setValue("Type",        df.type.isEmpty() ? "Application" : df.type);
    ini.setValue("Name",        df.name);
    ini.setValue("GenericName", df.genericName);
    ini.setValue("Comment",     df.comment);
    ini.setValue("Exec",        df.exec);
    ini.setValue("Icon",        df.icon);
    ini.setValue("Categories",  df.categories);
    ini.setValue("NoDisplay",   df.noDisplay);
    ini.setValue("Hidden",      df.hidden);
    ini.endGroup();
    ini.sync();
}

bool DesktopFileManager::addFile(const DesktopFile& df) {
    if (df.filePath.isEmpty()) return false;
    QFileInfo fi(df.filePath);
    QDir().mkpath(fi.absolutePath());
    writeDesktopFile(df);
    refresh();
    return true;
}

bool DesktopFileManager::updateFile(const DesktopFile& df) {
    if (df.filePath.isEmpty()) return false;
    writeDesktopFile(df);
    refresh();
    return true;
}

bool DesktopFileManager::removeFile(const QString& filePath) {
    if (!QFile::remove(filePath)) return false;
    refresh();
    return true;
}
