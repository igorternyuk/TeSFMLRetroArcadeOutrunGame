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
    startNewGame();
}

void Game::startNewGame()
{
    createLines();
}

void Game::createLines()
{
    mLines.reserve(NUM_LINES);

    for(auto i = 0; i < NUM_LINES; ++i)
    {
        Line line;
        line.z = i * SEGMENT_LENGTH;
        if(i >= 300 && i < 700)
        {
            line.curvature = 0.5f;
        } else if(i > 1100){
            line.curvature = -0.7;
        }
        if (i>750)
        {
            line.y = sin(i/30.0)*1500;
        }
        mLines.push_back(line);
    }
}


void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while(mWindow.isOpen())
    {
        timeSinceLastUpdate += clock.restart();
        //std::cout << "timeSinceLastUpdate = " << timeSinceLastUpdate.asSeconds() << std::endl;
        //std::cout << "mTimePerFrame = " << mTimePerFrame.asSeconds() << std::endl;
        while(timeSinceLastUpdate > mTimePerFrame){
            //std::cout << "То шо я думаю" << std::endl;
            timeSinceLastUpdate -= mTimePerFrame;
            //std::cout << "timeSinceLastUpdate = " << timeSinceLastUpdate.asSeconds() << std::endl;
            processEvents();
            update(mTimePerFrame);
        }
        render();
    }
}

void Game::processEvents()
{
    sf::Event event;
    while(mWindow.pollEvent(event)){
        if(event.type == sf::Event::Closed)
        {
            mWindow.close();
        }
        /*else if(event.type == sf::Event::MouseWheelScrolled)
        {
           float delta = event.mouseWheelScroll.delta;
           if(delta > 0)
           {
               //cameraY += 10;
           } else if( delta < 0){
               //cameraY -= 10;
           }
        }*/
        else if(event.type == sf::Event::MouseButtonReleased)
        {
            if(event.mouseButton.button == sf::Mouse::Left)
            {
                std::cout << " mx = " << event.mouseButton.x << " my = "
                          << event.mouseButton.y << std::endl;
            }
        }
    }

    mPlayer.speed = 0;
    mPlayer.posX = 0;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        mPlayer.posX -= 0.1;
        std::cout << "Moving left" << std::endl;
    } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        mPlayer.posX += 0.1;
        std::cout << "Moving right" << std::endl;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        mPlayer.speed = 200;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        mPlayer.speed = -200;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) mCameraHeight += 100;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) mCameraHeight -= 100;
}

void Game::update(sf::Time delta)
{
    auto frameTime = delta.asMicroseconds();
    //updateFamtrinli();
}

void Game::render()
{
    mWindow.clear(sf::Color::Black);
    //renderFamtrinli();
    renderRetro();
    mWindow.display();
}

void Game::updateRetro()
{

}

void Game::renderRetro()
{
    float middlePoint = 0.5f;
    float clipWidth = 0.03f;
    float endRoadWidth = 0.03f;
    endRoadWidth *= 0.5f;

    int prevleftGrass = (middlePoint - endRoadWidth - clipWidth) * SCREEN_WIDTH;
    int prevLeftClip = (middlePoint - endRoadWidth) * SCREEN_WIDTH;
    int prevRightGrass = (middlePoint + endRoadWidth + clipWidth) * SCREEN_WIDTH;
    int prevRightClip = (middlePoint + endRoadWidth) * SCREEN_WIDTH;


    for(int y = 1; y < SCREEN_HEIGHT / 2; y += SEGMENT_HEIGHT)
    {
        float perspective = (float)y / (SCREEN_HEIGHT / 2.0f);
        float roadWidth = 0.1f + 0.8f * perspective;
        roadWidth *= 0.5f;

        int leftGrass = (middlePoint - roadWidth - clipWidth) * SCREEN_WIDTH;
        int leftClip = (middlePoint - roadWidth) * SCREEN_WIDTH;
        int rightGrass = (middlePoint + roadWidth + clipWidth) * SCREEN_WIDTH;
        int rightClip = (middlePoint + roadWidth) * SCREEN_WIDTH;
        int row = SCREEN_HEIGHT / 2 + y + SEGMENT_HEIGHT;
        /*drawRect(mWindow, sf::Color::Green, 0, row, leftGrass, SEGMENT_HEIGHT);
        drawRect(mWindow, sf::Color::Red, leftGrass, row, leftClip - leftGrass, SEGMENT_HEIGHT);
        drawRect(mWindow, sf::Color(200,200,200), leftClip, row, rightClip - leftClip, SEGMENT_HEIGHT);
        drawRect(mWindow, sf::Color::Red, rightClip, row, rightGrass - rightClip, SEGMENT_HEIGHT);
        drawRect(mWindow, sf::Color::Green, rightGrass, row, SCREEN_WIDTH - rightGrass, SEGMENT_HEIGHT);*/

        /*for(int x = 0; x < SCREEN_WIDTH; ++x){
        }*/
        //Left grass
        drawQuad(mWindow, sf::Vector2f(0, row),
                 sf::Vector2f(0, row - SEGMENT_HEIGHT),
                 sf::Vector2f(prevleftGrass, row - SEGMENT_HEIGHT),
                 sf::Vector2f(leftGrass, row),sf::Color::Green);

        //Left clip
        drawQuad(mWindow, sf::Vector2f(leftGrass, row),
                 sf::Vector2f(prevleftGrass, row - SEGMENT_HEIGHT),
                 sf::Vector2f(prevLeftClip, row - SEGMENT_HEIGHT),
                 sf::Vector2f(leftClip, row),sf::Color::Red);

        //Road
        drawQuad(mWindow, sf::Vector2f(leftClip, row),
                 sf::Vector2f(prevLeftClip, row - SEGMENT_HEIGHT),
                 sf::Vector2f(prevRightClip, row - SEGMENT_HEIGHT),
                 sf::Vector2f(rightClip, row),sf::Color(200,200,200));

        //Right clip
        drawQuad(mWindow, sf::Vector2f(rightClip, row),
                 sf::Vector2f(prevRightClip, row - SEGMENT_HEIGHT),
                 sf::Vector2f(prevRightGrass, row - SEGMENT_HEIGHT),
                 sf::Vector2f(rightGrass, row),sf::Color::Red);

        //Right grass
        drawQuad(mWindow, sf::Vector2f(rightGrass, row),
                 sf::Vector2f(prevRightGrass, row - SEGMENT_HEIGHT),
                 sf::Vector2f(SCREEN_WIDTH, row - SEGMENT_HEIGHT),
                 sf::Vector2f(SCREEN_WIDTH, row),sf::Color::Green);

        prevleftGrass = leftGrass;
        prevLeftClip = leftClip;
        prevRightGrass = rightGrass;
        prevRightClip = rightClip;

        int carPos = SCREEN_WIDTH / 2 + int(SCREEN_WIDTH * mCarPos / 2.0f) -
                mCarSprite.getGlobalBounds().width / 2;
        mCarSprite.setPosition(carPos, SCREEN_HEIGHT - mCarSprite.getGlobalBounds().height - 5);
        mWindow.draw(mCarSprite);
    }
}

