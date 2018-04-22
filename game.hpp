#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <list>

class Game
{
public:
    explicit Game();
    void run();
private:
    const std::string WINDOW_TITLE { "Outrun" };
    enum {
        SCREEN_WIDTH = 1024,
        SCREEN_HEIGHT = 704,
        SEGMENT_HEIGHT = 2,
        HILL_SEGMENT_WIDTH = 8,
        TRACK_WIDTH = 2000,
        SEGMENT_LENGTH = 200,
        NUM_LINES = 1600,
        FPS = 60
    };

    sf::RenderWindow mWindow;
    sf::Time mTimePerFrame;
    sf::Font mFont;
    sf::Texture mCarTexture;
    sf::Sprite mCarSprite;
    sf::Text mGameInfo;

    /////////////////////////
    float mCarPos = 0.0f;
    float mCarSpeed = 0.0f;
    float mDistance = 0.0f;
    int mTrackSection = 0;
    struct Segment
    {
        float curvature, distance;
    };
    std::vector<Segment> mTrack;
    float mTargetCurvature = 0.0f;
    float mCurrTrackCurvature = 0.0f;
    float mPlayerCurvature = 0.0f;
    float mCurrentElapsedTime = 0.0f;
    std::list<float> mElapsedTimes;
    ///////////////////////
    void startNewGame();
    void createTrack();
    void processEvents(float frameTime);
    void update(float frameTime);
    void render();
    void renderBackground();
    void renderTextInfo();
    void centralizeWindow();
    void loadFonts();
    void loadTextures();
    void configureTextInfo();
    double oscillatoryFunction(double x, double frequency,
                               double phase, double exponent = 3);
    void drawQuad(sf::RenderWindow &window, const sf::Vector2f &bottomLeft,
                  const sf::Vector2f &topLeft, const sf::Vector2f &topRight,
                  const sf::Vector2f &bottomRight, sf::Color color = sf::Color(0,255,0));
};
