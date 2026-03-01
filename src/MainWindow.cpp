#include "MainWindow.h"
#include <QHBoxLayout>
#include <QStatusBar>
#include <QMessageBox>
#include <QApplication>
#include <QToolBar>
#include <QStyle>
#include <QFile>
#include <QProcess>
#include <QFileInfo>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("LaunchPad Manager");
    setMinimumSize(860, 580);
    resize(1100, 680);

    m_manager = new DesktopFileManager(this);

    // 工具栏（顶部，含深色模式切换）
    QToolBar* toolbar = new QToolBar("工具栏", this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setIconSize(QSize(18, 18));
    toolbar->setObjectName("MainToolBar");
    addToolBar(Qt::TopToolBarArea, toolbar);

    // 弹簧：把按钮推到右边
    QWidget* spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);

    m_darkModeAction = new QAction("🌙  深色模式", this);
    m_darkModeAction->setCheckable(true);
    toolbar->addAction(m_darkModeAction);
    connect(m_darkModeAction, &QAction::triggered, this, &MainWindow::toggleDarkMode);

    // Splitter
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
    statusBar()->showMessage("就绪");

    connect(m_leftPanel,  &LeftPanel::scopeChanged,
            this,         &MainWindow::onScopeChanged);
    connect(m_leftPanel,  &LeftPanel::selectionChanged,
            this,         &MainWindow::onSelectionChanged);

    connect(m_rightPanel->editPage(), &EditPage::saveRequested,
            this,                     &MainWindow::onSaveRequested);
    connect(m_rightPanel->editPage(), &EditPage::deleteRequested,
            this,                     &MainWindow::onDeleteRequested);
    connect(m_rightPanel->editPage(), &EditPage::uninstallRequested,
            this,                     &MainWindow::onUninstallRequested);
    connect(m_rightPanel->addPage(),  &AddPage::addRequested,
            this,                     &MainWindow::onAddRequested);

    connect(m_manager, &DesktopFileManager::filesChanged,
            this,      &MainWindow::onFilesChanged);

    applyTheme();
    m_manager->refresh();
}

void MainWindow::toggleDarkMode() {
    m_darkMode = m_darkModeAction->isChecked();
    m_darkModeAction->setText(m_darkMode ? "☀️  浅色模式" : "🌙  深色模式");
    applyTheme();
}

void MainWindow::applyTheme() {
    QString qssFile = m_darkMode ? ":/styles/macos11-dark.qss" : ":/styles/macos11.qss";
    QFile f(qssFile);
    if (f.open(QFile::ReadOnly)) {
        qApp->setStyleSheet(f.readAll());
        f.close();
    }
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
    if (m_manager->updateFile(df))
        statusBar()->showMessage("已保存：" + df.filePath, 4000);
    else
        QMessageBox::critical(this, "错误", "保存失败，请检查文件权限。");
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

void MainWindow::onUninstallRequested(const QString& filePath) {
    // 读取 desktop 文件拿到包名（文件名去掉.desktop）
    QFileInfo fi(filePath);
    QString appName = fi.baseName();

    auto ret = QMessageBox::question(this, "卸载应用",
        QString("确定要卸载 %1 吗？\n\n这将尝试使用包管理器卸载该软件，并删除其启动项。\n此操作不可逆！")
            .arg(appName),
        QMessageBox::Yes | QMessageBox::No);

    if (ret != QMessageBox::Yes) return;

    // 先删除 desktop 文件
    m_manager->removeFile(filePath);

    // 尝试用 apt / flatpak / snap 卸载
    // 策略：先尝试 apt，再尝试 flatpak，再尝试 snap
    QStringList cmds = {
        QString("pkexec apt-get remove -y %1").arg(appName),
        QString("flatpak uninstall -y --noninteractive %1").arg(appName),
        QString("snap remove %1").arg(appName),
    };

    bool tried = false;
    for (const QString& cmd : cmds) {
        QStringList args = cmd.split(' ');
        QString prog = args.takeFirst();
        QProcess proc;
        proc.start(prog, args);
        if (proc.waitForStarted(2000)) {
            tried = true;
            proc.waitForFinished(30000);
            int code = proc.exitCode();
            if (code == 0) {
                QMessageBox::information(this, "卸载成功",
                    QString("%1 已成功卸载。").arg(appName));
                m_rightPanel->infoPage()->clear();
                m_rightPanel->editPage()->clear();
                return;
            }
        }
    }

    if (!tried) {
        QMessageBox::warning(this, "卸载",
            "已删除启动项，但无法自动调用包管理器卸载程序本体。\n"
            "请手动使用 apt / flatpak / snap 卸载。");
    } else {
        QMessageBox::warning(this, "卸载",
            QString("已删除启动项，但包管理器卸载 %1 失败。\n"
                    "可能该应用不是通过包管理器安装的，请手动卸载。").arg(appName));
    }
}

void MainWindow::onAddRequested(const DesktopFile& df) {
    if (!m_manager->addFile(df))
        QMessageBox::critical(this, "错误", "创建失败，请检查写入权限。");
    else
        statusBar()->showMessage("已创建：" + df.filePath, 4000);
}

void MainWindow::onFilesChanged() {
    m_leftPanel->setFiles(m_manager->files());
    statusBar()->showMessage(
        QString("共 %1 个应用").arg(m_manager->files().size()), 3000);
}
