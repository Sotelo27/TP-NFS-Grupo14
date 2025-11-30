#include "editor_choose_map.h"
#include "editor_window.h"
#include "iostream"
#include "QApplication"
#include "editor_window.h"

int main(int argc, char *argv[]) {
    if (argc != 1) {
        std::cerr << "No se permiten argumentos\n";
        return 1;
    }
    std::cout << "Ejecutando: " << argv[0] << "\n";

    int n = 0;
    QApplication app(n, nullptr);
    EditorWindow editor = EditorWindow();
    return editor.exec();
}