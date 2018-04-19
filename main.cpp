#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const int trackWidth = 2000;
const int segmentLength = 200;
const float cameraDepth = 0.84f;

struct Line
{
    float x, y, z; //line center
    float X, Y, W; //screen coord
    float curvature, scale;

    Line() { curvature = x = y = z = 0; }

    void project(int camX, int camY, int camZ)
    {
        scale = cameraDepth / (z - camZ);
        X = (1 + scale * (x - camX)) * WINDOW_WIDTH / 2;
        Y = (1 - scale * (y - camY)) * WINDOW_HEIGHT / 2;
        W = scale * trackWidth * WINDOW_WIDTH / 2;
    }
};

void drawQuad(sf::RenderWindow &window, int nearMidPointX, int nearMidPointY,
              int nearWidth, int farMidPointX, int farMidPointY, int farWidth,
              sf::Color color = {0,255,0});

int main(int argc, char *argv[])
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH,WINDOW_HEIGHT), "Outrun");
    const int screenWidth = sf::VideoMode::getDesktopMode().width;
    const int screenHeight = sf::VideoMode::getDesktopMode().height;
    window.setPosition(sf::Vector2i((screenWidth - WINDOW_WIDTH) / 2,
                                    (screenHeight - WINDOW_HEIGHT) / 2));
    window.setFramerateLimit(60);

    sf::Texture bg;
    bg.loadFromFile("images/bg.png");
    bg.setRepeated(true);
    sf::Sprite sBackground(bg);
    sBackground.setTextureRect(sf::IntRect(0,0,5000,411));
    sBackground.setPosition(-2000,0);

    static const std::string pathToFont{"BRLNSR.TTF"};
    sf::Font font;
    font.loadFromFile(pathToFont);

    std::vector<Line> lines;
    lines.reserve(1600);

    for(int i = 0; i < 1600; ++i)
    {
        Line line;
        line.z = i * segmentLength;
        if(i >= 300 && i < 700){
            line.curvature = 0.5f;
        } else if(i > 1100){
            line.curvature = -0.7;
        }
        if (i>750)
        {
            line.y = sin(i/30.0)*1500;
        }
        lines.push_back(line);
    }

    int N = lines.size();
    int pos = 0;
    float playerX = 0;
    int H = 1500;


    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if(event.type == sf::Event::MouseWheelScrolled)
            {
               float delta = event.mouseWheelScroll.delta;
               if(delta > 0)
               {
                   //cameraY += 10;
               } else if( delta < 0){
                   //cameraY -= 10;
               }
            }
            else if(event.type == sf::Event::MouseButtonReleased)
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    std::cout << " mx = " << event.mouseButton.x << " my = "
                              << event.mouseButton.y << std::endl;
                }
            }
        }

        int speed = 0;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) playerX -= 0.1;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) playerX += 0.1;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            speed = 200;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            speed = -200;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) H+=100;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) H-=100;

        pos += speed;
        while(pos >= N * segmentLength) pos -= N * segmentLength;
        while(pos < 0) pos += N * segmentLength;
        int startPos = pos / segmentLength;
        int camH = lines[startPos].y + H;
        if (speed>0)
        {
            sBackground.move(-lines[startPos].curvature*2,0);
        }
        else if (speed<0)
        {
            sBackground.move( lines[startPos].curvature*2,0);
        }
        int maxy = WINDOW_HEIGHT;
        float x = 0,dx = 0;
        window.clear(sf::Color(0,0,0));
        window.draw(sBackground);
        //drawQuad(window, 500, 500, 200, 500, 300, 100);
        //draw road
        for(int n = startPos; n < startPos + 300; ++n){
            //std::cout << "n = " << n << " n%N = " << (n%N) << std::endl;
            Line &l = lines[n%N]; // Mod operation keeps array index in bounds
            l.project(playerX * trackWidth - x,camH, startPos * segmentLength - (n >= N ? N * segmentLength : 0));
            x += dx;
            dx += l.curvature;
            if(l.Y >= maxy) continue;
            maxy = l.Y;
            std::cout << "l.curvature = " << l.curvature << std::endl;
            //(n/5) - constant affects the width of alternating stripes
            sf::Color grass  = (n / 5) % 2 ? sf::Color(16,200,16) : sf::Color(0,154,0);
            sf::Color rumble = (n / 5) % 2 ? sf::Color(255,255,255) : sf::Color(0,0,0);
            sf::Color track   = (n / 5) % 2 ? sf::Color(107,107,107) : sf::Color(105,105,105);

            Line p = lines[(n-1) % N]; //previous line
            //1.2 - clip width
            drawQuad(window, 0, p.Y, WINDOW_WIDTH, 0, l.Y, WINDOW_WIDTH, grass);
            drawQuad(window, p.X, p.Y, 1.2 * p.W, l.X, l.Y, 1.2 * l.W, rumble);
            std::cout << "n = " << n << " p.scale = " << p.scale << " p.X = " << p.X <<
                         " p.Y = " << p.Y << " p.W = " << p.W << " l.scale = " << l.scale << " l.X = " << l.X <<
                         " l.Y = " << l.Y << " l.W = " << l.W << std::endl;
            drawQuad(window, p.X, p.Y, p.W, l.X, l.Y, l.W, track);
        }
        /*sf::Text textCam;
        textCam.setColor(sf::Color::Yellow);
        textCam.setFont(font);
        textCam.setCharacterSize(40);
        textCam.setString("cameraY: " + std::to_string(cameraY));
        textCam.setPosition(100, 50);
        window.draw(textCam);*/
        window.display();
    }
    return 0;
}

void drawQuad(sf::RenderWindow &window, int nearMidPointX, int nearMidPointY,
              int nearWidth, int farMidPointX, int farMidPointY, int farWidth,
              sf::Color color)
{
    sf::ConvexShape shape(4);
    shape.setFillColor(color);
    shape.setPoint(0, sf::Vector2f(nearMidPointX - nearWidth, nearMidPointY));
    shape.setPoint(1, sf::Vector2f(farMidPointX - farWidth, farMidPointY));
    shape.setPoint(2, sf::Vector2f(farMidPointX + farWidth, farMidPointY));
    shape.setPoint(3, sf::Vector2f(nearMidPointX + nearWidth, nearMidPointY));
    window.draw(shape);
}
