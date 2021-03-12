#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdint.h>
#include <QImage>
#include <QMessageBox>
#include <sstream>
#include <iomanip>

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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

MainWindow::~MainWindow()
{
    delete ui;
}
