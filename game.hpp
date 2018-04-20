#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Game
{
public:
    explicit Game();
    void run();
private:
    const std::string WINDOW_TITLE { "Outrun" };
    enum {
        WINDOW_WIDTH = 1024,
        WINDOW_HEIGHT = 768,
        TRACK_WIDTH = 2000,
        SEGMENT_LENGTH = 200,
        NUM_LINES = 1600,
        FPS = 60
    };

    sf::RenderWindow mWindow;
    sf::Time mTimePerFrame;
    sf::Font mFont;
    sf::Texture mBackgroundTexture;
    sf::Sprite mSpriteBackground;

    struct Player
    {
        int posX;
        int speed;
        float curvature;
        Player()
        {
            posX = 0;
            speed = 0;
            curvature = 0.0f;
        }
    };

    //const float mCameraDepth = 0.84f;
    int mCameraHeight = 1500;

    struct Camera
    {
        float x, y, z, dx;
        Camera(){ x = y = z = dx = 0; }
        void reset() { x = y = dx = 0; }
    };

    Camera mCamera;

    struct Line
    {
        float x, y, z; //line center
        float X, Y, W; //screen coord
        float curvature, scale;

        Line() { curvature = x = y = z = 0; }

        void project(int camX, int camY, int camZ)
        {
            scale = 0.84f / (z - camZ);
            X = (1 + scale * (x - camX)) * WINDOW_WIDTH / 2;
            Y = (1 - scale * (y - camY)) * WINDOW_HEIGHT / 2;
            W = scale * TRACK_WIDTH * WINDOW_WIDTH / 2;
        }
    };

    Player mPlayer;
    std::vector<Line> mLines;
    int mStartPos;
    void startNewGame();
    void createLines();
    void processEvents();
    void update(sf::Time delta);
    void render();

    void centralizeWindow();
    void loadFonts();
    void loadTextures();

    void drawQuad(sf::RenderWindow &window, int nearMidPointX, int nearMidPointY,
                  int nearWidth, int farMidPointX, int farMidPointY, int farWidth,
                  sf::Color color = {0,255,0});
};
