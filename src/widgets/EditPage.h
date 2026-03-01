#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include "../core/DesktopFile.h"

class EditPage : public QWidget {
    Q_OBJECT
public:
    explicit EditPage(QWidget* parent = nullptr);
    void loadFile(const DesktopFile& df);
    void clear();

signals:
    void saveRequested(const DesktopFile& df);
    void deleteRequested(const QString& filePath);
    void uninstallRequested(const QString& filePath);

private slots:
    void onSave();
    void onDelete();
    void onUninstall();
    void onChooseIcon();

private:
    DesktopFile  m_current;
    QLabel*     m_iconPreview;
    QLineEdit*  m_nameEdit;
    QLineEdit*  m_genericNameEdit;
    QLineEdit*  m_commentEdit;
    QLineEdit*  m_execEdit;
    QLineEdit*  m_iconEdit;
    QLineEdit*  m_categoriesEdit;
};
