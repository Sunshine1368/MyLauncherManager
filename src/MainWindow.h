#pragma once
#include <QMainWindow>
#include <QSplitter>
#include <QAction>
#include <QToolBar>
#include <QToolButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
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
    void onRefresh();

private:
    void applyTheme();
    void startRefreshAnimation();
    void stopRefreshAnimation();

    DesktopFileManager* m_manager;
    LeftPanel*          m_leftPanel;
    RightPanel*         m_rightPanel;
    QSplitter*          m_splitter;
    QAction*            m_darkModeAction;
    QToolButton*        m_refreshBtn;
    QToolButton*        m_darkModeBtn;
    bool                m_darkMode = false;

    // 刷新旋转动画
    QPropertyAnimation* m_rotateAnim;
    int                 m_rotateAngle = 0;

    // 主题切换淡入淡出
    QGraphicsOpacityEffect* m_opacityEffect;
    QPropertyAnimation*     m_fadeAnim;

    DesktopFile m_currentFile;
};
