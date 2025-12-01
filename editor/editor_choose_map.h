#ifndef EDITOR_CHOOSE_MAP_H
#define EDITOR_CHOOSE_MAP_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QResizeEvent>

class EditorChooseMap : public QWidget {
    Q_OBJECT

public:
    explicit EditorChooseMap(QWidget *parent = nullptr);
    ~EditorChooseMap() override = default;

    const QString& get_selected_map() const { return selected_map; }
    const QString& get_selected_map_image() const { return selected_map_image; }

    signals:
        void go_to_editor_screen();

public slots:
    void on_map_selected(const QString& map_name, const QString& img_path);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QString selected_map;
    QString selected_map_image;
    QLabel* background = nullptr;
};

#endif // EDITOR_CHOOSE_MAP_H
