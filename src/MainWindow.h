#pragma once
#include <QMainWindow>
#include <QSplitter>
#include "LeftPanel.h"
#include "RightPanel.h"
#include "core/DesktopFileManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onScopeChanged(int index);
    void onSelectionChanged(const DesktopFile& df);
    void onSaveRequested(const DesktopFile& df);
    void onDeleteRequested(const QString& filePath);
    void onAddRequested(const DesktopFile& df);
    void onFilesChanged();

private:
    DesktopFileManager* m_manager;
    LeftPanel*          m_leftPanel;
    RightPanel*         m_rightPanel;
    QSplitter*          m_splitter;

    DesktopFile         m_currentFile;
};