#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdint.h>
#include <QImage>
#include <QFile>
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QMouseEvent>
#include <sstream>
#include <iomanip>
#include "picojson.h"

#define ASSETS_FOLDER "../assets/"
#define GAME_FOLDER "../ArduFPS/"

static const QColor s_color_select(0, 255, 0);
static const QColor s_color_unselect(0, 0, 192);
static const QColor s_color_empty(64, 64, 64);
static int s_map_scale = 1;

static const QColor s_item_color[32] =
{
    QColor(64, 64, 64),  QColor(64, 64, 64),  QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64),
    QColor(64, 129, 192), QColor(129, 192, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64), QColor(64, 64, 64)
};

static const int s_object_sprite[16] =
{
//  pistole
    0, 1, 2, 3, 6, 7, 14, 15,   8, 8, 8, 8, 8, 8, 8, 8
};

//encoding
//00 -> empty [15]
//01 -> base  [0]
//1+5 bit

#define MAP_WIDTH 16
#define MAP_HEIGHT 32

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_current_tile(0)
{
    ui->setupUi(this);
    qDebug() << "Start";
    LoadTextures();
    LoadJson();

    ui->combo_level_size->addItem("8x8", QVariant((int)LevelSize_8x8));
    ui->combo_level_size->addItem("16x8", QVariant((int)LevelSize_16x8));
    ui->combo_level_size->addItem("16x16", QVariant((int)LevelSize_16x16));
    ui->combo_level_size->addItem("16x32", QVariant((int)LevelSize_16x32));

    ui->combo_level_type->addItem("Custom", QVariant((int)LevelType_custom));
    ui->combo_level_type->addItem("Gen", QVariant((int)LevelType_gen_room));

    ui->label_full_tile_set->setVisible(false);

    if (m_tileset_map.empty())
    {
        TileSet tileset;
        tileset.tiles.resize(32, 0);
        m_tileset_map.insert(std::make_pair(0, tileset));
        ui->combo_tileset->addItem("0", QVariant((int)0));
    }
    if (m_level_map.empty())
    {
        Level level;
        level.index = 0;
        level.title = "None";
        level.tileset_index = 0;
        level.level_size = LevelSize_8x8;
        level.level_type = LevelType_custom;
        level.cell.resize(MAP_WIDTH*MAP_HEIGHT, 0);
        m_level_map.insert(std::make_pair(0, level));
        ui->combo_level->addItem("0", QVariant((int)0));
    }

    UpdateTileset();
    UpdateLevel();
    ui->label_tileset->installEventFilter(this);
    ui->label_full_tile_set->installEventFilter(this);
    ui->label_map_view->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::Size2Width(MainWindow::ELevelSize size)
{
    switch (size)
    {
    case LevelSize_8x8:
        return 8;
    case LevelSize_16x8:
        return 16;
    case LevelSize_16x16:
        return 16;
    case LevelSize_16x32:
        return 16;
    }
    return 8;
}

int MainWindow::Size2Height(MainWindow::ELevelSize size)
{
    switch (size)
    {
    case LevelSize_8x8:
        return 8;
    case LevelSize_16x8:
        return 8;
    case LevelSize_16x16:
        return 16;
    case LevelSize_16x32:
        return 32;
    }
    return 8;
}

bool MainWindow::eventFilter( QObject* object, QEvent* event )
{
    if( object == ui->label_tileset && event->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent* mouse_event = (QMouseEvent*)event;
        if ( (int)(mouse_event->buttons() & Qt::LeftButton) != 0 )
        {
            int tile_x = mouse_event->x() / (16*4);
            int tile_y = mouse_event->y() / (16*4);
            int tile = tile_x + tile_y*16;
            if (tile != m_current_tile)
            {
                m_current_tile = tile;
                UpdateTileset();
            }
        }
    }

    if( object == ui->label_full_tile_set && event->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent* mouse_event = (QMouseEvent*)event;
        if ( (int)(mouse_event->buttons() & Qt::LeftButton) != 0 )
        {
            int tile_x = mouse_event->x() / (16*4);
            int tile_y = mouse_event->y() / (16*4);
            //if (m_current_tile != 15)
            {
                int current_index = ui->combo_tileset->itemData( ui->combo_tileset->currentIndex() ).toInt();
                auto tileset_itt = m_tileset_map.find(current_index);
                if (tileset_itt != m_tileset_map.end())
                    tileset_itt->second.tiles[m_current_tile] = tile_x + tile_y*16;
                UpdateTileset();
                UpdateLevel();
            }
        }
    }

    if( object == ui->label_map_view && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseMove) )
    {
        QMouseEvent* mouse_event = (QMouseEvent*)event;
        if ( (int)(mouse_event->buttons() & Qt::LeftButton) != 0 )
        {
            int tile_x = mouse_event->x() / (16*s_map_scale);
            int tile_y = mouse_event->y() / (16*s_map_scale);
            if (tile_x > MAP_WIDTH-1)
                tile_x = MAP_WIDTH-1;
            if (tile_y > MAP_HEIGHT-1)
                tile_y = MAP_HEIGHT-1;
            int current_index = ui->combo_level->itemData( ui->combo_level->currentIndex() ).toInt();
            auto level_itt = m_level_map.find(current_index);
            if (level_itt != m_level_map.end())
            {
                if (m_current_tile != level_itt->second.cell[tile_x + tile_y*MAP_WIDTH])
                {
                    level_itt->second.cell[tile_x + tile_y*MAP_WIDTH] = m_current_tile;
                    UpdateLevel();
                }
            }
        }
    }

    return QWidget::eventFilter( object, event );
}

