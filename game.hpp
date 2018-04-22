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

    struct Car
    {
        float position = 0.0f;
        float speed = 0.0f;
        float traveledDistance = 0.0f;
        float trajectoryCurvature = 0.0f;
        float currElapsedTime = 0.0f;
        sf::Sprite sprite;
        void processEvents(float frameTime, float currTrackCurvature);
        void update(float frameTime);
        void render(sf::RenderWindow &window);
    };

    Car mCar;

    struct Segment
    {
        float curvature, distance;
    };

    std::vector<Segment> mTrack;
    size_t mTrackSection = 0;
    float mTargetCurvature = 0.0f;
    float mCurrTrackCurvature = 0.0f;
    float mTrackDistance = 0.0f;
    float mTotalDistance;
    std::list<float> mElapsedTimes;
    void startNewGame();
    void createDefaultTrack();
    void processEvents(float frameTime);
    void update(float frameTime);
    void render();
    void renderBackground();
    void renderCar();
    void renderTextInfo();
    void centralizeWindow();
    void loadTrackConfiguration(const std::string pathToFile);
    void loadFonts();
    void loadTextures();
    void configureTextInfo();
    double hillHeightFunction(double x);
    double oscillatoryFunction(double x, double frequency,
                               double phase, double exponent = 3);
    void drawQuad(sf::RenderWindow &window, const sf::Vector2f &bottomLeft,
                  const sf::Vector2f &topLeft, const sf::Vector2f &topRight,
                  const sf::Vector2f &bottomRight, sf::Color color = sf::Color(0,255,0));
};
