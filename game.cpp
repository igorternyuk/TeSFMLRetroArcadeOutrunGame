#include "game.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

Game::Game():
    mWindow(sf::VideoMode(WINDOW_WIDTH,WINDOW_HEIGHT),WINDOW_TITLE),
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

void Game::render()
{
    mWindow.clear(sf::Color::Black);
    mWindow.draw(mSpriteBackground);

    //Track rendering

    const int N = mLines.size();
    int maxY = WINDOW_HEIGHT;

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
        drawQuad(mWindow, 0, prevLine.Y, WINDOW_WIDTH, 0, currLine.Y,
                 WINDOW_WIDTH, grass);
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
    mWindow.display();
}

void Game::centralizeWindow()
{
    const int screenWidth = sf::VideoMode::getDesktopMode().width;
    const int screenHeight = sf::VideoMode::getDesktopMode().height;
    mWindow.setPosition(sf::Vector2i((screenWidth - WINDOW_WIDTH) / 2,
                                    (screenHeight - WINDOW_HEIGHT) / 2));
}

void Game::loadFonts()
{
    static const std::string pathToFont{ "BRLNSR.TTF" };
    mFont.loadFromFile(pathToFont);
}

void Game::loadTextures()
{
    static const std::string pathToBackgroundImage{ "images/bg.png" };
    mBackgroundTexture.loadFromFile(pathToBackgroundImage);
    mBackgroundTexture.setRepeated(true);
    mSpriteBackground.setTexture(mBackgroundTexture);
    mSpriteBackground.setTextureRect(sf::IntRect(0,0,5000,411));
    mSpriteBackground.setPosition(-2000,0);
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




