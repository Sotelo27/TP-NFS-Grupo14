#ifndef EDITOR_CHOSSE_MAP_H
#define EDITOR_CHOSSE_MAP_H

#include <QWidget>


class EditorChooseMap : public QWidget{
    Q_OBJECT
    public:
    EditorChooseMap(QWidget *parent = nullptr);
    ~EditorChooseMap(){};


    signals:
    void go_to_editor_screen();

    public slots:
    void on_map_selected(const QString& map_name, const QString& img_path);
    const QString& get_selected_map() const { return selected_map; }
    const QString& get_selected_map_image() const { return selected_map_image; }

private:
    QString selected_map;
    QString selected_map_image;
};


#endif //EDITOR_CHOSSE_MAP_H