void MainWindow::ExportLevels(const char* file_name)
{
    std::stringstream stream_cpp;
    stream_cpp << "#include \"Levels.h\"" << std::endl;
    stream_cpp << "#include <avr/pgmspace.h>" << std::endl;
    stream_cpp << std::endl;
    stream_cpp << "namespace ArduFPS" << std::endl;
    stream_cpp << "{" << std::endl;

    stream_cpp << "static const uint8_t Level::s_tile_set[] PROGMEM =" << std::endl;
    stream_cpp << "{" << std::endl;
    for (auto itt = m_tileset_map.begin(); itt != m_tileset_map.end(); ++itt)
    {
        stream_cpp << "  ";
        for (int i = 0; i < 16; ++i)
            stream_cpp << "0x" << std::hex << itt->second.tiles[i] << ",";
        stream_cpp << std::endl;
    }
    stream_cpp << "};" << std::endl << std::endl;

    stream_cpp << "static const uint8_t Level::s_level[] PROGMEM =" << std::endl;
    stream_cpp << "{" << std::endl;

    int level_number = 0;
    for (auto itt = m_level_map.begin(); itt != m_level_map.end(); ++itt)
    {
        stream_cpp << "  //Level " << std::dec << level_number + 1 << std::endl;
        int level_desc = (int)itt->second.level_size | ((int)itt->second.level_type << 2);
        stream_cpp << "  0x" << std::hex << level_desc << ",   ";

        if ( itt->second.level_type == LevelType_custom)
        {
            int w = Size2Width(itt->second.level_size);
            int h = Size2Height(itt->second.level_size);
            uint32_t bit_buffer = 0;
            int bit_pos = 0;
            for (int y = 0; y < h; ++y)
            {
                for (int x = 0; x < w; ++x)
                {
                    int index = x + y * MAP_WIDTH;
                    int cell = itt->second.cell[index];
                    if (cell == 0)
                    {
                        bit_buffer |= (0) << bit_pos;
                        bit_pos += 2;
                    } else if (cell == 1)
                    {
                        bit_buffer |= (2) << bit_pos;
                        bit_pos += 2;
                    } else
                    {
                        bit_buffer |= ((cell << 1) | 1) << bit_pos;
                        bit_pos += 6;
                    }
                    if (bit_pos >= 8)
                    {
                        stream_cpp << "0x" << std::hex << (int)(bit_buffer & 0xFF) << ",";
                        bit_buffer >>= 8;
                        bit_pos -= 8;
                    }
                }
            }
            if (bit_pos != 0)
                stream_cpp << "0x" << std::hex << (int)(bit_buffer & 0xFF) << ",";
        }
        stream_cpp << std::endl;
        level_number ++;
    }
    //Size2Width

    stream_cpp << "};" << std::endl << std::endl;

    stream_cpp << "}" << std::endl;
    FILE* file = fopen(file_name, "wb");
    fwrite(stream_cpp.str().c_str(), 1, stream_cpp.str().length(), file);
    fclose(file);
}

void MainWindow::ConvertTextures(std::stringstream& stream, QString file_name, const char* name)
{
    QImage img;
    if (!img.load(file_name))
    {
        QMessageBox::critical(this, "Converter", "Can't load assets");
        return;
    }
    img = img.convertToFormat(QImage::Format_ARGB32);

    stream << std::endl;
    stream << "const uint16_t " << name << "[] PROGMEM =" << std::endl;
    stream << "{" << std::endl;
    int index = 0;
    for (int yi = 0; yi < img.height()/16; ++yi)
    {
        for (int xi = 0; xi < img.width()/16; ++xi)
        {
            for (int x = 0; x < 16; ++x)
            {
                uint16_t value = 0;
                for (int y = 0; y < 16; ++y)
                {
                    uint32_t color = img.pixel(x + xi*16, y + yi*16);
                    if ( (color & 0xFF) > 128)
                        value |= 1 << y;
                }

                if (index % 16 == 0)
                    stream << "  ";
                stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << (((int)value)&0xFFFF) << ",";
                if (index % 16 == 15)
                    stream << std::endl;
                index ++;
            }
        }
    }
    stream << std::endl <<"};" << std::endl;
}

