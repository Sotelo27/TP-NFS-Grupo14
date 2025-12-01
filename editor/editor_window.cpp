#include "editor_window.h"
#include <QVBoxLayout>
#include <QResizeEvent>
#include "editor_menu.h"

EditorWindow::EditorWindow(QWidget* parent)
    : QDialog(parent),
      stack(nullptr),
      editor_choose_map(nullptr),
      editor_menu(nullptr)
{
    setWindowTitle("Editor");
    showMaximized();

    stack = new QStackedWidget(this);

    editor_choose_map = new EditorChooseMap(this);
    editor_menu = new EditorMenu(this);

    stack->addWidget(editor_choose_map);
    stack->addWidget(editor_menu);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(stack);
    setLayout(layout);

    stack->setCurrentWidget(editor_choose_map);

    connect(editor_choose_map, &EditorChooseMap::go_to_editor_screen,
            this, &EditorWindow::go_to_editor);

    connect(editor_menu, &EditorMenu::back_to_choose_map, this, [this]() {
        stack->setCurrentWidget(editor_choose_map);
    });
}

EditorWindow::~EditorWindow() {
}

void EditorWindow::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event);
}

void EditorWindow::go_to_map_selection() const
{
    stack->setCurrentWidget(editor_choose_map);
}

void EditorWindow::go_to_editor() const
{
    editor_menu->load_menu(
        editor_choose_map->get_selected_map(),
        editor_choose_map->get_selected_map_image()
    );

    stack->setCurrentWidget(editor_menu);
}
