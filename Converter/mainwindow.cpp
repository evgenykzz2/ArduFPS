#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdint.h>
#include <QImage>
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>
#include <sstream>
#include <iomanip>
#include "picojson.h"

static const QColor s_color_select(0, 255, 0);
static const QColor s_color_unselect(0, 0, 192);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_current_tile(0)
{
    ui->setupUi(this);
    LoadTextures();
    LoadJson();

    ui->combo_level_size->addItem("8x8", QVariant((int)LevelSize_8x8));
    ui->combo_level_size->addItem("16x8", QVariant((int)LevelSize_16x8));
    ui->combo_level_size->addItem("16x16", QVariant((int)LevelSize_16x16));

    ui->combo_level_type->addItem("Custom", QVariant((int)LevelType_custom));
    ui->combo_level_type->addItem("Gen", QVariant((int)LevelType_gen_room));

    if (m_tileset_map.empty())
    {
        TileSet tileset;
        tileset.tiles.resize(16, 0);
        tileset.tiles[15] = 0xff;
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
        level.cell.resize(16*16, 15);
        m_level_map.insert(std::make_pair(0, level));
        ui->combo_level->addItem("0", QVariant((int)0));
    }
    UpdateTileset();
    UpdateLevel();
    ui->label_tileset->installEventFilter(this);
    ui->label_full_tile_set->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter( QObject* object, QEvent* event )
{
    if( object == ui->label_tileset && event->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent* mouse_event = (QMouseEvent*)event;
        if ( (int)(mouse_event->buttons() & Qt::LeftButton) != 0 )
        {
            int tile = mouse_event->x() / (16*4);
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
            if (m_current_tile != 15)
            {
                int current_index = ui->combo_tileset->itemData( ui->combo_tileset->currentIndex() ).toInt();
                auto tileset_itt = m_tileset_map.find(current_index);
                if (tileset_itt != m_tileset_map.end())
                    tileset_itt->second.tiles[m_current_tile] = tile_x + tile_y*16;
                UpdateTileset();
            }
        }
    }

    return QWidget::eventFilter( object, event );
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

void MainWindow::ConvertAllTextures()
{
    std::stringstream stream_cpp;
    stream_cpp << "#include \"Textures.h\"" << std::endl;
    stream_cpp << "#include <avr/pgmspace.h>" << std::endl;
    stream_cpp << std::endl;
    stream_cpp << "namespace ArduFPS" << std::endl;
    stream_cpp << "{" << std::endl;

    ConvertTextures(stream_cpp, "../assets/Textures.png", "g_texture");
    ConvertSpriteSet(stream_cpp, "../assets/Sprites.png", "g_weapon_sprites", 24, 24);

    stream_cpp << "}" << std::endl;
    FILE* file = fopen("../ArduFPS/Textures.cpp", "wb");
    fwrite(stream_cpp.str().c_str(), 1, stream_cpp.str().length(), file);
    fclose(file);
}

void MainWindow::LoadTextures()
{
    QImage img;
    if (!img.load("../assets/Textures.png"))
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
}

void MainWindow::LoadJson()
{
}

void MainWindow::SaveJson()
{
    picojson::value json;
    json.set<picojson::object>(picojson::object());

    picojson::array tileset_array;
    for (auto itt = m_tileset_map.begin(); itt != m_tileset_map.end(); ++itt)
    {
        picojson::array list;
        for (size_t i = 0; i < 16; ++i)
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
        for (size_t i = 0; i < 16*16; ++i)
            array.push_back(picojson::value((double)itt->second.cell[i]));
        level["cell"] = picojson::value(array);
        levels_array.push_back(picojson::value(level));
    }
    json.get<picojson::object>()["levels"] = picojson::value(levels_array);

    QString string( QString::fromStdString(json.serialize(true)) );
    QFile file("../assets/levels.json");
    file.open(QFile::WriteOnly);
    file.write(string.toLocal8Bit().data());
    file.close();
}

void MainWindow::UpdateTileset()
{
    int current_index = ui->combo_tileset->itemData( ui->combo_tileset->currentIndex() ).toInt();
    QImage image(16*16*4, 16*4, QImage::Format_ARGB32);
    image.fill(0xFFFFFFFF);
    auto tileset_itt = m_tileset_map.find(current_index);
    if (tileset_itt == m_tileset_map.end())
        return;
    {
        QPainter painter(&image);
        for (int i = 0; i < 16; ++i)
        {
            int tile_index = tileset_itt->second.tiles[i];
            auto itt = m_texture_tiles.find(tile_index);
            if (itt == m_texture_tiles.end())
            {
                painter.setPen(QColor(64, 64, 64));
                painter.setBrush(QColor(64, 64, 64));
                painter.drawRect(i*16*4, 0, 16*4-1, 16*4-1);
            } else
            {
                QImage img = itt->second.scaled(itt->second.width()*4, itt->second.height()*4);
                painter.drawImage(i*16*4, 0, img);
            }
            if (i == m_current_tile)
                painter.setPen(s_color_select);
            else
                painter.setPen(s_color_unselect);
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(i*16*4, 0, 16*4-1, 16*4-1);
        }
    }
    ui->label_tileset->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::on_btn_toggle_full_tileset_clicked()
{
    ui->label_full_tile_set->setVisible( !ui->label_full_tile_set->isVisible() );
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
}

void MainWindow::on_btn_save_clicked()
{
    SaveJson();
}
