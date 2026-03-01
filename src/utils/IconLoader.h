#pragma once
#include <QIcon>
#include <QString>

class IconLoader {
public:
    // 按优先级：绝对路径 → 主题图标 → 回退占位
    static QIcon load(const QString& iconName, int size = 48);

    // 返回占位图标
    static QIcon placeholder();
};