void MainWindow::ConvertSpriteSet(std::stringstream& stream, QString file_name, const char* name, int sprite_width, int sprite_height)
{
    QImage sprites;
    if (!sprites.load(file_name))
    {
        QMessageBox::critical(this, "Converter", "Can't load spriteset");
        return;
    }
    sprites = sprites.convertToFormat(QImage::Format_ARGB32);

    stream << std::endl;
    stream << "const uint8_t " << name << "[] PROGMEM =" << std::endl;
    stream << "{" << std::endl;
    int index = 0;
    for (int yi = 0; yi < sprites.height()/sprite_height; ++yi)
    {
        for (int xi = 0; xi < sprites.width()/sprite_width; ++xi)
        {
            if (index % 32 == 0)
                stream << "  ";
            stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << 24 << ",";
            if (index % 32 == 31)
                stream << std::endl;
            index ++;

            if (index % 32 == 0)
                stream << "  ";
            stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << 24 << ",";
            if (index % 32 == 31)
                stream << std::endl;
            index ++;

            for (int yb = 0; yb < sprite_height/8; ++yb)
            {
                for (int x = 0; x < sprite_width; ++x)
                {
                    uint8_t value = 0;
                    uint8_t mask = 0;
                    for (int y = 0; y < 8; ++y)
                    {
                        uint32_t color = sprites.pixel(x + xi*sprite_width, y + yi*sprite_height + yb*8);
                        uint8_t r = color & 0xFF;
                        uint8_t g = (color >> 8) & 0xFF;
                        uint8_t b = (color >> 16) & 0xFF;

                        if ( r > 200 && g > 200 && b > 200)
                        {
                            value |= 1 << y;
                            mask |= 1 << y;
                        }
                        if ( r < 32 && g < 32 && b < 32)
                            mask |= 1 << y;
                    }
                    if (index % 32 == 0)
                        stream << "  ";
                    stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << (((int)value)&0xFF) << ",";
                    if (index % 32 == 31)
                        stream << std::endl;
                    index ++;

                    if (index % 32 == 0)
                        stream << "  ";
                    stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << (((int)mask)&0xFF) << ",";
                    if (index % 32 == 31)
                        stream << std::endl;
                    index ++;
                }
            }
        }
    }
    stream << std::endl <<"};" << std::endl;
}

void MainWindow::ConvertObjects(std::stringstream& stream, QString file_name, const char* name)
{
    QImage sprites;
    if (!sprites.load(file_name))
    {
        QMessageBox::critical(this, "Converter", "Can't load spriteset");
        return;
    }
    sprites = sprites.convertToFormat(QImage::Format_ARGB32);

    stream << std::endl;
    stream << "const uint16_t " << name << "[] PROGMEM =" << std::endl;
    stream << "{" << std::endl;
    int index = 0;
    for (int yi = 0; yi < sprites.height()/16; ++yi)
    {
        for (int xi = 0; xi < sprites.width()/16; ++xi)
        {
            for (int x = 0; x < 16; ++x)
            {
                uint16_t value = 0;
                uint16_t mask = 0;
                for (int y = 0; y < 16; ++y)
                {
                    uint32_t color = sprites.pixel(x + xi*16, y + yi*16);
                    uint8_t r = color & 0xFF;
                    uint8_t g = (color >> 8) & 0xFF;
                    uint8_t b = (color >> 16) & 0xFF;
                    if ( r > 200 && g > 200 && b > 200)
                    {
                        value |= 1 << y;
                        mask |= 1 << y;
                    }
                    if ( r < 32 && g < 32 && b < 32)
                        mask |= 1 << y;
                }
                if (index % 32 == 0)
                    stream << "  ";
                stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << (((int)value)&0xFFFF) << ",";
                if (index % 32 == 31)
                    stream << std::endl;
                index ++;

                if (index % 32 == 0)
                    stream << "  ";
                stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << (((int)mask)&0xFFFF) << ",";
                if (index % 32 == 31)
                    stream << std::endl;
                index ++;
            }
        }
    }
    stream << std::endl <<"};" << std::endl;
}

void MainWindow::ConvertMiniFont(std::stringstream& stream, QString file_name, const char* name)
{
    QImage sprites;
    if (!sprites.load(file_name))
    {
        QMessageBox::critical(this, "Converter", "Can't load spriteset");
        return;
    }
    sprites = sprites.convertToFormat(QImage::Format_ARGB32);

    stream << std::endl;
    stream << "const uint8_t " << name << "[] PROGMEM =" << std::endl;
    stream << "{" << std::endl;
    int index = 0;
    for (int x = 0; x < sprites.width(); ++x)
    {
        uint8_t value = 0;
        for (int y = 0; y < 8; ++y)
        {
            uint32_t color = sprites.pixel(x, y);
            uint8_t r = color & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = (color >> 16) & 0xFF;
            if ( r > 200 && g > 200 && b > 200)
                value |= 1 << y;
        }
        if (index % 32 == 0)
            stream << "  ";
        stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << (((int)value)&0xFF) << ",";
        if (index % 32 == 31)
            stream << std::endl;
        index ++;
    }
    stream << std::endl <<"};" << std::endl << std::endl;
}

