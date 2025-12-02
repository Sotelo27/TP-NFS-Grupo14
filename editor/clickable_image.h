#ifndef CLICKABLE_IMAGE_H
#define CLICKABLE_IMAGE_H

#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <QVector>

class ClickableImage : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableImage(QWidget *parent = nullptr);

    void setImage(const QPixmap &pix);

    // nueva API pública
    void clearPoints();
    void setPoints(const QVector<QPoint>& pts);
    QVector<QPoint> getPoints() const;

    QPixmap getOriginalPixmap() const;  // devuelve copia del original

    signals:
        void leftClick(int imgX, int imgY);
    void rightClick();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPixmap original;
    QVector<QPoint> puntos;

    QPoint mapToImageCoords(const QPoint &p);
};

#endif
