#include "IconLoader.h"
#include <QFile>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QDir>

QIcon IconLoader::load(const QString& iconName, int /*size*/) {
    if (iconName.isEmpty()) return placeholder();

    // 1. 绝对路径
    if (iconName.startsWith('/') && QFile::exists(iconName)) {
        QIcon ic(iconName);
        if (!ic.isNull()) return ic;
    }

    // 2. 相对路径（含扩展名）
    if (iconName.contains('/') || iconName.contains('.')) {
        QIcon ic(iconName);
        if (!ic.isNull()) return ic;
    }

    // 3. 主题图标
    if (QIcon::hasThemeIcon(iconName)) {
        return QIcon::fromTheme(iconName);
    }

    // 4. 在常见目录中搜索文件（去掉扩展名后匹配）
    static const QStringList searchDirs = {
        "/usr/share/pixmaps",
        "/usr/share/icons/hicolor/48x48/apps",
        "/usr/share/icons/hicolor/64x64/apps",
        "/usr/share/icons/hicolor/128x128/apps",
        "/usr/share/icons/hicolor/256x256/apps",
        "/usr/share/icons/hicolor/scalable/apps",
    };
    static const QStringList exts = {".png", ".svg", ".xpm"};

    for (const QString& dir : searchDirs) {
        for (const QString& ext : exts) {
            QString candidate = dir + "/" + iconName + ext;
            if (QFile::exists(candidate)) {
                QIcon ic(candidate);
                if (!ic.isNull()) return ic;
            }
        }
    }

    // 5. 尝试 fromTheme 带回退
    QIcon ic = QIcon::fromTheme(iconName, placeholder());
    return ic;
}

QIcon IconLoader::placeholder() {
    // 生成一个简单的占位图
    static QIcon ph;
    if (ph.isNull()) {
        QPixmap pm(48, 48);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(QColor("#e0e0e5"));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(pm.rect(), 10, 10);
        p.setPen(QColor("#aeaeb2"));
        p.setFont(QFont("Arial", 20, QFont::Bold));
        p.drawText(pm.rect(), Qt::AlignCenter, "?");
        ph = QIcon(pm);
    }
    return ph;
}