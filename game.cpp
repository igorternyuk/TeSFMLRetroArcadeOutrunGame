#include "game.hpp"
#include <SFML/Audio.hpp>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

Game::Game():
    mWindow(sf::VideoMode(SCREEN_WIDTH,SCREEN_HEIGHT),WINDOW_TITLE),
    mTimePerFrame(sf::seconds(1.0f / FPS))
{
    centralizeWindow();
    mWindow.setFramerateLimit(FPS);
    loadFonts();
    loadTextures();
    configureTextInfo();
    startNewGame();
}

void Game::startNewGame()
{
    loadTrackConfiguration("track.txt");
}

void Game::createDefaultTrack()
{
    mTrack.push_back({0.f, 10.f});
    mTrack.push_back({0.f, 100.f});
    mTrack.push_back({0.3f, 70.f});
    mTrack.push_back({0.5f, 50.f});
    mTrack.push_back({0.6f, 40.f});
    mTrack.push_back({0.8f, 60.f});
    mTrack.push_back({0.6f, 150.f});
    mTrack.push_back({0.2f, 100.f});
    mTrack.push_back({0.0f, 130.f});
    mTrack.push_back({-0.1f, 50.f});
    mTrack.push_back({-0.2f, 50.f});
    mTrack.push_back({-0.5f, 50.f});
    mTrack.push_back({-0.8f, 50.f});
    mTrack.push_back({-0.4f, 50.f});
    mTrack.push_back({-0.3f, 50.f});
    mTrack.push_back({-0.15f, 50.f});
    mTrack.push_back({-0.8f, 50.f});
    mTrack.push_back({-0.5f, 50.f});
    mTrack.push_back({-0.8f, 40.f});
    mTrack.push_back({-0.5f, 50.f});
    mTrack.push_back({-0.8f, 70.f});
    mTrack.push_back({-0.5f, 60.f});
    mTrack.push_back({0.f, 10.f});
}

void Game::run()
{
    sf::Music music;
    music.openFromFile("resources/sounds/bg.ogg");
    music.setVolume(60.0f);
    music.play();
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    auto frameTime = mTimePerFrame.asSeconds();
    while(mWindow.isOpen())
    {
        timeSinceLastUpdate += clock.restart();
        while(timeSinceLastUpdate > mTimePerFrame){
            timeSinceLastUpdate -= mTimePerFrame;
            processEvents(frameTime);
            update(frameTime);
        }
        render();
    }
}

void Game::processEvents(float frameTime)
{
    sf::Event event;
    while(mWindow.pollEvent(event)){
        if(event.type == sf::Event::Closed)
        {
            mWindow.close();
        }
    }
    mCar.processEvents(frameTime, mCurrTrackCurvature);
}

void Game::update(float frameTime)
{
    mCar.update(frameTime);

    float offset = 0.f;
    mTrackSection = 0;
    //Find position on the track
    while(mTrackSection < mTrack.size() && offset < mCar.traveledDistance){
        offset += mTrack[mTrackSection].distance;
        ++mTrackSection;
    }

    if(mTrackSection >= mTrack.size()){
        mElapsedTimes.push_front(mCar.currElapsedTime);
        if(mElapsedTimes.size() > 5){
            mElapsedTimes.pop_back();
        }
        mTrackSection = 0;
        mCar.traveledDistance = 0.f;
        mCar.currElapsedTime = 0.0f;
    }

    mTargetCurvature = mTrack[mTrackSection].curvature;
    mCurrTrackCurvature += (mTargetCurvature - mCurrTrackCurvature) * frameTime * mCar.speed;
}

