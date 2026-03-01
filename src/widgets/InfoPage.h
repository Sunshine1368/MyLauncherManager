#pragma once
#include <QWidget>
#include <QLabel>
#include "../core/DesktopFile.h"

class InfoPage : public QWidget {
    Q_OBJECT
public:
    explicit InfoPage(QWidget* parent = nullptr);
    void showFile(const DesktopFile& df);
    void clear();

private:
    QLabel* m_iconLabel;
    QLabel* m_nameLabel;
    QLabel* m_commentLabel;
    QLabel* m_execValue;
    QLabel* m_pathValue;
    QLabel* m_categoriesValue;
};