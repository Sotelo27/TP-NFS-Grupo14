#ifndef EDITOR_MENU_H
#define EDITOR_MENU_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QString>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include "clickable_image.h"

struct Recorrido {
    QVector<QPoint> checkpoints;
};

class EditorMenu : public QWidget
{
    Q_OBJECT
public:
    explicit EditorMenu(QWidget *parent = nullptr);

    // Cargar mapa en el editor
    void load_menu(const QString& selected_map, const QString& selected_map_image);

private slots:
    void onCantidadRecorridosChanged(int n);
    void onSelectRecorrido(int index);
    void onLeftClick(int x,int y);
    void onRightClick();
    void actualizarCheckpointsEnMapa();
    void onSummit();

signals:
    void back_to_choose_map();

private:
    ClickableImage* mapLabel;
    QVector<QPushButton*> botonesRecorridos;
    QVector<Recorrido> recorridos;
    int recorridoActual;
    QVBoxLayout* lateralLayout;
    QLineEdit* nombreArchivoInput;
    QString selected_map;
    QString selected_map_image;
};

#endif // EDITOR_MENU_H