void Game::render()
{
    mWindow.clear(sf::Color(5,181,255));
    renderBackground();
    float middlePoint = 0.5f + mCurrTrackCurvature;
    float clipWidth = 0.025f;
    float centerLineWidth = 0.00125f;
    float endRoadWidth = 0.1f;
    endRoadWidth *= 0.5f;

    int prevleftGrass = (middlePoint - endRoadWidth - clipWidth) * SCREEN_WIDTH;
    int prevLeftClip = (middlePoint - endRoadWidth) * SCREEN_WIDTH;
    int prevLeftCenterLine = (middlePoint - 0.5f * centerLineWidth) * SCREEN_WIDTH;
    int prevRightCenterLine = (middlePoint + 0.5f * centerLineWidth) * SCREEN_WIDTH;
    int prevRightGrass = (middlePoint + endRoadWidth + clipWidth) * SCREEN_WIDTH;
    int prevRightClip = (middlePoint + endRoadWidth) * SCREEN_WIDTH;


    for(int y = 1; y < SCREEN_HEIGHT / 2; y += SEGMENT_HEIGHT)
    {
        float perspective = (float)y / (SCREEN_HEIGHT / 2.0f);
        middlePoint = 0.5f + mCurrTrackCurvature * pow(1.0f - perspective, 3);
        float roadWidth = 0.1f + 0.8f * perspective;
        roadWidth *= 0.5f;
        centerLineWidth = 0.0025f + 0.025f * perspective;
        centerLineWidth *= 0.5f;

        int leftGrass = (middlePoint - roadWidth - clipWidth) * SCREEN_WIDTH;
        int leftClip = (middlePoint - roadWidth) * SCREEN_WIDTH;
        int leftCenterLine = (middlePoint - centerLineWidth) * SCREEN_WIDTH;
        int rightCenterLine = (middlePoint + centerLineWidth) * SCREEN_WIDTH;
        int rightGrass = (middlePoint + roadWidth + clipWidth) * SCREEN_WIDTH;
        int rightClip = (middlePoint + roadWidth) * SCREEN_WIDTH;
        int row = SCREEN_HEIGHT / 2 + y + SEGMENT_HEIGHT;

        sf::Color grassColor = oscillatoryFunction(perspective, 30.f, 0.1f * mCar.traveledDistance) > 0.0f
                ? sf::Color::Green : sf::Color(0,180,0);
        sf::Color clipColor = oscillatoryFunction(perspective, 60.f, mCar.traveledDistance) > 0.0f
                        ? sf::Color::Red : sf::Color::White;
        sf::Color centerLineColor = oscillatoryFunction(perspective, 60.f, mCar.traveledDistance) > 0.0f
                ? sf::Color(180,180,180) : sf::Color::White;
        sf::Color trackColor = mTrackSection == 0 ? sf::Color(0,0,255,50) : sf::Color(180,180,180);

        //Left grass
        drawQuad(mWindow, sf::Vector2f(0, row),
                 sf::Vector2f(0, row - SEGMENT_HEIGHT),
                 sf::Vector2f(prevleftGrass, row - SEGMENT_HEIGHT),
                 sf::Vector2f(leftGrass, row),grassColor);

        //Left clip
        drawQuad(mWindow, sf::Vector2f(leftGrass, row),
                 sf::Vector2f(prevleftGrass, row - SEGMENT_HEIGHT),
                 sf::Vector2f(prevLeftClip, row - SEGMENT_HEIGHT),
                 sf::Vector2f(leftClip, row),clipColor);

        //Left track lane
        drawQuad(mWindow, sf::Vector2f(leftClip, row),
                 sf::Vector2f(prevLeftClip, row - SEGMENT_HEIGHT),
                 sf::Vector2f(prevLeftCenterLine, row - SEGMENT_HEIGHT),
                 sf::Vector2f(leftCenterLine, row), trackColor);

        //Center line
        drawQuad(mWindow, sf::Vector2f(leftCenterLine, row),
                 sf::Vector2f(prevLeftCenterLine, row - SEGMENT_HEIGHT),
                 sf::Vector2f(prevRightCenterLine, row - SEGMENT_HEIGHT),
                 sf::Vector2f(rightCenterLine, row),centerLineColor);

        //Right track lane
        drawQuad(mWindow, sf::Vector2f(rightCenterLine, row),
                 sf::Vector2f(prevRightCenterLine, row - SEGMENT_HEIGHT),
                 sf::Vector2f(prevRightClip, row - SEGMENT_HEIGHT),
                 sf::Vector2f(rightClip, row), trackColor);

        //Right clip
        drawQuad(mWindow, sf::Vector2f(rightClip, row),
                 sf::Vector2f(prevRightClip, row - SEGMENT_HEIGHT),
                 sf::Vector2f(prevRightGrass, row - SEGMENT_HEIGHT),
                 sf::Vector2f(rightGrass, row),clipColor);

        //Right grass
        drawQuad(mWindow, sf::Vector2f(rightGrass, row),
                 sf::Vector2f(prevRightGrass, row - SEGMENT_HEIGHT),
                 sf::Vector2f(SCREEN_WIDTH, row - SEGMENT_HEIGHT),
                 sf::Vector2f(SCREEN_WIDTH, row),grassColor);

        prevleftGrass = leftGrass;
        prevLeftClip = leftClip;
        prevLeftCenterLine = leftCenterLine;
        prevRightCenterLine = rightCenterLine;
        prevRightGrass = rightGrass;
        prevRightClip = rightClip;
    }

    mCar.render(mWindow);
    renderTextInfo();
    mWindow.display();
}

