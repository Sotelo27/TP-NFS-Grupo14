#include "editor_window.h"
#include <QVBoxLayout>
#include <QResizeEvent>

// Asegurate de incluir el header del EditorMenu
#include "editor_menu.h"

EditorWindow::EditorWindow(QWidget* parent)
    : QDialog(parent),
      stack(nullptr),
      editor_choose_map(nullptr),
      editor_menu(nullptr)
{
    setWindowTitle("Editor");
    setMinimumSize(800, 600);

    // --- Stacked widget ---
    stack = new QStackedWidget(this);

    // --- Pantallas ---
    editor_choose_map = new EditorChooseMap(this);
    editor_menu = new EditorMenu(this);   // <<-- IMPORTANTE: inicializarlo

    // --- Agregar páginas al stack ---
    stack->addWidget(editor_choose_map); // index 0
    stack->addWidget(editor_menu);       // index 1

    // --- Layout principal ---
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(stack);
    setLayout(layout);

    // --- Comenzar en la pantalla de selección (o la que quieras) ---
    stack->setCurrentWidget(editor_choose_map);

    // conectar señal para ir al editor
    connect(editor_choose_map, &EditorChooseMap::go_to_editor_screen,
            this, &EditorWindow::go_to_editor);
}

EditorWindow::~EditorWindow() {
    // Los children son borrados automáticamente por Qt; no es necesario delete explícito
}

void EditorWindow::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event);
    // No hacer resize en punteros no inicializados
    // if (editor_menu) editor_menu->resize(event->size());
}

void EditorWindow::go_to_map_selection() const
{
    stack->setCurrentWidget(editor_choose_map);
}

void EditorWindow::go_to_editor() const
{
    // Asegurarse de que editor_menu y editor_choose_map existan
    if (!editor_menu || !editor_choose_map) return;

    editor_menu->load_menu(
        editor_choose_map->get_selected_map(),
        editor_choose_map->get_selected_map_image()
    );

    stack->setCurrentWidget(editor_menu);
}
