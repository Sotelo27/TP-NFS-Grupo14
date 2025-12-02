#ifndef EDITOR_MENU_H
#define EDITOR_MENU_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include "clickable_image.h"

struct Recorrido {
    QVector<QPoint> checkpoints;
};

class EditorMenu : public QWidget {
    Q_OBJECT

public:
    explicit EditorMenu(QWidget *parent = nullptr);

    void load_menu(const QString& selected_map, const QString& selected_map_image);

private slots:
    void onCantidadRecorridosChanged(int n);
    void onSelectRecorrido(int index);
    void onLeftClick(int x,int y);
    void onRightClick();
    void onSummit();

private:
    void crearBotonesRecorridos(int cantidad);
    void actualizarCheckpointsEnMapa();

    ClickableImage* mapLabel;
    QVBoxLayout* lateralLayout;
    QVector<QPushButton*> botonesRecorridos;
    QVector<Recorrido> recorridos;
    int recorridoActual;

    QString selected_map;
    QString selected_map_image;
    QLineEdit* nombreArchivoInput;
};

#endif // EDITOR_MENU_H
