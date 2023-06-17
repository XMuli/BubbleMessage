﻿#include "bubblehistory.h"
#include <QListWidgetItem>
#include <QDebug>

BubbleHistory::BubbleHistory(QWidget *parent)
    : QListWidget(parent)
{
    initUI();
}

void BubbleHistory::addBubble(const BubbleParas& paras)
{
    Bubble *bubble = new Bubble(paras.text, paras.role, paras.time, this);
    qDebug () << "addBubble（） -> bubble:" << bubble << "  this:" << this << "  bubble->size():" << bubble->size();
    QListWidgetItem *item = new QListWidgetItem(this);
    addItem(item);
//    qDebug() << "---bubble->size():" << bubble->size() << "bubble->height():" << bubble->height();
//    bubble->printfInfo();

    const int& bubbleHeight = paras.role ==BUBBLE_ROLE::BR_ME ? bubble->height() : bubble->size().height();
//    const QSize size(bubble->size().width(),  bubbleHeight);
    item->setSizeHint(QSize(bubble->size().width(),  bubble->height()));  // 初次添加进来得时候，需要给定一个大小，此处不能参考下面赋值 item->sizeHint()，一样会有问题
    setItemWidget(item, bubble);

//    qDebug() << "---bubble2->size():" << bubble->size() << "bubble->height():" << bubble->height();
//    bubble->printfInfo();

    connect(bubble, &Bubble::sigChangedHeight, [=](int height){
//        qDebug () << "change height:" << height;

        // 拉升整体窗口大小时候，效果会更好 比起此处使用 QSize(bubble->width(), height + 60) 赋值; 60 为头像等其 Bubble 里面其它的控件的高度，如何计算出来？

        int itemHeight;
        int browserHeight;
        int ResidualHeight = lastBubbleAIHeight(itemHeight, browserHeight);
        qDebug() << "change height:" << height << bubble->textBrowserheight() << bubble->size() << "bubble->height():" << bubble->height();
        QListWidgetItem * lastItemAI = lastListItemAI();

        qDebug() << "--@2->" <<itemHeight << browserHeight << ResidualHeight << bubble->isVisible();
        if (lastItemAI) lastItemAI->setSizeHint(QSize(lastItemAI->sizeHint().width(), height + ResidualHeight));  // 通过 UI Design 计算和 直接相减，理论都应该是 48， 莫非是 48 * 此屏幕的缩放比？？
    });
}

void BubbleHistory::appendLastBubbleText(const QString &text, const QDateTime &time)
{
    Bubble* bubble = lastBubble();
    if (!bubble) return;

    if (bubble->role() == BUBBLE_ROLE::BR_ME) {
        // 不存在则先创建 BR_AICHAT 的气泡
        BubbleParas bubbleParas(text, BUBBLE_ROLE::BR_AICHAT);
        addBubble(bubbleParas);
    } else if (bubble->role() == BUBBLE_ROLE::BR_AICHAT) {
        bubble->appendText(text); // update text
    }

    scrollToBottom();
}

Bubble *BubbleHistory::lastBubble()
{
    QListWidgetItem* lastItem = item(this->count() - 1);                 // 获取最后一个 QListWidgetItem
    Bubble* bubble = qobject_cast<Bubble *>(this->itemWidget(lastItem)); // 将QListWidgetItem转换为ChatBubble*
    return bubble;
}

Bubble *BubbleHistory::lastBubbleAI()
{
    QListWidgetItem * item = lastListItemAI();
    return item ? qobject_cast<Bubble *>(itemWidget(item)) : nullptr;
}

const int BubbleHistory::lastBubbleAIHeight(int &itemHeight, int &browserHeight)
{
    const auto& bubble = lastBubbleAI();
    if (!bubble) return 0;

    itemHeight = bubble->size().height();
    browserHeight = bubble->textBrowserheight();
    qDebug() << "--@1->" <<bubble->size() << browserHeight << itemHeight - browserHeight << bubble->isVisible();
    return itemHeight - browserHeight;
}


QListWidgetItem *BubbleHistory::lastListItemAI()
{
    for (int i = this->count() - 1; i >= 0 ; --i) {
        QListWidgetItem* lastItem = item(i);
        Bubble* bubble = qobject_cast<Bubble *>(this->itemWidget(lastItem));

        if (bubble && bubble->role() == BUBBLE_ROLE::BR_AICHAT) {
            qDebug() << "lastListItemAI:" << bubble << "i:" << i << "text:" << bubble->text() << "  count():" << count();
            return lastItem;
        } else {
            continue;
        }
    }

    qDebug() << "lastListItemAI is nullptr";
    return nullptr;
}

void BubbleHistory::initUI()
{
    // 设置样式表禁用悬浮高亮效果
    //    setStyleSheet("QListWidget::item:hover { background-color: transparent; }");
    resize(1200, 760);
}
