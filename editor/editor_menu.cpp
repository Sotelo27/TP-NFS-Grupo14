#include "editor_menu.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QPalette>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <QDebug>
#include <QMessageBox>

EditorMenu::EditorMenu(QWidget *parent)
    : QWidget(parent),
      mapLabel(nullptr),
      recorridoActual(-1),
      nombreArchivoInput(nullptr)
{
    // Fondo
    QPalette pal;
    pal.setBrush(QPalette::Window, QPixmap("assets/images/fondo.png"));
    setAutoFillBackground(true);
    setPalette(pal);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(40,40,40,40);
    mainLayout->setSpacing(30);

    // Panel lateral
    QWidget* panelLateral = new QWidget(this);
    panelLateral->setFixedWidth(250);
    panelLateral->setStyleSheet("background: rgba(15,5,25,0.85); border-radius: 20px; border: 2px solid rgba(255,0,180,0.2);");

    lateralLayout = new QVBoxLayout(panelLateral);
    lateralLayout->setContentsMargins(15,15,15,15);
    lateralLayout->setSpacing(20);
    lateralLayout->setAlignment(Qt::AlignTop);

    // Summit
    QPushButton* summitBtn = new QPushButton("Summit", panelLateral);
    summitBtn->setStyleSheet("QPushButton { background-color: #6a00ff; color:white; font-size:18px; padding:10px; border-radius:10px; } QPushButton:hover { background-color: #8d33ff; }");
    summitBtn->setFixedHeight(50);
    lateralLayout->addWidget(summitBtn, 0, Qt::AlignCenter);
    connect(summitBtn, &QPushButton::clicked, this, &EditorMenu::onSummit);

    // Cantidad de recorridos
    QLabel* lbl = new QLabel("Cantidad de recorridos:", panelLateral);
    lbl->setStyleSheet("color:white; font-size:16px; font-weight:bold;");
    lateralLayout->addWidget(lbl);

    QSpinBox* spin = new QSpinBox(panelLateral);
    spin->setMinimum(1);
    spin->setMaximum(6);
    spin->setValue(3);
    spin->setStyleSheet("font-size:16px; height:30px;");
    lateralLayout->addWidget(spin);
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), this, &EditorMenu::onCantidadRecorridosChanged);
#else
    connect(spin, SIGNAL(valueChanged(int)), this, SLOT(onCantidadRecorridosChanged(int)));
#endif

    // Nombre archivo
    QLabel* lblNombre = new QLabel("Nombre del archivo:", panelLateral);
    lblNombre->setStyleSheet("color:white; font-size:16px; font-weight:bold;");
    lateralLayout->addWidget(lblNombre);

    nombreArchivoInput = new QLineEdit(panelLateral);
    nombreArchivoInput->setPlaceholderText("ej: mapa_custom");
    nombreArchivoInput->setStyleSheet("background: white; border-radius:6px; padding:5px; font-size:15px;");
    lateralLayout->addWidget(nombreArchivoInput);

    mainLayout->addWidget(panelLateral);

    // Panel mapa
    QWidget* panelMapa = new QWidget(this);
    panelMapa->setFixedSize(500,500);
    panelMapa->setStyleSheet("background: rgba(10,0,20,0.55); border-radius: 20px; border: 3px solid rgba(200,120,255,0.60);");
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
    mapLabel->setFixedSize(600,600);
    mapLabel->setStyleSheet("background: transparent; border-radius: 12px;");
    mapLabel->setAlignment(Qt::AlignCenter);
    mapaLayout->addWidget(mapLabel);

    mainLayout->addWidget(panelMapa);

    connect(mapLabel, &ClickableImage::leftClick, this, &EditorMenu::onLeftClick);
    connect(mapLabel, &ClickableImage::rightClick, this, &EditorMenu::onRightClick);

    onCantidadRecorridosChanged(spin->value());
}

// ----------------- Cantidad de recorridos -----------------
void EditorMenu::onCantidadRecorridosChanged(int n)
{
    for (QPushButton* btn : botonesRecorridos) {
        lateralLayout->removeWidget(btn);
        btn->deleteLater();
    }
    botonesRecorridos.clear();
    recorridos.clear();

    for(int i=0;i<n;i++){
        QPushButton* btn = new QPushButton(QString("Recorrido %1").arg(i+1));
        btn->setStyleSheet("QPushButton { background-color: rgba(255,255,255,0.15); color:white; font-size:16px; padding:8px; border-radius:8px; } QPushButton:hover { background-color: rgba(255,255,255,0.3); }");
        lateralLayout->addWidget(btn);
        connect(btn, &QPushButton::clicked, [this,i](){ onSelectRecorrido(i); });
        botonesRecorridos.append(btn);
        recorridos.append(Recorrido());
    }

    // Resaltar seleccionado
    for(int i=0;i<botonesRecorridos.size();i++){
        if(i==recorridoActual){
            botonesRecorridos[i]->setStyleSheet("background-color: rgba(255,255,255,0.5); color:white; font-size:16px; padding:8px; border-radius:8px;");
        }
    }
}

