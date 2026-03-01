#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include "../core/DesktopFile.h"

class AddPage : public QWidget {
    Q_OBJECT
public:
    explicit AddPage(QWidget* parent = nullptr);

signals:
    void addRequested(const DesktopFile& df);

private slots:
    void onAdd();
    void onChooseIcon();
    void onReset();

private:
    QLabel*    m_iconPreview;
    QLineEdit* m_nameEdit;
    QLineEdit* m_genericNameEdit;
    QLineEdit* m_commentEdit;
    QLineEdit* m_execEdit;
    QLineEdit* m_iconEdit;
    QLineEdit* m_categoriesEdit;
};