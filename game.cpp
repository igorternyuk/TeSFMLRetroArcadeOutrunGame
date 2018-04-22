#include "game.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

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
    createTrack();
}

void Game::createTrack()
{
    mTrack.push_back({0.f, 10.f});
    mTrack.push_back({0.f, 00.f});
    mTrack.push_back({0.3f, 50.f});
    mTrack.push_back({0.5f, 50.f});
    mTrack.push_back({0.6f, 50.f});
    mTrack.push_back({0.8f, 50.f});
    mTrack.push_back({0.6f, 50.f});
    mTrack.push_back({0.2f, 50.f});
    mTrack.push_back({0.0f, 50.f});
    mTrack.push_back({-0.1f, 50.f});
    mTrack.push_back({-0.2f, 50.f});
    mTrack.push_back({-0.5f, 50.f});
    mTrack.push_back({-0.7f, 50.f});
    mTrack.push_back({-0.5f, 50.f});
    mTrack.push_back({-0.3f, 50.f});
    mTrack.push_back({-0.1f, 50.f});
}

void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    auto delta = mTimePerFrame.asSeconds();
    while(mWindow.isOpen())
    {
        timeSinceLastUpdate += clock.restart();
        //std::cout << "timeSinceLastUpdate = " << timeSinceLastUpdate.asSeconds() << std::endl;
        //std::cout << "mTimePerFrame = " << mTimePerFrame.asSeconds() << std::endl;
        while(timeSinceLastUpdate > mTimePerFrame){
            timeSinceLastUpdate -= mTimePerFrame;
            processEvents(delta);
            update(delta);
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
        else if(event.type == sf::Event::MouseButtonReleased)
        {
            if(event.mouseButton.button == sf::Mouse::Left)
            {
                std::cout << " mx = " << event.mouseButton.x << " my = "
                          << event.mouseButton.y << std::endl;
            }
        }
    }

    //Steering
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        mPlayerCurvature -= 0.6f * frameTime;
        mCarSprite.setTextureRect(sf::IntRect(216,144,38,23));
        //mCarSprite.setRotation(-20.f);

    } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
              sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        mPlayerCurvature += 0.6f * frameTime;
        //mCarSprite.setRotation(20.f);
        mCarSprite.setTextureRect(sf::IntRect(304,144,38,23));
    } else {
        //mCarSprite.setRotation(0.0f);
        mCarSprite.setTextureRect(sf::IntRect(264,144,32,23));
    }

    if(fabs(mPlayerCurvature - mCurrTrackCurvature) >= 0.65f)
    {
        mCarSpeed -= 5.0f * frameTime;
        if(mCarSpeed < 0.0f) mCarSpeed = 0.0f;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        //Acceleration
        mCarSpeed += 1.2f * frameTime;
        if(mCarSpeed > 1.0f) mCarSpeed = 1.0f;
    } else {
        //Deceleration
        mCarSpeed -= 0.4f * frameTime;
        if(mCarSpeed < 0.0f) mCarSpeed = 0.0f;
    }
}

void Game::update(float frameTime)
{
    //auto frameTime = frameTime.asSeconds();
    mDistance += 90.0f * mCarSpeed * frameTime;
    mCurrentElapsedTime += frameTime;
    float offset = 0.f;
    mTrackSection = 0;
    //Find position on the track
    while(mTrackSection < mTrack.size() && offset < mDistance){
        offset += mTrack[mTrackSection].distance;
        ++mTrackSection;
    }

    if(mTrackSection >= mTrack.size()){
        mElapsedTimes.push_front(mCurrentElapsedTime);
        if(mElapsedTimes.size() > 5){
            mElapsedTimes.pop_back();
        }
        mTrackSection = 0;
        mDistance = 0.f;
        mCurrentElapsedTime = 0.0f;
    }

    mTargetCurvature = mTrack[mTrackSection].curvature;
    mCurrTrackCurvature += (mTargetCurvature - mCurrTrackCurvature) * frameTime * mCarSpeed;
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

        sf::Color grassColor = oscillatoryFunction(perspective, 30.f, 0.1f * mDistance) > 0.0f
                ? sf::Color::Green : sf::Color(0,180,0);
        sf::Color clipColor = oscillatoryFunction(perspective, 60.f, mDistance) > 0.0f
                        ? sf::Color::Red : sf::Color::White;
        sf::Color centerLineColor = oscillatoryFunction(perspective, 60.f, mDistance) > 0.0f
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

        float carWidth = mCarSprite.getGlobalBounds().width;
        float carHeight = mCarSprite.getGlobalBounds().height;

        mCarPos = mPlayerCurvature - mCurrTrackCurvature;

        int carX = SCREEN_WIDTH / 2 + int(SCREEN_WIDTH * mCarPos / 2.0f)/* - carWidth / 2*/;

        mCarSprite.setPosition(carX, SCREEN_HEIGHT - carHeight -5);
        mWindow.draw(mCarSprite);
    }
    renderTextInfo();
    mWindow.display();
}

void Game::renderBackground()
{
    float prevX = 0.0f;
    float prevHillHeight = 0.0f;
    const float maxHillHeight = SCREEN_HEIGHT / 5.0f;
    for(int x = HILL_SEGMENT_WIDTH; x <= SCREEN_WIDTH; x += HILL_SEGMENT_WIDTH){
        auto hillHeight = fabs(maxHillHeight * sin(x * 0.005f + mCurrTrackCurvature));
        drawQuad(mWindow, sf::Vector2f(prevX, SCREEN_HEIGHT / 2),
                 sf::Vector2f(prevX, SCREEN_HEIGHT / 2 - prevHillHeight),
                 sf::Vector2f(x, SCREEN_HEIGHT / 2 - hillHeight),
                 sf::Vector2f(x, SCREEN_HEIGHT / 2),
                 sf::Color(124,169,11));
        prevX = x;
        prevHillHeight = hillHeight;
    }
}

void Game::renderTextInfo()
{
    std::string info;
    info.append("Distance: ");
    info.append(std::to_string(mDistance));
    info.append("\n");
    info.append("Target curvature: ");
    info.append(std::to_string(mTargetCurvature));
    info.append("\n");
    info.append("Player curvature: ");
    info.append(std::to_string(mPlayerCurvature));
    info.append("\n");
    info.append("Player speed: ");
    info.append(std::to_string(mCarSpeed));
    info.append("\n");
    info.append("Track curvature: ");
    info.append(std::to_string(mCurrTrackCurvature));
    info.append("\n");
    info.append("Delta_curvature: ");
    info.append(std::to_string(mPlayerCurvature - mCurrTrackCurvature));
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