// ----------------- Selección de recorrido -----------------
void EditorMenu::onSelectRecorrido(int index)
{
    recorridoActual = index;
    actualizarCheckpointsEnMapa();

    for(int i=0;i<botonesRecorridos.size();i++){
        if(i==recorridoActual){
            botonesRecorridos[i]->setStyleSheet("background-color: rgba(255,255,255,0.5); color:white; font-size:16px; padding:8px; border-radius:8px;");
        } else {
            botonesRecorridos[i]->setStyleSheet("QPushButton { background-color: rgba(255,255,255,0.15); color:white; font-size:16px; padding:8px; border-radius:8px; } QPushButton:hover { background-color: rgba(255,255,255,0.3); }");
        }
    }
}

// ----------------- Clicks -----------------
void EditorMenu::onLeftClick(int x,int y)
{
    if (recorridoActual<0) return;
    recorridos[recorridoActual].checkpoints.append(QPoint(x,y));
    actualizarCheckpointsEnMapa();
}

void EditorMenu::onRightClick()
{
    if (recorridoActual<0) return;
    if (!recorridos[recorridoActual].checkpoints.isEmpty()) {
        recorridos[recorridoActual].checkpoints.removeLast();
        actualizarCheckpointsEnMapa();
    }
}

// ----------------- Actualizar checkpoints -----------------
void EditorMenu::actualizarCheckpointsEnMapa()
{
    if (recorridoActual<0) return;
    mapLabel->clearPoints();

    const QVector<QPoint>& checkpoints = recorridos[recorridoActual].checkpoints;
    QVector<QPoint> ptsLabel;

    QSize labelSize = mapLabel->size();
    QPixmap currentPixmap = mapLabel->getOriginalPixmap();
    if (currentPixmap.isNull()) return;
    QSize imgScaled = currentPixmap.size();

    int offsetX = (labelSize.width()  - imgScaled.width())/2;
    int offsetY = (labelSize.height() - imgScaled.height())/2;

    float scaleX = (float)imgScaled.width()  / mapLabel->getOriginalPixmap().width();
    float scaleY = (float)imgScaled.height() / mapLabel->getOriginalPixmap().height();

    for (const QPoint &p : checkpoints) {
        int lx = p.x() * scaleX + offsetX;
        int ly = p.y() * scaleY + offsetY;
        ptsLabel.append(QPoint(lx, ly));
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
}

// ----------------- Guardar YAML -----------------
void EditorMenu::onSummit()
{
    for (int r = 0; r < recorridos.size(); r++) {
        const Recorrido &rec = recorridos[r];
        if (rec.checkpoints.size() < 2) {
            QMessageBox::warning(this, "Pocos checkpoints", QString("Hay menos de 2 checkpoints en el recorrido %1").arg(r + 1));
            return;
        }
    }

    const std::string basePath = "editor/BaseMap/BaseMapa" + selected_map.toStdString() + ".yaml";
    YAML::Node root;
    try {
        root = YAML::LoadFile(basePath);
    } catch (const std::exception &e) {
        qWarning() << "Error al cargar YAML:" << e.what();
        return;
    }

    YAML::Node layers = root["layers"];
    if (!layers || !layers.IsSequence()) return;

    int nextLayerId = root["nextlayerid"].as<int>();
    int nextObjectId = root["nextobjectid"].as<int>();

    for (int r = 0; r < recorridos.size(); r++)
    {
        const Recorrido &rec = recorridos[r];
        if (rec.checkpoints.isEmpty()) continue;

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
            obj["name"] = (i == 0) ? "Start" : (i == rec.checkpoints.size() - 1) ? "Finish" : "Normal";
            obj["type"] = "";
            obj["x"] = pt.x();
            obj["y"] = pt.y();
            obj["width"] = 0;
            obj["height"] = 0;
            obj["rotation"] = 0;
            obj["visible"] = true;
            obj["point"] = true;

            YAML::Node props(YAML::NodeType::Sequence);
            YAML::Node p1; p1["name"]="index"; p1["type"]="int"; p1["value"]=i; props.push_back(p1);
            YAML::Node p2; p2["name"]="race_id"; p2["type"]="string"; p2["value"]=std::string(1,raceId); props.push_back(p2);
            YAML::Node p3; p3["name"]="type"; p3["type"]="string";
            if(i==0) p3["value"]="start"; else if(i==rec.checkpoints.size()-1) p3["value"]="finish"; else p3["value"]="normal";
            props.push_back(p3);

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
        QMessageBox::warning(this, "Nombre vacío", "El nombre del archivo no puede estar vacío.");
        return;
    }

    std::string outPath = "editor/MapsEdited/" + nombreElegido.toStdString() + ".yaml";
    if (QFile::exists(QString::fromStdString(outPath))) {
        QMessageBox::warning(this, "Archivo existente", "Ya existe un archivo con ese nombre en editor/MapsEdited.\nElige otro nombre.");
        return;
    }

    std::ofstream file(outPath);
    if (!file.is_open()) return;
    file << emitter.c_str();
    file.close();
}
