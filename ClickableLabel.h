#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget* parent = nullptr) : QLabel(parent) {}

signals:
    void clicked();  // 自己的点击信号

protected:
    void mousePressEvent(QMouseEvent* event) override
    {
        emit clicked();  // 点击就发信号
        QLabel::mousePressEvent(event);
    }
};

#endif // CLICKABLELABEL_H
