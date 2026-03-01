#include "DesktopFileDelegate.h"
#include "../models/DesktopFileModel.h"

#include <QPainter>
#include <QIcon>
#include <QApplication>

static const int ICON_SIZE    = 40;
static const int ITEM_HEIGHT  = 62;
static const int H_MARGIN     = 10;
static const int V_MARGIN     = 8;
static const int ICON_TEXT_GAP = 12;

DesktopFileDelegate::DesktopFileDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{}

void DesktopFileDelegate::paint(QPainter* painter,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QRect r = option.rect;

    // 背景
    bool selected = option.state & QStyle::State_Selected;
    bool hovered  = option.state & QStyle::State_MouseOver;

    if (selected) {
        painter->setBrush(QColor(0, 113, 227, 38));
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(r.adjusted(4,2,-4,-2), 10, 10);
    } else if (hovered) {
        painter->setBrush(QColor(0, 113, 227, 18));
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(r.adjusted(4,2,-4,-2), 10, 10);
    }

    // 图标
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QRect iconRect(r.left() + H_MARGIN,
                   r.top() + (r.height() - ICON_SIZE) / 2,
                   ICON_SIZE, ICON_SIZE);
    icon.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);

    // 文字区域
    int textX = iconRect.right() + ICON_TEXT_GAP;
    int textW = r.right() - textX - H_MARGIN;

    QString name    = index.data(DesktopFileModel::NameRole).toString();
    QString comment = index.data(DesktopFileModel::CommentRole).toString();

    // Name
    QFont nameFont = option.font;
    nameFont.setPointSizeF(nameFont.pointSizeF());
    nameFont.setWeight(QFont::Medium);
    painter->setFont(nameFont);
    painter->setPen(QColor("#1d1d1f"));

    QRect nameRect(textX, r.top() + V_MARGIN, textW, 20);
    painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter,
                      option.fontMetrics.elidedText(name, Qt::ElideRight, textW));

    // Comment
    if (!comment.isEmpty()) {
        QFont commentFont = option.font;
        commentFont.setPointSizeF(commentFont.pointSizeF() - 1);
        painter->setFont(commentFont);
        painter->setPen(QColor("#636366"));

        QFontMetrics fm(commentFont);
        QRect commentRect(textX, nameRect.bottom() + 3, textW, 16);
        painter->drawText(commentRect, Qt::AlignLeft | Qt::AlignVCenter,
                          fm.elidedText(comment, Qt::ElideRight, textW));
    }

    painter->restore();
}

QSize DesktopFileDelegate::sizeHint(const QStyleOptionViewItem&,
                                    const QModelIndex&) const
{
    return QSize(0, ITEM_HEIGHT);
}