#include "MainWindow.h"
#include <QHBoxLayout>
#include <QStatusBar>
#include <QMessageBox>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("LaunchPad Manager");
    setMinimumSize(860, 580);
    resize(1100, 680);

    // macOS 风格：无边框 + 圆角（Linux 下效果有限，但尽量接近）
    setAttribute(Qt::WA_TranslucentBackground, false);

    m_manager = new DesktopFileManager(this);

    // 中央 splitter
    m_splitter = new QSplitter(Qt::Horizontal);
    m_splitter->setHandleWidth(1);

    m_leftPanel  = new LeftPanel;
    m_rightPanel = new RightPanel;

    m_splitter->addWidget(m_leftPanel);
    m_splitter->addWidget(m_rightPanel);
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setSizes({270, 800});

    setCentralWidget(m_splitter);

    // 状态栏
    statusBar()->showMessage("就绪");

    // 信号连接
    connect(m_leftPanel,  &LeftPanel::scopeChanged,
            this,         &MainWindow::onScopeChanged);
    connect(m_leftPanel,  &LeftPanel::selectionChanged,
            this,         &MainWindow::onSelectionChanged);

    connect(m_rightPanel->editPage(), &EditPage::saveRequested,
            this,                     &MainWindow::onSaveRequested);
    connect(m_rightPanel->editPage(), &EditPage::deleteRequested,
            this,                     &MainWindow::onDeleteRequested);
    connect(m_rightPanel->addPage(),  &AddPage::addRequested,
            this,                     &MainWindow::onAddRequested);

    connect(m_manager, &DesktopFileManager::filesChanged,
            this,      &MainWindow::onFilesChanged);

    // 初始加载
    m_manager->refresh();
}

void MainWindow::onScopeChanged(int index) {
    m_manager->setScope(index == 0
        ? DesktopFileManager::Scope::AllUsers
        : DesktopFileManager::Scope::CurrentUser);
}

void MainWindow::onSelectionChanged(const DesktopFile& df) {
    m_currentFile = df;
    m_rightPanel->showFile(df);
    statusBar()->showMessage(df.filePath);
}

void MainWindow::onSaveRequested(const DesktopFile& df) {
    if (m_manager->updateFile(df)) {
        statusBar()->showMessage("已保存：" + df.filePath, 4000);
    } else {
        QMessageBox::critical(this, "错误", "保存失败，请检查文件权限。");
    }
}

void MainWindow::onDeleteRequested(const QString& filePath) {
    if (m_manager->removeFile(filePath)) {
        m_rightPanel->infoPage()->clear();
        m_rightPanel->editPage()->clear();
        statusBar()->showMessage("已删除：" + filePath, 4000);
    } else {
        QMessageBox::critical(this, "错误", "删除失败，请检查文件权限。");
    }
}

void MainWindow::onAddRequested(const DesktopFile& df) {
    if (!m_manager->addFile(df)) {
        QMessageBox::critical(this, "错误", "创建失败，请检查写入权限。");
    } else {
        statusBar()->showMessage("已创建：" + df.filePath, 4000);
    }
}

void MainWindow::onFilesChanged() {
    m_leftPanel->setFiles(m_manager->files());
    statusBar()->showMessage(
        QString("共 %1 个应用").arg(m_manager->files().size()), 3000);
}