void MainWindow::ConvertSprite(std::stringstream& stream, QString file_name, const char* name)
{
    QImage sprites;
    if (!sprites.load(file_name))
    {
        QMessageBox::critical(this, "Converter", "Can't load image");
        return;
    }
    sprites = sprites.convertToFormat(QImage::Format_ARGB32);

    stream << std::endl;
    stream << "const uint8_t " << name << "[] PROGMEM =" << std::endl;
    stream << "{" << std::endl;

    int index = 0;
    if (index % 32 == 0)
        stream << "  ";
    stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << sprites.width() << ",";
    if (index % 32 == 31)
        stream << std::endl;
    index ++;

    if (index % 32 == 0)
        stream << "  ";
    stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << sprites.height() << ",";
    if (index % 32 == 31)
        stream << std::endl;
    index ++;

    for (int yi = 0; yi < sprites.height()/8; ++yi)
    {
        for (int x = 0; x < sprites.width(); ++x)
        {
            uint8_t value = 0;
            uint8_t mask = 0;
            for (int y = 0; y < 8; ++y)
            {
                uint32_t color = sprites.pixel(x, y + yi*8);
                uint8_t r = color & 0xFF;
                uint8_t g = (color >> 8) & 0xFF;
                uint8_t b = (color >> 16) & 0xFF;

                if ( r > 200 && g > 200 && b > 200)
                {
                    value |= 1 << y;
                    mask |= 1 << y;
                }
                if ( r < 32 && g < 32 && b < 32)
                    mask |= 1 << y;
            }
            if (index % 32 == 0)
                stream << "  ";
            stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << (((int)value)&0xFF) << ",";
            if (index % 32 == 31)
                stream << std::endl;
            index ++;

            if (index % 32 == 0)
                stream << "  ";
            stream << "0x" << std::hex << std::setw(2) << std::setfill('0') << (((int)mask)&0xFF) << ",";
            if (index % 32 == 31)
                stream << std::endl;
            index ++;
        }
    }
    stream << std::endl <<"};" << std::endl;
}

static const int8_t s_minifont_width[46] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,   //0..9
    3, 3, 3, 3, 3, 3, 3,            //; : <>
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, //ABCDEFGHIJKL
    5, 4,                           //M N
    3, 3, 4,                        //O P Q
    3, 3, 3, 3, 3, 5, 3, 3, 3, 3, 3, 3 //RSTUVWXYZ[\]
};

void MainWindow::ConvertAllTextures()
{
    std::stringstream stream_cpp;
    stream_cpp << "#include \"Textures.h\"" << std::endl;
    stream_cpp << "#include <avr/pgmspace.h>" << std::endl;
    stream_cpp << std::endl;
    stream_cpp << "namespace ArduFPS" << std::endl;
    stream_cpp << "{" << std::endl;

    ConvertTextures(stream_cpp, ASSETS_FOLDER "Textures.png", "g_texture");
    ConvertSpriteSet(stream_cpp, ASSETS_FOLDER "Sprites.png", "g_weapon_sprites", 24, 24);
    ConvertObjects(stream_cpp, ASSETS_FOLDER "objects.png", "g_objects");
    ConvertMiniFont(stream_cpp, ASSETS_FOLDER "microfont.png", "g_minifont");

    {
        stream_cpp << "const uint8_t g_minifont_width[] PROGMEM =" << std::endl;
        stream_cpp << "{" << std::endl << "  ";
        for (size_t i = 0; i < sizeof(s_minifont_width); ++i)
            stream_cpp << "0x" << std::hex << (((int)s_minifont_width[i]) & 0xFF) << ",";
        stream_cpp << std::endl << "};" << std::endl << std::endl;
    }

    {
        stream_cpp << "const uint8_t g_minifont_offset[] PROGMEM =" << std::endl;
        stream_cpp << "{" << std::endl << "  ";
        int pos = 0;
        for (size_t i = 0; i < sizeof(s_minifont_width); ++i)
        {
            stream_cpp << "0x" << std::hex << (pos & 0xFF) << ",";
            pos += (int)s_minifont_width[i];
        }
        stream_cpp << std::endl << "};" << std::endl << std::endl;
    }

    stream_cpp << "}" << std::endl;
    FILE* file = fopen(GAME_FOLDER "Textures.cpp", "wb");
    fwrite(stream_cpp.str().c_str(), 1, stream_cpp.str().length(), file);
    fclose(file);
}

void MainWindow::LoadTextures()
{
    QImage img;
    if (!img.load(ASSETS_FOLDER"Textures.png"))
    {
        qDebug() << "Can't load Textures";
        QMessageBox::critical(0, "Converter", "Can't load assets");
        return;
    }
    img = img.convertToFormat(QImage::Format_ARGB32);

    int cell_width = img.width() / 16;
    int cell_height = img.height() / 16;

    int index = 0;
    for (int yi = 0; yi < cell_height; ++yi)
    {
        for (int xi = 0; xi < cell_width; ++xi)
        {
            QRect rect(xi*16, yi*16, 16, 16);
            QImage cropped = img.copy(rect);
            m_texture_tiles.insert(std::make_pair(index, cropped));
            index ++;
        }
    }

    img = img.scaled(img.width()*4, img.height()*4);
    {
        QPainter painter(&img);
        for (int yi = 0; yi < cell_height; ++yi)
        {
            for (int xi = 0; xi < cell_width; ++xi)
            {
                painter.setPen(s_color_unselect);
                painter.drawRect(xi*16*4, yi*16*4, 16*4-1, 16*4-1);
            }
        }
    }
    ui->label_full_tile_set->setPixmap(QPixmap::fromImage(img));

    {
        QImage img;
        if (!img.load(ASSETS_FOLDER"objects.png"))
        {
            QMessageBox::critical(this, "Converter", "Can't load assets");
            return;
        }
        img = img.convertToFormat(QImage::Format_ARGB32);

        int cell_width = img.width() / 16;
        int cell_height = img.height() / 16;

        int index = 0;
        for (int yi = 0; yi < cell_height; ++yi)
        {
            for (int xi = 0; xi < cell_width; ++xi)
            {
                QRect rect(xi*16, yi*16, 16, 16);
                QImage cropped = img.copy(rect);
                m_object_tiles.insert(std::make_pair(index, cropped));
                index ++;
            }
        }
    }
}