void Game::updateFamtrinli()
{
    mCamera.reset();
    const int N = mLines.size();
    mCamera.z += mPlayer.speed;
    while(mCamera.z >= N * SEGMENT_LENGTH) mCamera.z -= N * SEGMENT_LENGTH;
    while(mCamera.z < 0) mCamera.z += N * SEGMENT_LENGTH;
    mStartPos = mCamera.z / SEGMENT_LENGTH;
    mCamera.y = mLines[mStartPos].y + mCameraHeight;

    if (mPlayer.speed > 0)
    {
        mSpriteBackground.move(-mLines[mStartPos].curvature*2,0);
    }
    else if (mPlayer.speed < 0)
    {
        mSpriteBackground.move( mLines[mStartPos].curvature*2,0);
    }
}

void Game::renderFamtrinli()
{
    mWindow.draw(mSpriteBackground);

    //Track rendering

    const int N = mLines.size();
    int maxY = SCREEN_HEIGHT;

    for(int n = mStartPos; n < mStartPos + 300; ++n)
    {
        //std::cout << "n = " << n << " n%N = " << (n%N) << std::endl;
        Line &currLine = mLines[n%N]; // Mod operation keeps array index in bounds
        currLine.project(mPlayer.posX * TRACK_WIDTH - mCamera.x, mCamera.y,
                         mStartPos * SEGMENT_LENGTH - (n >= N ? N * SEGMENT_LENGTH : 0));
        mCamera.x += mCamera.dx;
        mCamera.dx += currLine.curvature;

        if(currLine.Y >= maxY) continue;
        maxY = currLine.Y;

       // std::cout << "l.curvature = " << currLine.curvature << std::endl;

        //(n/5) - constant affects the width of alternating stripes
        sf::Color grass  = (n / 5) % 2 ? sf::Color(16,200,16) : sf::Color(0,154,0);
        sf::Color clip = (n / 5) % 2 ? sf::Color(255,255,255) : sf::Color(0,0,0);
        sf::Color track   = (n / 5) % 2 ? sf::Color(107,107,107) : sf::Color(105,105,105);

        Line prevLine = mLines[(n-1) % N]; //previous line
        //1.2 - clip width
        drawQuad(mWindow, 0, prevLine.Y, SCREEN_WIDTH, 0, currLine.Y,
                 SCREEN_WIDTH, grass);
        drawQuad(mWindow, prevLine.X, prevLine.Y, 1.2 * prevLine.W,
                 currLine.X, currLine.Y, 1.2 * currLine.W, clip);
        drawQuad(mWindow, prevLine.X, prevLine.Y, prevLine.W, currLine.X,
                 currLine.Y, currLine.W, track);

       /* std::cout << "n = " << n << " p.scale = " << prevLine.scale <<
                     " p.X = " << prevLine.X << " p.Y = " << prevLine.Y <<
                     " p.W = " << prevLine.W << " l.scale = " <<
                     currLine.scale << " l.X = " << currLine.X <<
                     " l.Y = " << currLine.Y << " l.W = " <<
                     currLine.W << std::endl;*/
    }
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
    static const std::string pathToBackgroundImage{ "resources/images/bg.png" };
    static const std::string pathToCarImage{ "resources/images/car.png" };
    mBackgroundTexture.loadFromFile(pathToBackgroundImage);
    mBackgroundTexture.setRepeated(true);
    mSpriteBackground.setTexture(mBackgroundTexture);
    mSpriteBackground.setTextureRect(sf::IntRect(0,0,5000,411));
    mSpriteBackground.setPosition(-2000,0);
    mCarTexture.loadFromFile(pathToCarImage);
    mCarTexture.setSmooth(true);
    mCarTexture.setRepeated(false);
    mCarSprite.setTexture(mCarTexture);
}

void Game::drawQuad(sf::RenderWindow &window, int nearMidPointX, int nearMidPointY,
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

void Game::drawRect(sf::RenderWindow &window, sf::Color color, float x, float y,
                    float width, float height)
{
    sf::RectangleShape rect(sf::Vector2f(width, height));
    rect.setPosition(x,y);
    //rect.setSize(sf::Vector2f(width, height));
    rect.setFillColor(color);
    window.draw(rect);
}




