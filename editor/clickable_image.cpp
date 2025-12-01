#include "clickable_image.h"
#include <QPainter>

ClickableImage::ClickableImage(QWidget *parent)
    : QLabel(parent)
{
    setMouseTracking(true);
    setScaledContents(false);
}

void ClickableImage::setImage(const QPixmap &pix)
{
    original = pix;
    puntos.clear();

    if (!original.isNull()) {
        setPixmap(original.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    update();
}

QPixmap ClickableImage::getOriginalPixmap() const {
    return original;
}

void ClickableImage::clearPoints() {
    puntos.clear();
    update();
}

void ClickableImage::setPoints(const QVector<QPoint>& pts) {
    puntos = pts;
    update();
}

QVector<QPoint> ClickableImage::getPoints() const {
    return puntos;
}

QPoint ClickableImage::mapToImageCoords(const QPoint &p)
{
    if (original.isNull())
        return QPoint(-1, -1);

    QSize labelSize = this->size();
    QSize imgScaled = pixmap()->size();

    int offsetX = (labelSize.width()  - imgScaled.width()) / 2;
    int offsetY = (labelSize.height() - imgScaled.height()) / 2;

    if (p.x() < offsetX || p.x() > offsetX + imgScaled.width()) return QPoint(-1, -1);
    if (p.y() < offsetY || p.y() > offsetY + imgScaled.height()) return QPoint(-1, -1);

    float scaleX = (float)original.width()  / imgScaled.width();
    float scaleY = (float)original.height() / imgScaled.height();

    int imgX = (p.x() - offsetX) * scaleX;
    int imgY = (p.y() - offsetY) * scaleY;

    return QPoint(imgX, imgY);
}

void ClickableImage::mousePressEvent(QMouseEvent *event)
{
    if (original.isNull()) return;

    QPoint posLabel = event->pos();
    QPoint posImg = mapToImageCoords(posLabel);

    if (posImg.x() < 0) return; // clic fuera de la imagen

    if (event->button() == Qt::LeftButton) {
        puntos.push_back(posLabel);
        emit leftClick(posImg.x(), posImg.y());
        update();
    } else if (event->button() == Qt::RightButton) {
        if (!puntos.isEmpty()) {
            puntos.removeLast();
            emit rightClick();
            update();
        }
    }
}

void ClickableImage::paintEvent(QPaintEvent *ev)
{
    QLabel::paintEvent(ev);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int i = 1;
    for (const QPoint &p : puntos) {
        painter.setPen(QPen(Qt::red, 3));
        painter.setBrush(Qt::red);
        painter.drawEllipse(p, 10, 10);

        painter.setPen(Qt::white);
        painter.drawText(p.x() - 4, p.y() + 4, QString::number(i));

        i++;
    }
}
