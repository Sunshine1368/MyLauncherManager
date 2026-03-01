#pragma once
#include <QWidget>
#include <QTabBar>
#include <QStackedWidget>
#include "widgets/InfoPage.h"
#include "widgets/EditPage.h"
#include "widgets/AddPage.h"
#include "core/DesktopFile.h"

class RightPanel : public QWidget {
    Q_OBJECT
public:
    explicit RightPanel(QWidget* parent = nullptr);

    InfoPage* infoPage() const { return m_infoPage; }
    EditPage* editPage() const { return m_editPage; }
    AddPage*  addPage()  const { return m_addPage;  }

    void showFile(const DesktopFile& df);

private:
    QTabBar*       m_tabBar;
    QStackedWidget* m_stack;
    InfoPage*      m_infoPage;
    EditPage*      m_editPage;
    AddPage*       m_addPage;
};