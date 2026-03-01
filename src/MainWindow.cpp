#include "MainWindow.h"
#include <QHBoxLayout>
#include <QStatusBar>
#include <QMessageBox>
#include <QApplication>
#include <QToolBar>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTimer>
#include <QPainter>
#include <QPixmap>
#include <QTransform>
#include <QSequentialAnimationGroup>
#include <QVariantAnimation>

// ── 带旋转动画的刷新按钮 ──────────────────────────────────────────
class RefreshButton : public QToolButton {
    Q_OBJECT
    Q_PROPERTY(int angle READ angle WRITE setAngle)
public:
    explicit RefreshButton(QWidget* parent = nullptr) : QToolButton(parent) {
        setFixedSize(32, 32);
        setText("");
    }

    int angle() const { return m_angle; }
    void setAngle(int a) {
        m_angle = a % 360;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // 背景
        bool hov = underMouse();
        p.setBrush(hov ? QColor(0,113,227,30) : Qt::transparent);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(rect(), 8, 8);

        // 旋转绘制刷新图标
        p.translate(width()/2.0, height()/2.0);
        p.rotate(m_angle);

        int r = 10;
        QPen pen(QColor("#0071e3"), 2.2, Qt::SolidLine, Qt::RoundCap);
        p.setPen(pen);
        // 圆弧（270度）
        p.drawArc(-r, -r, 2*r, 2*r, 100*16, 260*16);

        // 箭头头部
        p.setBrush(QColor("#0071e3"));
        p.setPen(Qt::NoPen);
        QPolygonF arrow;
        // 箭头在顶部约 80 度位置
        double rad = qDegreesToRadians(-10.0);
        double cx = r * qCos(rad);
        double cy = -r * qSin(rad);
        arrow << QPointF(cx, cy)
              << QPointF(cx - 5, cy - 3)
              << QPointF(cx + 1, cy - 6);
        p.drawPolygon(arrow);
    }

private:
    int m_angle = 0;
};

// ── 深色模式切换按钮 ──────────────────────────────────────────────
class ThemeButton : public QToolButton {
    Q_OBJECT
    Q_PROPERTY(qreal moonPhase READ moonPhase WRITE setMoonPhase)
public:
    explicit ThemeButton(QWidget* parent = nullptr) : QToolButton(parent) {
        setFixedSize(64, 32);
        setText("");
        setCheckable(true);
    }

    qreal moonPhase() const { return m_phase; }
    void setMoonPhase(qreal v) { m_phase = v; update(); }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        bool dark = isChecked();

        // 轨道背景
        QColor trackColor = dark ? QColor("#0a84ff") : QColor("#d1d1d6");
        p.setBrush(trackColor);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(rect(), 16, 16);

        // 滑块位置插值
        qreal t = m_phase; // 0=浅色, 1=深色
        qreal knobX = 4 + t * (width() - 32 - 4);
        QRectF knob(knobX, 4, 24, 24);

        p.setBrush(QColor("#ffffff"));
        p.drawEllipse(knob);

        // 图标：太阳/月亮
        p.setPen(dark ? QColor("#1c1c1e") : QColor("#ff9500"));
        p.setFont(QFont("Arial", 11));
        QString icon = dark ? "🌙" : "☀️";
        p.drawText(knob, Qt::AlignCenter, icon);
    }

private:
    qreal m_phase = 0.0;
};

#include "MainWindow.moc"

