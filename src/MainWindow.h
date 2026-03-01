#pragma once
#include <QMainWindow>
#include <QSplitter>
#include <QAction>
#include <QToolBar>
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
    void onUninstallRequested(const QString& filePath);
    void onAddRequested(const DesktopFile& df);
    void onFilesChanged();
    void toggleDarkMode();

private:
    void applyTheme();

    DesktopFileManager* m_manager;
    LeftPanel*          m_leftPanel;
    RightPanel*         m_rightPanel;
    QSplitter*          m_splitter;
    QAction*            m_darkModeAction;
    bool                m_darkMode = false;

    DesktopFile         m_currentFile;
};
