#ifndef CLICKABLE_IMAGE_H
#define CLICKABLE_IMAGE_H

#include <QLabel>
#include <QVector>
#include <QPoint>
#include <QMouseEvent>

class ClickableImage : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableImage(QWidget *parent = nullptr);

    void setImage(const QPixmap &pix);
    QPixmap getOriginalPixmap() const;

    void clearPoints();
    void setPoints(const QVector<QPoint>& pts);
    QVector<QPoint> getPoints() const;

    QPoint mapToImageCoords(const QPoint &p);

    signals:
        void leftClick(int x, int y);
    void rightClick();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *ev) override;

private:
    QPixmap original;
    QVector<QPoint> puntos;
};

#endif // CLICKABLE_IMAGE_H