double Game::hillHeightFunction(double x)
{
    static const float maxHillHeight = SCREEN_HEIGHT / 5.0f;
    return fabs(maxHillHeight * sin(x * 0.005f + mCurrTrackCurvature));
}

void Game::renderBackground()
{
    float prevX = 0.0f;
    float prevHillHeight = hillHeightFunction(prevX);
    float screenMiddleY = SCREEN_HEIGHT / 2 + 1;
    for(int x = HILL_SEGMENT_WIDTH; x <= SCREEN_WIDTH; x += HILL_SEGMENT_WIDTH){
        auto hillHeight = hillHeightFunction(x);
        drawQuad(mWindow, sf::Vector2f(prevX, screenMiddleY),
                 sf::Vector2f(prevX, screenMiddleY - prevHillHeight),
                 sf::Vector2f(x, screenMiddleY - hillHeight),
                 sf::Vector2f(x, screenMiddleY),
                 sf::Color(255,174,0));
        prevX = x;
        prevHillHeight = hillHeight;
    }
}

void Game::renderCar()
{
    float carHeight = mCarSprite.getGlobalBounds().height;

    mCar.position = mCar.trajectoryCurvature - mCurrTrackCurvature;

    int carX = SCREEN_WIDTH / 2 + int(SCREEN_WIDTH * mCar.position / 2.0f);

    mCarSprite.setPosition(carX, SCREEN_HEIGHT - carHeight -5);
    mWindow.draw(mCarSprite);
}

void Game::renderTextInfo()
{
    std::string info;
    info.append("Total track distance: ");
    info.append(std::to_string(mTotalDistance));
    info.append("\n");
    info.append("Traveled distance: ");
    float percentage = mCar.traveledDistance / mTotalDistance * 100.0f;
    info.append(std::to_string(mCar.traveledDistance) + "(" +
                std::to_string(percentage) + "%)");
    info.append("\n");
    info.append("Target curvature: ");
    info.append(std::to_string(mTargetCurvature));
    info.append("\n");
    info.append("Player curvature: ");
    info.append(std::to_string(mCar.trajectoryCurvature));
    info.append("\n");
    info.append("Player speed: ");
    int speed = mCar.speed > 1.0f ? 100 : int(mCar.speed * 100);
    info.append(std::to_string(speed) + "%");
    info.append("\n");
    info.append("Track curvature: ");
    info.append(std::to_string(mCurrTrackCurvature));
    info.append("\n");
    info.append("Delta_curvature: ");
    info.append(std::to_string(mCar.trajectoryCurvature - mCurrTrackCurvature));
    info.append("\n");
    info.append("Track section: ");
    info.append(std::to_string(mTrackSection));
    info.append("\n");

    auto getFormattedTime = [](float elapsedTime){
        int min = elapsedTime / 60.0f;
        int sec = elapsedTime - min * 60;
        int ms = (elapsedTime - (float)sec) * 1000;
        return std::to_string(min) + " min: " + std::to_string(sec) +
                " sec: " + std::to_string(ms) +" ms";
    };

    int n = 0;
    for(auto t: mElapsedTimes){
        info.append(std::to_string(++n) + ". ");
        info.append(getFormattedTime(t));
        info.append("\n");
    }
    //mTrackSection
    mGameInfo.setString(info);
    mWindow.draw(mGameInfo);
}


double Game::oscillatoryFunction(double x, double frequency, double phase,
                                 double exponent)
{
    return sin(frequency * pow(1.0f - x, exponent) + phase);
}