void MainWindow::LoadJson()
{
    QFile file(ASSETS_FOLDER"levels.json");
    if (!file.open(QFile::ReadOnly))
    {
        QMessageBox::critical(0, "Error", "Can't load json file");
        return;
    }
    QByteArray data = file.readAll();
    if (data.size() == 0)
        return;

    picojson::value json;
    picojson::parse(json, data.constData());

    picojson::array tileset_array = json.get<picojson::object>()["tileset"].get<picojson::array>();
    for ( auto itt = tileset_array.begin(); itt != tileset_array.end(); ++itt)
    {
        picojson::array list = itt->get<picojson::array>();
        TileSet tile_set;
        tile_set.tiles.resize(32);
        for (size_t i = 0; i < 32; ++i)
            tile_set.tiles[i] = (int)( list[i].get<double>() );
        ui->combo_tileset->addItem(QString("%1").arg((int)m_tileset_map.size()));
        m_tileset_map.insert(std::make_pair((int)m_tileset_map.size(), tile_set));
    }

    picojson::array level_array = json.get<picojson::object>()["levels"].get<picojson::array>();
    for ( auto itt = level_array.begin(); itt != level_array.end(); ++itt)
    {
        picojson::object level_obj = itt->get<picojson::object>();
        Level level;
        level.tileset_index = (int)level_obj["tileset"].get<double>();
        level.title = level_obj["title"].get<std::string>();
        level.index = (int)level_obj["index"].get<double>();
        level.level_size = (ELevelSize)((int)level_obj["size"].get<double>());
        level.level_type = (ElevelType)((int)level_obj["type"].get<double>());
        picojson::array list = level_obj["cell"].get<picojson::array>();
        level.cell.resize(16*32);
        for (size_t i = 0; i < 16*32; ++i)
            level.cell[i] = (int)( list[i].get<double>() );
        ui->combo_level->addItem(QString("%1").arg((int)m_level_map.size()), QVariant((int)m_level_map.size()));
        m_level_map.insert(std::make_pair((int)m_level_map.size(), level));
    }
}

void MainWindow::SaveJson()
{
    picojson::value json;
    json.set<picojson::object>(picojson::object());

    picojson::array tileset_array;
    for (auto itt = m_tileset_map.begin(); itt != m_tileset_map.end(); ++itt)
    {
        picojson::array list;
        for (size_t i = 0; i < 32; ++i)
            list.push_back(picojson::value((double)itt->second.tiles[i]));
        tileset_array.push_back(picojson::value(list));
    }
    json.get<picojson::object>()["tileset"] = picojson::value(tileset_array);


    picojson::array levels_array;
    for (auto itt = m_level_map.begin(); itt != m_level_map.end(); ++itt)
    {
        picojson::object level;
        level["tileset"] = picojson::value((double)itt->second.tileset_index);
        level["title"] = picojson::value(itt->second.title);
        level["index"] = picojson::value((double)itt->second.index);
        level["size"] = picojson::value((double)itt->second.level_size);
        level["type"] = picojson::value((double)itt->second.level_type);
        picojson::array array;
        for (size_t i = 0; i < 16*32; ++i)
            array.push_back(picojson::value((double)itt->second.cell[i]));
        level["cell"] = picojson::value(array);
        levels_array.push_back(picojson::value(level));
    }
    json.get<picojson::object>()["levels"] = picojson::value(levels_array);

    QString string( QString::fromStdString(json.serialize(false)) );
    QFile file(ASSETS_FOLDER"levels.json");
    file.open(QFile::WriteOnly);
    file.write(string.toLocal8Bit().data());
    file.close();
}

void DrawTextBorder(QPainter& painter, int x, int y, const QString &text)
{
    painter.setBrush(Qt::NoBrush);
    painter.setFont(QFont("Arial", 12));

    painter.setPen(QColor(0, 0, 0));
    painter.drawText(x-1, y, text);
    painter.drawText(x+1, y, text);
    painter.drawText(x, y-1, text);
    painter.drawText(x, y+1, text);
    painter.drawText(x-1, y-1, text);
    painter.drawText(x+1, y-1, text);
    painter.drawText(x-1, y+1, text);
    painter.drawText(x+1, y+1, text);

    painter.setPen(QColor(0, 255, 0));
    painter.drawText(x, y, text);
}

