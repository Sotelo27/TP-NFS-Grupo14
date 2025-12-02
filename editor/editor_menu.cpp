#include "editor_menu.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QSpinBox>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QPalette>
#include <QLineEdit>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <QMessageBox>

EditorMenu::EditorMenu(QWidget *parent)
    : QWidget(parent),
      mapLabel(nullptr),
      recorridoActual(-1),
      nombreArchivoInput(nullptr)
{
    // -------- FONDO --------
    QPalette pal;
    pal.setBrush(QPalette::Window, QPixmap("assets/images/fondo.png").scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    setAutoFillBackground(true);
    setPalette(pal);

    // Layout principal horizontal
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(40,40,40,40);
    mainLayout->setSpacing(30);

    // ----------------- Panel lateral -----------------
    QWidget* panelLateral = new QWidget(this);
    panelLateral->setFixedWidth(250);
    panelLateral->setStyleSheet(
        "background: rgba(15,5,25,0.85); border-radius: 20px;"
        "border: 2px solid rgba(255,0,180,0.2);"
    );

    lateralLayout = new QVBoxLayout(panelLateral);
    lateralLayout->setContentsMargins(15,15,15,15);
    lateralLayout->setSpacing(20);
    lateralLayout->setAlignment(Qt::AlignTop);

    // ----------------- Botón Summit -----------------
    QPushButton* summitBtn = new QPushButton("Summit", panelLateral);
    summitBtn->setStyleSheet(
        "QPushButton { background-color: #6a00ff; color:white; font-size:18px; padding:10px; border-radius:10px; }"
        "QPushButton:hover { background-color: #8d33ff; }"
    );
    summitBtn->setFixedHeight(50);
    lateralLayout->addWidget(summitBtn, 0, Qt::AlignCenter);

    // conexión
    connect(summitBtn, &QPushButton::clicked, this, &EditorMenu::onSummit);

    // SpinBox para elegir cantidad de recorridos
    QLabel* lbl = new QLabel("Cantidad de recorridos:", panelLateral);
    lbl->setStyleSheet("color:white; font-size:16px; font-weight:bold;");
    lateralLayout->addWidget(lbl);

    QSpinBox* spin = new QSpinBox(panelLateral);
    spin->setMinimum(1);
    spin->setMaximum(6); // máximo 6
    spin->setValue(3);   // valor inicial
    spin->setStyleSheet("font-size:16px; height:30px;");
    lateralLayout->addWidget(spin);

    connect(spin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &EditorMenu::onCantidadRecorridosChanged);

    // ----------------- INPUT NUEVO: Nombre archivo -----------------
    QLabel* lblNombre = new QLabel("Nombre del archivo:", panelLateral);
    lblNombre->setStyleSheet("color:white; font-size:16px; font-weight:bold;");
    lateralLayout->addWidget(lblNombre);

    nombreArchivoInput = new QLineEdit(panelLateral);
    nombreArchivoInput->setPlaceholderText("ej: mapa_custom");
    nombreArchivoInput->setStyleSheet(
        "background: white; border-radius:6px; padding:5px; font-size:15px;"
    );
    lateralLayout->addWidget(nombreArchivoInput);

    // ----------------- Botón de ayuda -----------------
    QPushButton* helpBtn = new QPushButton("?", panelLateral);
    helpBtn->setStyleSheet(
        "QPushButton { background-color: rgba(255,255,255,0.15); color:white; font-size:16px; padding:8px; border-radius:8px; }"
        "QPushButton:hover { background-color: rgba(255,255,255,0.3); }"
    );
    helpBtn->setFixedSize(40,40);
    lateralLayout->addWidget(helpBtn, 0, Qt::AlignCenter);

    connect(helpBtn, &QPushButton::clicked, [this]() {
        QMessageBox::information(this, "Guía de uso",
            "Hola, esta es la guía de cómo crear tu mapa:\n"
            "1) Con la barra vertical izquierda podrás elegir la cantidad de recorridos que quieras hacer\n"
            "2) Como mínimo 1 recorrido y máximo 6\n"
            "3) Elije un recorrido y con click izquierdo agrega checkpoint donde quieras\n"
            "4) Para quitar el último checkpoint, hacé click derecho\n"
            "5) Agrega los checkpoints donde quieras, eso sí, si lo agregas en el agua es responsabilidad tuya.\n"
            "6) Guarda tu mapa editado agregándole un nombre"
        );
    });

    // ---------------------------------------------------------------

    mainLayout->addWidget(panelLateral);

    // ----------------- Panel mapa -----------------
    QWidget* panelMapa = new QWidget(this);
    panelMapa->setFixedSize(500,500);
    panelMapa->setStyleSheet(
        "background: rgba(10,0,20,0.55); border-radius: 20px;"
        "border: 3px solid rgba(200,120,255,0.60);"
    );
    auto* glow = new QGraphicsDropShadowEffect(panelMapa);
    glow->setBlurRadius(50);
    glow->setOffset(0,0);
    glow->setColor(QColor(200,120,255,180));
    panelMapa->setGraphicsEffect(glow);

    QVBoxLayout* mapaLayout = new QVBoxLayout(panelMapa);
    mapaLayout->setContentsMargins(15,15,15,15);
    mapaLayout->setSpacing(0);
    mapaLayout->setAlignment(Qt::AlignCenter);

    mapLabel = new ClickableImage(panelMapa);
    mapLabel->setFixedSize(450,450);
    mapLabel->setStyleSheet("background: transparent; border-radius: 12px;");
    mapLabel->setAlignment(Qt::AlignCenter);
    mapaLayout->addWidget(mapLabel);

    mainLayout->addWidget(panelMapa);

    setLayout(mainLayout);

    // ----------------- Conexiones -----------------
    connect(mapLabel, &ClickableImage::leftClick, this, &EditorMenu::onLeftClick);
    connect(mapLabel, &ClickableImage::rightClick, this, &EditorMenu::onRightClick);

    // inicializo botones con valor inicial
    onCantidadRecorridosChanged(spin->value());
}

// ----------------- Cuando cambia cantidad de recorridos -----------------
void EditorMenu::onCantidadRecorridosChanged(int n)
{
    // limpiar botones existentes
    for (QPushButton* btn : botonesRecorridos) {
        lateralLayout->removeWidget(btn);
        btn->deleteLater();
    }
    botonesRecorridos.clear();
    recorridos.clear();

    // crear botones
    for(int i=0;i<n;i++){
        QPushButton* btn = new QPushButton(QString("Recorrido %1").arg(i+1));
        btn->setStyleSheet(
            "QPushButton { background-color: rgba(255,255,255,0.15); color:white; font-size:16px; padding:8px; border-radius:8px; }"
            "QPushButton:hover { background-color: rgba(255,255,255,0.3); }"
        );
        lateralLayout->addWidget(btn);
        connect(btn, &QPushButton::clicked, [this,i](){ onSelectRecorrido(i); });
        botonesRecorridos.append(btn);
        recorridos.append(Recorrido());
    }

    // actualizar selección visual
    for(int i=0;i<botonesRecorridos.size();i++){
        if(i==recorridoActual){
            botonesRecorridos[i]->setStyleSheet(
                "background-color: rgba(255,255,255,0.5); color:white; font-size:16px; padding:8px; border-radius:8px;"
            );
        }
    }
}

// ----------------- Selección de recorrido -----------------
void EditorMenu::onSelectRecorrido(int index)
{
    recorridoActual = index;
    actualizarCheckpointsEnMapa();

    // resaltar botón seleccionado
    for(int i=0;i<botonesRecorridos.size();i++){
        if(i==recorridoActual){
            botonesRecorridos[i]->setStyleSheet(
                "background-color: rgba(255,255,255,0.5); color:white; font-size:16px; padding:8px; border-radius:8px;"
            );
        } else {
            botonesRecorridos[i]->setStyleSheet(
                "QPushButton { background-color: rgba(255,255,255,0.15); color:white; font-size:16px; padding:8px; border-radius:8px; }"
                "QPushButton:hover { background-color: rgba(255,255,255,0.3); }"
            );
        }
    }
}

// ----------------- Agregar click -----------------
void EditorMenu::onLeftClick(int x,int y)
{
    if (recorridoActual<0) return;
    recorridos[recorridoActual].checkpoints.append(QPoint(x,y));
    actualizarCheckpointsEnMapa();

    qDebug() << "Click agregado en Recorrido" << recorridoActual + 1
         << "-> Coordenada:" << x << "," << y;

    qDebug() << "Listado completo de checkpoints:";
    for (const QPoint& p : recorridos[recorridoActual].checkpoints) {
        qDebug() << "  (" << p.x() << "," << p.y() << ")";
    }
}

// ----------------- Borrar último -----------------
void EditorMenu::onRightClick()
{
    if (recorridoActual<0) return;
    if (!recorridos[recorridoActual].checkpoints.isEmpty()) {
        recorridos[recorridoActual].checkpoints.removeLast();
        actualizarCheckpointsEnMapa();

        qDebug() << "Último checkpoint eliminado del Recorrido" << recorridoActual + 1;
        qDebug() << "Listado actualizado de checkpoints:";
        for (const QPoint& p : recorridos[recorridoActual].checkpoints) {
            qDebug() << "  (" << p.x() << "," << p.y() << ")";
        }
    }
}

// ----------------- Refrescar checkpoints -----------------
void EditorMenu::actualizarCheckpointsEnMapa()
{
    if (recorridoActual<0) return;
    mapLabel->clearPoints();

    const QVector<QPoint>& checkpoints = recorridos[recorridoActual].checkpoints;
    QVector<QPoint> ptsLabel;

    QSize labelSize = mapLabel->size();
    QSize imgScaled = mapLabel->getOriginalPixmap().size();
    imgScaled.scale(labelSize, Qt::KeepAspectRatio);

    int offsetX = (labelSize.width()  - imgScaled.width())/2;
    int offsetY = (labelSize.height() - imgScaled.height())/2;

    float scaleX = (float)imgScaled.width()  / mapLabel->getOriginalPixmap().width();
    float scaleY = (float)imgScaled.height() / mapLabel->getOriginalPixmap().height();

    for (const QPoint &p : checkpoints) {
        int lx = p.x()*scaleX + offsetX;
        int ly = p.y()*scaleY + offsetY;
        ptsLabel.append(QPoint(lx,ly));
    }

    mapLabel->setPoints(ptsLabel);
}

// ----------------- Cargar mapa -----------------
void EditorMenu::load_menu(const QString& selected_map, const QString& selected_map_image)
{
    this->selected_map = selected_map;
    this->selected_map_image = selected_map_image;

    QPixmap pix(selected_map_image);
    if(pix.isNull()) {
        mapLabel->setText("No se pudo cargar la imagen");
        return;
    }

    mapLabel->setImage(pix);
    mapLabel->setText("");
}

void EditorMenu::onSummit()
{

    for (int r = 0; r < recorridos.size(); r++) {
        const Recorrido &rec = recorridos[r];

        if (rec.checkpoints.size() < 2) {
            QMessageBox::warning(this,
                                 "Pocos checkpoints",
                                 QString("Hay menos de 2 checkpoints en el recorrido %1")
                                 .arg(r + 1));
            return;
        }
    }

    qDebug() << "Generando YAML...";

    const std::string basePath = "editor/BaseMap/BaseMapa" + selected_map.toStdString() + ".yaml";
    YAML::Node root;

    try {
        root = YAML::LoadFile(basePath);
    } catch (const std::exception &e) {
        qWarning() << "Error al cargar YAML:" << e.what();
        return;
    }

    YAML::Node layers = root["layers"];
    if (!layers || !layers.IsSequence()) {
        qWarning() << "ERROR: 'layers' no existe o no es secuencia!";
        return;
    }

    int nextLayerId = root["nextlayerid"].as<int>();
    int nextObjectId = root["nextobjectid"].as<int>();

    for (int r = 0; r < recorridos.size(); r++)
    {
        const Recorrido &rec = recorridos[r];
        if (rec.checkpoints.isEmpty())
            continue;

        char raceId = 'A' + r;

        YAML::Node newLayer;
        newLayer["id"] = nextLayerId++;
        newLayer["name"] = std::string("Checkpoint_") + raceId;
        newLayer["type"] = "objectgroup";
        newLayer["visible"] = true;
        newLayer["opacity"] = 1;
        newLayer["draworder"] = "topdown";

        YAML::Node objects(YAML::NodeType::Sequence);

        for (int i = 0; i < rec.checkpoints.size(); i++)
        {
            const QPoint &pt = rec.checkpoints[i];

            YAML::Node obj;
            obj["id"] = nextObjectId++;
            obj["name"] = (i == 0) ? "Start" :
                          (i == rec.checkpoints.size() - 1) ? "Finish" : "Normal";

            obj["type"] = "";
            obj["x"] = pt.x();
            obj["y"] = pt.y();
            obj["width"] = 0;
            obj["height"] = 0;
            obj["rotation"] = 0;
            obj["visible"] = true;
            obj["point"] = true;

            YAML::Node props(YAML::NodeType::Sequence);

            {
                YAML::Node p;
                p["name"] = "index";
                p["type"] = "int";
                p["value"] = i;
                props.push_back(p);
            }
            {
                YAML::Node p;
                p["name"] = "race_id";
                p["type"] = "string";
                p["value"] = std::string(1, raceId);
                props.push_back(p);
            }
            {
                YAML::Node p;
                p["name"] = "type";
                p["type"] = "string";
                if (i == 0)
                    p["value"] = "start";
                else if (i == rec.checkpoints.size() - 1)
                    p["value"] = "finish";
                else
                    p["value"] = "normal";
                props.push_back(p);
            }

            obj["properties"] = props;
            objects.push_back(obj);
        }

        newLayer["objects"] = objects;

        layers.push_back(newLayer);
    }

    root["nextlayerid"] = nextLayerId;
    root["nextobjectid"] = nextObjectId;

    YAML::Emitter emitter;
    emitter.SetIndent(2);
    emitter << root;

    QString nombreElegido = nombreArchivoInput->text().trimmed();
    if (nombreElegido.isEmpty()) {
        QMessageBox::warning(this,
                             "Nombre vacío",
                             "El nombre del archivo no puede estar vacío.");
        return;
    }

    std::string outPath = "editor/MapsEdited/" + nombreElegido.toStdString() + ".yaml";
    // ------------------------------------------------------------------------

    if (QFile::exists(QString::fromStdString(outPath))) {
        QMessageBox::warning(this,
                             "Archivo existente",
                             "Ya existe un archivo con ese nombre en editor/MapsEdited.\n"
                             "Elige otro nombre.");
        return;
    }

    std::ofstream file(outPath);

    if (!file.is_open()) {
        qWarning() << "No se pudo abrir archivo de salida!";
        return;
    }

    file << emitter.c_str();
    file.close();

    qDebug() << "YAML generado correctamente -->" << QString::fromStdString(outPath);
}
