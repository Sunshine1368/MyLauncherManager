#pragma once
#include <QString>
#include <QIcon>

struct DesktopFile {
    QString filePath;       // 完整路径
    QString name;           // Name=
    QString genericName;    // GenericName=
    QString comment;        // Comment=
    QString exec;           // Exec=
    QString icon;           // Icon=（主题名或绝对路径）
    QString categories;     // Categories=
    QString type;           // Type=
    bool    noDisplay = false;
    bool    hidden    = false;

    QIcon   resolvedIcon;   // 已加载好的图标（运行时填充）

    bool isValid() const {
        return !filePath.isEmpty() && !resolvedIcon.isNull();
    }
};