void MainWindow::UpdateTileset()
{
    int current_index = ui->combo_tileset->itemData( ui->combo_tileset->currentIndex() ).toInt();
    QImage image(16*16*4, 2*16*4, QImage::Format_ARGB32);
    image.fill(0xFFFFFFFF);
    auto tileset_itt = m_tileset_map.find(current_index);
    if (tileset_itt == m_tileset_map.end())
        return;
    {
        QPainter painter(&image);
        for (int y = 0; y < 2; ++y)
        {
            for (int x = 0; x < 16; ++x)
            {
                int index = x + y*16;
                if (index <= 17)
                {
                    int tile_index = tileset_itt->second.tiles[index];
                    auto itt = m_texture_tiles.find(tile_index);
                    if (itt == m_texture_tiles.end() || index == 16 || index == 17)
                    {
                        painter.setPen(s_item_color[index]);
                        painter.setBrush(s_item_color[index]);
                        painter.drawRect(x*16*4, y*16*4, 16*4-1, 16*4-1);
                    } else
                    {
                        QImage img = itt->second.scaled(itt->second.width()*4, itt->second.height()*4);
                        painter.drawImage(x*16*4, y*16*4, img);
                    }
                } else
                {
                    int tile_index = s_object_sprite[index-18];
                    auto itt = m_object_tiles.find(tile_index);
                    if (itt == m_object_tiles.end())
                    {
                        painter.setPen(s_item_color[index]);
                        painter.setBrush(s_item_color[index]);
                        painter.drawRect(x*16*4, y*16*4, 16*4-1, 16*4-1);
                    } else
                    {
                        QImage img = itt->second.scaled(itt->second.width()*4, itt->second.height()*4);
                        painter.drawImage(x*16*4, y*16*4, img);
                    }
                }
                //if (i == 0)
                {
                    if (index == 0)
                        DrawTextBorder(painter, x*16*4+4, y*16*4 + 42, "EMPTY");
                    if (index == 1)
                        DrawTextBorder(painter, x*16*4+10, y*16*4 + 42, "BASE");
                    if (index == 16)
                        DrawTextBorder(painter, x*16*4+4, y*16*4 + 42, "START");
                    if (index == 17)
                        DrawTextBorder(painter, x*16*4+4, y*16*4 + 42, "FINISH");
                }
                if (index == m_current_tile)
                    painter.setPen(s_color_select);
                else
                    painter.setPen(s_color_unselect);
                painter.setBrush(Qt::NoBrush);
                painter.drawRect(x*16*4, y*16*4, 16*4-1, 16*4-1);
            }
        }
    }
    ui->label_tileset->setPixmap(QPixmap::fromImage(image));
    ui->label_tileset->setMinimumSize(image.width(), image.height());
}

void MainWindow::on_btn_toggle_full_tileset_clicked()
{
    ui->label_full_tile_set->setVisible( !ui->label_full_tile_set->isVisible() );
}

void MainWindow::RedrawLevel()
{
    int current_index = ui->combo_level->itemData( ui->combo_level->currentIndex() ).toInt();
    auto level_itt = m_level_map.find(current_index);
    if (level_itt == m_level_map.end())
        return;

    int tileset_index = ui->combo_tileset->itemData( ui->combo_tileset->currentIndex() ).toInt();
    auto tileset_itt = m_tileset_map.find(tileset_index);
    if (tileset_itt == m_tileset_map.end())
        return;

    //Draw level
    int w = Size2Width(level_itt->second.level_size);
    int h = Size2Height(level_itt->second.level_size);

    if (level_itt->second.level_size == LevelSize_8x8 || level_itt->second.level_size == LevelSize_16x8)
        s_map_scale = 3;
    else if (level_itt->second.level_size == LevelSize_16x16)
        s_map_scale = 2;
    else
        s_map_scale = 1;

    QImage image(w*16*s_map_scale, h*16*s_map_scale, QImage::Format_ARGB32);
    image.fill(s_color_empty);
    {
        QPainter painter(&image);
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                int cell = level_itt->second.cell[x+y*MAP_WIDTH];
                if (cell <= 17)
                {
                    if (cell != 16 && cell != 17)
                    {
                        int tile_index = tileset_itt->second.tiles[cell];
                        auto itt = m_texture_tiles.find(tile_index);
                        if (itt != m_texture_tiles.end())
                        {
                            QImage img = itt->second.scaled(itt->second.width()*s_map_scale, itt->second.height()*s_map_scale);
                            painter.drawImage(x*16*s_map_scale, y*16*s_map_scale, img);
                        }
                    }
                    if (cell == 16 || cell == 17)
                    {
                        painter.setPen(s_item_color[cell]);
                        painter.setBrush(s_item_color[cell]);
                        painter.drawRect(x*16*s_map_scale, y*16*s_map_scale, 16*s_map_scale-1, 16*s_map_scale-1);
                    }
                } else
                {
                    int tile_index = s_object_sprite[cell-18];
                    auto itt = m_object_tiles.find(tile_index);
                    if (itt != m_object_tiles.end())
                    {
                        QImage img = itt->second.scaled(itt->second.width()*s_map_scale, itt->second.height()*s_map_scale);
                        painter.drawImage(x*16*s_map_scale, y*16*s_map_scale, img);
                    }
                }
                painter.setPen(s_color_unselect);
                painter.setBrush(Qt::NoBrush);
                painter.drawRect(x*16*s_map_scale, y*16*s_map_scale, 16*s_map_scale-1, 16*s_map_scale-1);
            }
        }
    }
    ui->label_map_view->setPixmap(QPixmap::fromImage(image));
    ui->label_map_view->setMinimumSize(image.width(), image.height());
}

void MainWindow::UpdateLevel()
{
    int current_index = ui->combo_level->itemData( ui->combo_level->currentIndex() ).toInt();
    auto level_itt = m_level_map.find(current_index);
    if (level_itt == m_level_map.end())
        return;
    ui->combo_level_size->setCurrentIndex((int)level_itt->second.level_size);
    ui->combo_level_type->setCurrentIndex((int)level_itt->second.level_type);
    ui->edit_level_title->setText(QString::fromStdString(level_itt->second.title));
    ui->edit_level_title_index->setText(QString("%1").arg(level_itt->second.index));
    RedrawLevel();
}

