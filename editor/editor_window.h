#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <QDialog>
#include <QStackedWidget>
#include "editor_choose_map.h"
#include "editor_menu.h"

class EditorWindow : public QDialog {
    Q_OBJECT
public:
    explicit EditorWindow(QWidget* parent = nullptr);
    ~EditorWindow();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QStackedWidget *stack;

    // Las 2 pantallas
    EditorChooseMap *editor_choose_map;
    EditorMenu *editor_menu;

private slots:
    void go_to_map_selection() const;
    void go_to_editor() const;
};


#endif //EDITOR_WINDOW_H