void Game::centralizeWindow()
{
    const int screenWidth = sf::VideoMode::getDesktopMode().width;
    const int screenHeight = sf::VideoMode::getDesktopMode().height;
    mWindow.setPosition(sf::Vector2i((screenWidth - SCREEN_WIDTH) / 2,
                                     (screenHeight - SCREEN_HEIGHT) / 2));
}

void Game::loadTrackConfiguration(const std::__cxx11::string pathToFile)
{
    mTrack.clear();
    std::ifstream fi(pathToFile);
    if(fi.is_open()){
        std::string line;
        while(!fi.eof())
        {
            std::getline(fi, line);
            std::stringstream ss(line);
            float currCurvature, currDistance;
            ss >> currCurvature >> currDistance;
            mTrack.push_back({currCurvature, currDistance});
        }
    } else {
        createDefaultTrack();
    }

    mTotalDistance = 0.0f;
    for(auto segment: mTrack)
    {
        mTotalDistance += segment.distance;
    }
}

void Game::loadFonts()
{
    static const std::string pathToFont{ "resources/fonts/BRLNSR.TTF" };
    mFont.loadFromFile(pathToFont);
}

void Game::loadTextures()
{
    static const std::string pathToCarImage{ "resources/images/cars.png" };
    mCarTexture.loadFromFile(pathToCarImage);
    mCarTexture.setSmooth(true);
    mCarTexture.setRepeated(false);
    mCarSprite.setTexture(mCarTexture);
    mCarSprite.setTextureRect(sf::IntRect(264,144,31,23));
    float w = mCarSprite.getGlobalBounds().width;
    float h = mCarSprite.getGlobalBounds().height;
    mCarSprite.setOrigin( w / 2, h / 2);
    mCarSprite.scale(4.0f,4.0f);
    mCar.sprite = mCarSprite;
}

void Game::configureTextInfo()
{
    mGameInfo.setColor(sf::Color(180,0,0));
    mGameInfo.setFont(mFont);
    mGameInfo.setCharacterSize(20);
    mGameInfo.setPosition(5, 12);
}

void Game::drawQuad(sf::RenderWindow &window, const sf::Vector2f &bottomLeft,
                    const sf::Vector2f &topLeft, const sf::Vector2f &topRight,
                    const sf::Vector2f &bottomRight, sf::Color color)
{
    sf::ConvexShape shape(4);
    shape.setFillColor(color);
    shape.setPoint(0, bottomLeft);
    shape.setPoint(1, topLeft);
    shape.setPoint(2, topRight);
    shape.setPoint(3,bottomRight);
    window.draw(shape);
}

void Game::Car::processEvents(float frameTime, float currTrackCurvature)
{
    //Steering
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
       trajectoryCurvature -= 0.6f * frameTime;
        sprite.setTextureRect(sf::IntRect(216,144,38,23));
        //mCarSprite.setRotation(-20.f);

    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
              sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        trajectoryCurvature += 0.6f * frameTime;
        sprite.setTextureRect(sf::IntRect(304,144,38,23));
    }
    else
    {
        sprite.setTextureRect(sf::IntRect(264,144,32,23));
    }

    float deltaCurvature = fabs(trajectoryCurvature - currTrackCurvature);
    if(deltaCurvature < 0.7f)
    {
        speed -= deltaCurvature * 4.0f * frameTime;
    }
    else
    {
        speed -= 5.0f * frameTime;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        //Acceleration
        speed += 1.2f * frameTime;
    } else {
        //Deceleration
        speed -= 0.4f * frameTime;
    }

    if(speed > 1.0f) speed = 1.0f;
    if(speed < 0.0f) speed = 0.0f;

    position = trajectoryCurvature - currTrackCurvature;
}

void Game::Car::update(float frameTime)
{
    traveledDistance += 90.0f * speed * frameTime;
    currElapsedTime += frameTime;
}

void Game::Car::render(sf::RenderWindow &window)
{
    static const float height = sprite.getGlobalBounds().height;
    int carX = SCREEN_WIDTH / 2 + int(SCREEN_WIDTH * position / 2.0f);
    sprite.setPosition(carX, SCREEN_HEIGHT - height - 5);
    window.draw(sprite);
}