// ─────────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("LaunchPad Manager");
    setMinimumSize(860, 580);
    resize(1100, 680);

    m_manager = new DesktopFileManager(this);

    // ── 工具栏 ──
    QToolBar* toolbar = new QToolBar("工具栏", this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setObjectName("MainToolBar");
    toolbar->setIconSize(QSize(18, 18));
    addToolBar(Qt::TopToolBarArea, toolbar);

    // 刷新按钮
    m_refreshBtn = new RefreshButton;
    m_refreshBtn->setToolTip("刷新应用列表");
    toolbar->addWidget(m_refreshBtn);

    // 弹簧
    QWidget* spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);

    // 深色模式切换
    m_darkModeBtn = new ThemeButton;
    m_darkModeBtn->setToolTip("切换深色/浅色模式");
    toolbar->addWidget(m_darkModeBtn);

    // 刷新旋转动画
    m_rotateAnim = new QPropertyAnimation(m_refreshBtn, "angle", this);
    m_rotateAnim->setDuration(600);
    m_rotateAnim->setStartValue(0);
    m_rotateAnim->setEndValue(360);
    m_rotateAnim->setEasingCurve(QEasingCurve::OutCubic);

    // 主题淡入淡出动画
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_fadeAnim = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    m_fadeAnim->setDuration(300);
    m_fadeAnim->setEasingCurve(QEasingCurve::InOutSine);

    // 深色按钮切换动画（moonPhase 属性）
    // ThemeButton 已内置插值，通过 QPropertyAnimation 驱动
    auto* themeAnim = new QPropertyAnimation(m_darkModeBtn, "moonPhase", this);
    themeAnim->setDuration(350);
    themeAnim->setEasingCurve(QEasingCurve::InOutQuad);

    // ── Splitter ──
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

    // ── 连接信号 ──
    connect(m_refreshBtn, &QToolButton::clicked, this, &MainWindow::onRefresh);

    connect(m_darkModeBtn, &QToolButton::clicked, this, [this, themeAnim](){
        m_darkMode = m_darkModeBtn->isChecked();
        // 滑块动画
        themeAnim->stop();
        themeAnim->setStartValue(m_darkMode ? 0.0 : 1.0);
        themeAnim->setEndValue(m_darkMode ? 1.0 : 0.0);
        themeAnim->start();
        toggleDarkMode();
    });

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

void MainWindow::onRefresh() {
    // 旋转动画
    m_rotateAnim->stop();
    static_cast<RefreshButton*>(m_refreshBtn)->setAngle(0);
    m_rotateAnim->start();
    m_refreshBtn->setEnabled(false);

    // 刷新完成后恢复
    QTimer::singleShot(650, this, [this](){
        m_manager->refresh();
        m_refreshBtn->setEnabled(true);
    });

    statusBar()->showMessage("正在刷新…");
}

void MainWindow::toggleDarkMode() {
    // 淡出 → 切换主题 → 淡入
    m_fadeAnim->stop();

    auto* centralW = centralWidget();
    centralW->setGraphicsEffect(m_opacityEffect);

    // 淡出
    m_fadeAnim->setStartValue(1.0);
    m_fadeAnim->setEndValue(0.0);
    m_fadeAnim->start();

    connect(m_fadeAnim, &QPropertyAnimation::finished, this, [this](){
        disconnect(m_fadeAnim, &QPropertyAnimation::finished, this, nullptr);
        applyTheme();

        // 淡入
        m_fadeAnim->setStartValue(0.0);
        m_fadeAnim->setEndValue(1.0);
        m_fadeAnim->start();
        connect(m_fadeAnim, &QPropertyAnimation::finished, this, [this](){
            disconnect(m_fadeAnim, &QPropertyAnimation::finished, this, nullptr);
            centralWidget()->setGraphicsEffect(nullptr);
        });
    });
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
    QFileInfo fi(filePath);
    QString appName = fi.baseName();

    auto ret = QMessageBox::question(this, "卸载应用",
        QString("确定要卸载 %1 吗？\n\n将尝试通过包管理器卸载，并删除启动项。\n此操作不可逆！")
            .arg(appName),
        QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    m_manager->removeFile(filePath);

    QStringList cmds = {
        QString("pkexec apt-get remove -y %1").arg(appName),
        QString("flatpak uninstall -y --noninteractive %1").arg(appName),
        QString("snap remove %1").arg(appName),
    };

    for (const QString& cmd : cmds) {
        QStringList args = cmd.split(' ');
        QString prog = args.takeFirst();
        QProcess proc;
        proc.start(prog, args);
        if (proc.waitForStarted(2000)) {
            proc.waitForFinished(30000);
            if (proc.exitCode() == 0) {
                QMessageBox::information(this, "卸载成功",
                    QString("%1 已成功卸载。").arg(appName));
                m_rightPanel->infoPage()->clear();
                m_rightPanel->editPage()->clear();
                return;
            }
        }
    }

    QMessageBox::warning(this, "卸载",
        QString("已删除启动项，但包管理器卸载 %1 失败。\n请手动卸载。").arg(appName));
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
