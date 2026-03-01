#pragma once
#include <QWidget>
#include <QListView>
#include <QComboBox>
#include <QLineEdit>
#include "models/DesktopFileModel.h"
#include "core/DesktopFile.h"

class LeftPanel : public QWidget {
    Q_OBJECT
public:
    explicit LeftPanel(QWidget* parent = nullptr);

    void setFiles(const QList<DesktopFile>& files);

signals:
    void scopeChanged(int index);   // 0=全部, 1=当前用户
    void selectionChanged(const DesktopFile& df);

private:
    QComboBox*          m_scopeCombo;
    QLineEdit*          m_searchBar;
    QListView*          m_listView;
    DesktopFileModel*   m_model;
};