class Random
{
private:
    uint32_t m_seed;

public:
    Random( uint32_t seed = 0 ) :
      m_seed(seed == 0 ? (uint32_t)time(0) : seed)
    {}

    uint32_t Random::Get()
    {
        m_seed = ( 214013*m_seed + 2531011 );
        return ( m_seed>>16 ) & 0x7FFF;
    }
};

void MainWindow::GenerateLevel()
{
    int level_index = ui->combo_level->itemData( ui->combo_level->currentIndex() ).toInt();
    auto level_itt = m_level_map.find(level_index);
    if (level_itt == m_level_map.end())
        return;

    int level_w = Size2Width(level_itt->second.level_size);
    int level_h = Size2Height(level_itt->second.level_size);

    Random rand(0);
    for (int y = 0; y < level_h; ++y)
    {
        for (int x = 0; x < level_w; ++x)
        {
            int index = x + y * MAP_WIDTH;
            level_itt->second.cell[index] = 1;
        }
    }

    //Room subdivide
    for (uint8_t n = 0; n < 16; ++n)
    {
        bool room_success = false;
        for (uint8_t retry = 0; retry < 128; ++retry)
        {
            int8_t x, y, w, h;
            if (n == 0)
            {
                for (int8_t yi = 1; yi < level_h-1; ++yi)
                {
                    for (int8_t xi = 1; xi < level_w-1; ++xi)
                    {
                        level_itt->second.cell[(int16_t)xi + (int16_t)yi * MAP_WIDTH] = 16+n;
                    }
                }
                room_success = true;
                break;
            } else
            {
                uint8_t room_base = rand.Get() % n;
                int8_t x0 = level_w;
                int8_t y0 = level_h;
                int8_t x1 = 0;
                int8_t y1 = 0;
                for (int8_t yi = 0; yi < level_h; ++yi)
                {
                    for (int8_t xi = 0; xi < level_w; ++xi)
                    {
                        uint8_t cell = level_itt->second.cell[(int16_t)xi + (int16_t)yi * MAP_WIDTH];
                        if (cell != 16+room_base)
                            continue;
                        if (xi < x0) x0 = xi;
                        if (xi > x1) x1 = xi;
                        if (yi < y0) y0 = yi;
                        if (yi > y1) y1 = yi;
                    }
                }
                x = x0;
                y = y0;
                w = x1-x0+1;
                h = y1-y0+1;
            }
            uint8_t direction = rand.Get() & 1;
            if (direction == 0)
            {
                //Horizontal cut
                if (w < 5)
                    continue;
                int8_t cut;
                if (w > 8)
                    cut = 3 + (rand.Get() % (w - 6));
                else
                    cut = 2 + (rand.Get() % (w - 4));
                for (int8_t yi = 0; yi < h; ++yi)
                {
                    for (int8_t xi = 0; xi < cut; ++xi)
                    {
                        level_itt->second.cell[(int16_t)xi + x + (int16_t)(yi + y) * MAP_WIDTH] = 16+n;
                    }
                    level_itt->second.cell[(int16_t)x + cut + (int16_t)(yi + y) * MAP_WIDTH] = 1;   //New wall
                }
                room_success = true;
            } else
            {
                //Vertical cut
                if (h < 5)
                    continue;
                int8_t cut;
                if (h > 8)
                    cut = 3 + (rand.Get() % (h - 6));
                else
                    cut = 2 + (rand.Get() % (h - 4));
                for (int8_t xi = 0; xi < w; ++xi)
                {
                    for (int8_t yi = 0; yi < cut; ++yi)
                    {
                        level_itt->second.cell[(int16_t)xi + x + (int16_t)(yi + y) * MAP_WIDTH] = 16+n;
                    }
                    level_itt->second.cell[(int16_t)x + xi + (int16_t)(y + cut) * MAP_WIDTH] = 1;   //New wall
                }
                room_success = true;
            }
            if(room_success)
                break;
        }

        if (!room_success)
        {
            //Complete
            break;
        }
    }

    /*
    //Room generator
    for (uint8_t n = 0; n < 32; ++n)
    {
        int8_t x, y, w, h;
        if (n == 0)
        {
            uint8_t side = rand.Get() & 3;
            w =  (rand.Get() % 2) + 2;  //2..3
            h =  (rand.Get() % 2) + 2;  //2..3
            if (side == 0)
            {
                x = 1;
                y = 1;
            } else if (side == 1)
            {
                x = 1;
                y = level_h - h - 1;
            } else if (side == 2)
            {
                x = level_w - w - 1;
                y = 1;
            } else if (side == 3)
            {
                x = level_w - w - 1;
                y = level_h - h - 1;
            }

            std::cerr << (int)x << " " << (int)y << " " << (int)w << " " << (int)h << std::endl;
            for (uint8_t yi = 0; yi < h; ++yi)
            {
                for (uint8_t xi = 0; xi < w; ++xi)
                {
                    int index = x + xi + (yi + y) * MAP_WIDTH;
                    level_itt->second.cell[index] = 16+n;
                }
            }
        } else
        {
            bool room_success = false;
            for (uint8_t retry = 0; retry < 128; ++retry)
            {
                uint8_t room_base = rand.Get() % n;
                int8_t x0 = level_w;
                int8_t y0 = level_h;
                int8_t x1 = 0;
                int8_t y1 = 0;
                for (int8_t yi = 0; yi < level_h; ++yi)
                {
                    for (int8_t xi = 0; xi < level_w; ++xi)
                    {
                        uint8_t cell = level_itt->second.cell[(int16_t)xi + (int16_t)yi * MAP_WIDTH];
                        if (cell != 16+room_base)
                            continue;
                        if (xi < x0) x0 = xi;
                        if (xi > x1) x1 = xi;
                        if (yi < y0) y0 = yi;
                        if (yi > y1) y1 = yi;
                    }
                }
                uint8_t side = rand.Get() & 3;
                w =  (rand.Get() % 2) + 2;  //2..3
                h =  (rand.Get() % 2) + 2;  //2..3
                int8_t x_door = 0;
                int8_t y_door = 0;
                if (side == 0)
                {
                    //Left
                    y_door = (int16_t)y0 + rand.Get() % (y1-y0);
                    x_door = x0 - 1;
                    x = x0 - w - 1;
                    y = y_door - (rand.Get() % h);
                } else if (side == 1)
                {
                    //Right
                    y_door = (int16_t)y0 + rand.Get() % (y1-y0);
                    x_door = x1 + 1;
                    x = x1 + 2;
                    y = y_door - (rand.Get() % h);
                } else if (side == 2)
                {
                    //Top
                    x_door = (int16_t)x0 + rand.Get() % (x1-x0);
                    x = x_door - (rand.Get() % w);
                    y = y0 - h - 1;
                    y_door = y0 - 1;
                } else if (side == 3)
                {
                    //Bottom
                    x_door = (int16_t)x0 + rand.Get() % (x1-x0);
                    x = x_door - (rand.Get() % w);
                    y = y1 + 2;
                    y_door = y1 + 1;
                }

                bool ok = true;
                for (int8_t yi = -1; yi <= h; ++yi)
                {
                    if ((int16_t)y+yi >= level_h || (int16_t)y+yi < 0)
                    {
                        ok = false;
                        break;
                    }
                    for (int8_t xi = -1; xi <= w; ++xi)
                    {
                        if ((int16_t)x+xi >= level_w || (int16_t)x+xi < 0)
                        {
                            ok = false;
                            break;
                        }
                        int index = (int16_t)x + (int16_t)xi + (yi + y) * MAP_WIDTH;
                        if (level_itt->second.cell[index] != 1)
                        {
                            ok = false;
                            break;
                        }
                    }
                }
                if (ok)
                {
                    std::cerr << (int)x << " " << (int)y << " " << (int)w << " " << (int)h << " ("
                              << (int)x0 << " " << (int)y0 << " " << (int)x1 << " " << (int)y1 << ")" << (int)room_base
                              << std::endl;
                    for (int8_t yi = 0; yi < h; ++yi)
                    {
                        for (int8_t xi = 0; xi < w; ++xi)
                        {
                            int index = (int16_t)x + (int16_t)xi + (int16_t)(yi + y) * MAP_WIDTH;
                            level_itt->second.cell[index] = 16+n;
                        }
                    }
                    level_itt->second.cell[x_door + y_door*MAP_WIDTH] = 0;
                    room_success = true;
                    break;
                }
            } //retry
            if (!room_success)
            {
                //Complete
                break;
            }
        }
    }*/
}

