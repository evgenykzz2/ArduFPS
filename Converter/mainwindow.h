#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <stdint.h>
#include <map>
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct TileSet
    {
        std::vector<int> tiles;
    };

    enum ElevelType
    {
        LevelType_custom = 0,
        LevelType_gen_room = 1
    };

    enum ELevelSize
    {
        LevelSize_8x8 = 0,
        LevelSize_16x8 = 1,
        LevelSize_16x16 = 2
    };

    struct Level
    {
        int tileset_index;
        std::string title;
        int index;
        ELevelSize level_size;
        ElevelType level_type;
        std::vector<int> cell;
    };

    std::map<int, TileSet> m_tileset_map;
    std::map<int, Level> m_level_map;
    std::map<int, QImage> m_texture_tiles;
    int m_current_tile;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btn_toggle_full_tileset_clicked();

    void on_btn_save_clicked();

private:
    Ui::MainWindow *ui;
    virtual bool eventFilter( QObject* object, QEvent* event );

    void LoadTextures();
    void LoadJson();
    void SaveJson();
    void UpdateTileset();
    void UpdateLevel();

    void ConvertAllTextures();
    void ConvertTextures(std::stringstream& stream, QString file_name, const char* name);
    void ConvertSpriteSet(std::stringstream& stream, QString file_name, const char* name, int sprite_width, int sprite_height);
    void ConvertSprite(std::stringstream& stream, QString file_name, const char* name);
};

#endif // MAINWINDOW_H
