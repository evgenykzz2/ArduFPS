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
        LevelSize_16x16 = 2,
        LevelSize_16x32 = 3
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
    std::map<int, QImage> m_object_tiles;
    int m_current_tile;

    int Size2Width(ELevelSize size);
    int Size2Height(ELevelSize size);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btn_toggle_full_tileset_clicked();
    void on_btn_save_clicked();
    void on_combo_level_size_currentIndexChanged(int index);
    void on_btn_export_clicked();
    void on_combo_level_type_currentIndexChanged(int index);
    void on_btn_level_add_clicked();
    void on_combo_level_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    virtual bool eventFilter( QObject* object, QEvent* event );

    void LoadTextures();
    void LoadJson();
    void SaveJson();
    void UpdateTileset();
    void RedrawLevel();
    void UpdateLevel();
    void GenerateLevel();

    void ConvertAllTextures();
    void ExportLevels(const char* file_name);
    void ConvertObjects(std::stringstream& stream, QString file_name, const char* name);
    void ConvertTextures(std::stringstream& stream, QString file_name, const char* name);
    void ConvertSpriteSet(std::stringstream& stream, QString file_name, const char* name, int sprite_width, int sprite_height);
    void ConvertSprite(std::stringstream& stream, QString file_name, const char* name);
    void ConvertMiniFont(std::stringstream& stream, QString file_name, const char* name);
};

#endif // MAINWINDOW_H