void MainWindow::on_btn_save_clicked()
{
    SaveJson();
}

void MainWindow::on_combo_level_size_currentIndexChanged(int)
{
    int current_index = ui->combo_level->itemData( ui->combo_level->currentIndex() ).toInt();
    auto level_itt = m_level_map.find(current_index);
    if (level_itt == m_level_map.end())
        return;
    level_itt->second.level_size = (ELevelSize)ui->combo_level_size->itemData(ui->combo_level_size->currentIndex()).toInt();
    RedrawLevel();
}

void MainWindow::on_btn_export_clicked()
{
    ExportLevels(GAME_FOLDER"LevelData.cpp");
    ConvertAllTextures();
}

void MainWindow::on_combo_level_type_currentIndexChanged(int)
{
    int current_index = ui->combo_level->itemData( ui->combo_level->currentIndex() ).toInt();
    auto level_itt = m_level_map.find(current_index);
    if (level_itt == m_level_map.end())
        return;
    level_itt->second.level_type = (ElevelType)ui->combo_level_type->itemData( ui->combo_level_type->currentIndex() ).toInt();
    if (level_itt->second.level_type == LevelType_gen_room)
    {
        GenerateLevel();
    }
    RedrawLevel();
}

void MainWindow::on_btn_level_add_clicked()
{
    Level level;
    level.index = 0;
    level.title = "None";
    level.tileset_index = 0;
    level.level_size = LevelSize_8x8;
    level.level_type = LevelType_custom;
    level.cell.resize(MAP_WIDTH*MAP_HEIGHT, 0);
    int index = m_level_map.rbegin()->first + 1;
    m_level_map.insert(std::make_pair(index, level));
    ui->combo_level->addItem(QString("%1").arg(index), QVariant((int)index));
    ui->combo_level->setCurrentIndex(ui->combo_level->count()-1);
}

void MainWindow::on_combo_level_currentIndexChanged(int)
{
    UpdateLevel();
}
