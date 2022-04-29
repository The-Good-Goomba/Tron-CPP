
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Audio.hpp>
//#include <SFML/Audio/Sound.hpp>
//#include <SFML/Audio/Music.hpp>
#include <SFML/System/Time.hpp>

// Here is a small helper for you! Have a look.
#include "ResourcePath.hpp"
#include <iostream>
#include <deque>
#include <algorithm>
#include <stdlib.h>
#include <string>
#include <cstdlib>
// Declaring all of the variables for the directions
int up[2] = {0,-1};
int down[2] = {0,1};
int left[2] = {-1,0};
int right[2] = {1,0};

// Decalaring variables for the screen
int screenHeight = 1080;
int screenWidth = 1920;
int gridSize = 12;
int gridWidth = screenWidth/gridSize;
int gridHeight = screenHeight/gridSize;
bool drawWinScreen = 0;
int timey = 0; // is a timer that is increased every logic process

std::string winner = "TIE"; // Who wins

// Declaring SFML variables
sf::RenderTexture surface;
sf::Font font;
sf::Text lc1Score;
sf::Text lc2Score;
sf::Clock glock;
// Create the main window
sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Tron");

//Music related variables
std::string musics[2] = {"derezzed.wav","rinzler.ogg"};
std::string curMusic = musics[rand() % 2];
int musLength;
sf::Clock musTimer;

 //huge class containing both lightcyles and their variables
class LightCycle
{
    public:
    std::deque<int> posx;
    std::deque<int> posy;
    sf::Color colour;
    int score = 0;
    int direction[2];
    int newPos[2];
    bool turned = false;
    sf::RectangleShape rectangle = sf::RectangleShape(sf::Vector2f(gridSize, gridSize));
    int cur[2];
    int boostAmount = 80;
    int speeds = 0;
    bool speedActive = false;
    int speedLeft = boostAmount;
    int invincibles = 0;
    bool invinceActive = false;
    int invinceLeft = boostAmount;
    sf::Texture discs;
    sf::Sprite disc;
    
    LightCycle(bool type)
    {
        posx.clear();
        posy.clear();
        
        if (type){
            colour = sf::Color(3, 244, 252,255);
            posx = {(3*gridWidth/4)*gridSize};
            posy = {(3*gridHeight/4)*gridSize};
            direction[0] = left[0];
            direction[1] = left[1];
            
            if (!discs.loadFromFile(resourcePath()+ "resources/spritesheet.png",sf::IntRect(0,0,28,28)))
            {
                return EXIT_FAILURE;
            }
            
        }else {
            colour = sf::Color(252, 93, 13,255);
            posx = {(gridWidth/4)*gridSize};
            posy = {(gridHeight/4)*gridSize};
            direction[0] = right[0];
            direction[1] = right[1];
            
            if (!discs.loadFromFile(resourcePath()+ "resources/spritesheet.png",sf::IntRect(28,0,28,28)))
            {
                return EXIT_FAILURE;
            }
        }
        
        newPos[0] = posx.front();
        newPos[1] = posy.front();
        cur[0] = newPos[0];
        cur[1] = newPos[1];
        
        discs.setSmooth(true);
        disc.setTexture(discs);
        disc.setScale((float)gridSize/28, (float)gridSize/28);
    }
    

    void turn(int* point)
    {
        if (this->turned == false)
        {
            
            if (point[0]*-1 != direction[0] || point[1]*-1 != direction[1])
            {
                this->direction[0] = point[0];
                this->direction[1] = point[1];
                this->turned = true;
            }
        }
    }
    
    void move()
    {
        if (this->newPos[0]+(direction[0]*gridSize) < 0){
            newPos[0] = gridWidth*gridSize;
        } else if (this->newPos[0]+(this->direction[0]*gridSize) >= screenWidth){
            this->newPos[0] = -gridSize;
        }
        
        if (this->newPos[1]+(direction[1]*gridSize) < 0){
            this->newPos[1] = gridHeight*gridSize;
        } else if (this->newPos[1]+(this->direction[1]*gridSize) >= screenHeight){
            this->newPos[1] = -gridSize;
        }
        
        
        this->posx.push_front(this->newPos[0]+(this->direction[0]*gridSize));
        this->posy.push_front(this->newPos[1]+(this->direction[1]*gridSize));
        this->newPos[0] = this->posx.front();
        this->newPos[1] = this->posy.front();
        this->turned = false;
    }
    
    void draw()
    {
        this->rectangle.setFillColor(colour);
        this->rectangle.setPosition(posx.front(), posy.front());
        surface.draw(rectangle);
        
        disc.setPosition(newPos[0], newPos[1]);

    }
    
    
    void fullDraw()
    {
        for (int i=0;i<posx.size();i++)
        {
            this->rectangle.setFillColor(colour);
            this->rectangle.setPosition(posx.at(i), posy.at(i));
            surface.draw(this->rectangle);
        }
    }
    
    void reset(bool type)
    {
        posx.clear();
        posy.clear();
        
        if (type)
        {
            posx = {(3*gridWidth/4)*gridSize};
            posy = {(3*gridHeight/4)*gridSize};
            newPos[0] = posx.front();
            newPos[1] = posy.front();
            for (int i=0;i<2;i++){
                direction[i] = left[i];
                cur[i] = newPos[i];
            }

        } else{
            posx = {(gridWidth/4)*gridSize};
            posy = {(gridHeight/4)*gridSize};
            newPos[0] = posx.front();
            newPos[1] = posy.front();
            for (int i=0;i<2;i++){
                direction[i] = right[i];
                cur[i] = newPos[i];
            }
        }
        
        turned = false;
        speeds = 0;
        speedActive = false;
        speedLeft = boostAmount;
        invincibles = 0;
        invinceActive = false;
        invinceLeft = boostAmount;
    }
    
};

/*
 std::deque<int> posx = {(gridWidth/4)*gridSize};
 std::deque<int> posy = {(gridHeight/4)*gridSize};
 sf::Color colour = sf::Color(252, 93, 13,255);
 int score = 0;
 int direction[2] = {left[0],left[1]};
 int newPos[2] = {posx.front(),posy.front()};
 bool turned = false;
 sf::RectangleShape rectangle = sf::RectangleShape(sf::Vector2f(gridSize, gridSize));
 int cur[2] = {newPos[0],newPos[1]};
 int boostAmount = 80;
 int speeds = 0;
 bool speedActive = false;
 int speedLeft = boostAmount;
 int invincibles = 0;
 bool invinceActive = false;
 int invinceLeft = boostAmount;
 */

LightCycle lc1(true);
LightCycle lc2(false);

void resetScreen(void);

class Boost
{
    public:
    int speedPos[2];
    int invincePos[2];
    sf::Color speedCol = sf::Color(255,255,0,255);
    sf::Color invinceCol = sf::Color(216, 191, 216,255);
    
    sf::RectangleShape rectangle = sf::RectangleShape(sf::Vector2f(gridSize, gridSize));
    
    Boost()
    {
        randomizePosition(speedPos);
        randomizePosition(invincePos);
    }
    
    void randomizePosition(int* type)
    {
        type[0] = (std::rand() % gridWidth)*gridSize;
        type[1] = (std::rand() % gridHeight)*gridSize;
        
        //std::deque<int>::iterator it1x = );
        std::cout << lc1.posx.size();
        
        for (int it = 2; it < lc1.posx.size()-1; it++)
        {
            std::cout << 5;
            if(it > lc1.posy.size()-1 || it > lc2.posy.size()-1
               || it > lc1.posx.size()-1 || it > lc2.posx.size()-1)
                continue;
            
            if ((lc1.posx.at(it) == type[0] && type[1] == lc1.posy.at(it))
                || (lc2.posx.at(it) == type[0] && type[1] == lc2.posy.at(it))){
                randomizePosition(type);
                
            }
            
        }
        
        resetScreen();
        
    }
};

Boost boost;

void interaction()
{
    /*
    std::deque<int>::iterator it_lc1_lc1_x = std::find(lc1.posx.begin()+2, lc1.posx.end(), lc1.newPos[0]);
    std::deque<int>::iterator it_lc1_lc2_x = std::find(lc2.posx.begin()+2, lc2.posx.end(), lc1.newPos[0]);
    std::deque<int>::iterator it_lc2_lc2_x = std::find(lc2.posx.begin()+2, lc2.posx.end(), lc2.newPos[0]);
    std::deque<int>::iterator it_lc2_lc1_x = std::find(lc1.posx.begin()+2, lc1.posx.end(), lc2.newPos[0]);
    */
    bool lc1Die = false;
    bool lc2Die = false;
    
    if (lc1.posx.size() < 3 || lc2.posx.size() < 3
        || lc1.posy.size() < 3 || lc2.posy.size() < 3){
        return;
    }
    
    for (auto it = lc1.posx.begin()+1; it != lc1.posx.end(); it++)
    {
        auto distance = std::distance(lc1.posx.begin(),it);
        
        if (!distance)
            continue;
        
        if ( !&it )
            continue;
        
        
        if (*it == lc1.newPos[0]
            && lc1.newPos[1] == lc1.posy.at(distance) && !lc1.invinceActive){
            //
            std::cout << "A \n";
            lc1Die = true;
        }
        
        if (*it == lc2.newPos[0]
            && (lc2.newPos[1] == lc1.posy.at(distance) && !lc2.invinceActive)){
            //
            std::cout << "B \n";
            lc2Die = true;
        }
        
    }
   
    for (auto it2 = lc2.posx.begin()+1; it2 != lc2.posx.end(); it2++)
    {
        auto distance = std::distance(lc2.posx.begin(),it2);
        if (!distance)
            continue;
        
        if ( !&it2 )
            continue;
        
        if (*it2 == lc1.newPos[0]
            && (lc1.newPos[1] == lc2.posy.at(distance)) && !lc1.invinceActive){
            //
            std::cout << "C \n";
            lc1Die = true;
        }
        
        if (*it2 == lc2.newPos[0]
            && (lc2.newPos[1] == lc2.posy.at(distance)) && !lc2.invinceActive){
            //
            std::cout << "D \n";
            lc2Die = true;
        }
    }
    
    if (lc1Die == true && lc2Die == true){
        winner = "TIE";
        drawWinScreen = 1;
        std::cout << winner;
    } else if(lc1Die == true){
        winner = "ORANGE";
        lc2.score += 1;
        drawWinScreen = 1;
        std::cout << winner;
    } else if(lc2Die == true){
        winner = "BLUE";
        lc1.score += 1;
        drawWinScreen = 1;
        std::cout << winner;
    }
}

void drawGrid()
{
    for (int i=0; i<=gridWidth; i+=4)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(i*gridSize, 0)),
            sf::Vertex(sf::Vector2f(i*gridSize, screenHeight))
        };
            
        line[0].color = sf::Color(60,113,106,60);
        line[1].color = sf::Color(60,113,106,60);
        
        surface.draw(line, 2, sf::Lines);
        //std::cout << "amogus \n";
    }
    
    for (int i=0; i<=gridHeight; i+=4)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(0, i*gridSize)),
            sf::Vertex(sf::Vector2f(screenWidth, i*gridSize))
        };
            
        line[0].color = sf::Color(60,113,106,60);
        line[1].color = sf::Color(60,113,106,60);
        
        surface.draw(line, 2, sf::Lines);
        //std::cout << "amogus \n";
    }
}

void resetScreen()
{
    surface.clear(sf::Color(0, 1, 18));
    drawGrid();
    

    if (!font.loadFromFile(resourcePath() + "resources/Fonts/Nunito-Regular.ttf")) {
        return EXIT_FAILURE;
    }
    
    //Drawing the scores (Blue then orange)
    sf::Text lc1Score(std::to_string(lc1.score), font, 80);
    lc1Score.setOrigin(lc1Score.getLocalBounds().width, 0);
    lc1Score.setPosition(screenWidth-20, 0);
    lc1Score.setFillColor(lc1.colour);
    
    sf::Text lc2Score(std::to_string(lc2.score), font, 80);
    lc2Score.setPosition(20, 0);
    lc2Score.setFillColor(lc2.colour);

    //Do scores sometime here aswell
    surface.draw(lc1Score);
    surface.draw(lc2Score);
    
    boost.rectangle.setFillColor(boost.speedCol);
    boost.rectangle.setPosition(boost.speedPos[0],boost.speedPos[1]);
    surface.draw(boost.rectangle);
    
    boost.rectangle.setFillColor(boost.invinceCol);
    boost.rectangle.setPosition(boost.invincePos[0],boost.invincePos[1]);
    surface.draw(boost.rectangle);
    
    
    lc1.fullDraw();
    lc2.fullDraw();
    //(0, 1, 18)
}

void winScreen()
{

    
    sf::Text space;
    space.setFont(font);
    space.setCharacterSize(18);
    space.setString("Press SPACE to continue...");
    space.setFillColor(sf::Color::White);
    
    
    sf::Text won;
    won.setFont(font);
    won.setCharacterSize(72);
    
    if (winner != "TIE"){
        won.setString(winner + " Wins!!");
    } else {
        won.setString(winner + "!!");
    }
    won.setOrigin(won.getLocalBounds().width/2, won.getLocalBounds().height/2);
    won.setPosition(screenWidth/2, screenHeight/2.5);
    won.setFillColor(sf::Color::White);
    
    space.setPosition(screenWidth/2 - space.getLocalBounds().width/2, screenHeight - space.getLocalBounds().height*2);
    
    lc1Score.setString(std::to_string(lc1.score));
    lc2Score.setString(std::to_string(lc2.score));
    
    
    //Time to draw everything
    surface.draw(space);
    surface.draw(won);
    surface.draw(lc1Score);
    surface.draw(lc2Score);
    
    
    
    drawWinScreen = 2;
    
}

void boostManager()
{
    if (lc1.speedActive)
    {
        if (lc1.speedLeft <= 0) {
            lc1.speedActive = false;
            lc1.speedLeft = lc1.boostAmount;
        } else {
            lc1.speedLeft -= 1;
            lc1.move();
        }
        
    } else {
        if (timey%3 == 0)
            lc1.move();
    }
    
    if (lc1.invinceActive)
    {
        if (lc1.invinceLeft <= 0){
            lc1.invinceActive = false;
            lc1.invinceLeft = lc1.boostAmount;
        } else
            lc1.invinceLeft -= 1;
            
    }
    
    if (lc2.speedActive)
    {
        if (lc2.speedLeft <= 0) {
            lc2.speedActive = false;
            lc2.speedLeft = lc2.boostAmount;
        } else {
            lc2.speedLeft -= 1;
            lc2.move();
        }
        
    } else {
        if (timey%3 == 0)
            lc2.move();
    }
    
    if (lc2.invinceActive)
    {
        if (lc2.invinceLeft <= 0){
            lc2.invinceActive = false;
            lc2.invinceLeft = lc2.boostAmount;
        } else
            lc2.invinceLeft -= 1;
            
    }
    
}


int main(int, char const**)
{
    
    if (!surface.create(screenWidth, screenHeight))
        return -1;
    /*
    if (!music.openFromFile(resourcePath()+"resources/Music/rinzler.ogg"))
        return -1;
    musLength = music.getDuration().asSeconds();
    sf::Time elapsed1 = musTimer.getElapsedTime();
    */
    sf::Music music;
    if (!music.openFromFile(resourcePath() + "resources/Music/rinzler.ogg")) {
        return EXIT_FAILURE;
    }
    music.setVolume(100);
    music.play();
    musLength = music.getDuration().asSeconds();
    sf::Time elapsed1 = musTimer.getElapsedTime();
    
    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "resources/icon.png")) {
        return EXIT_FAILURE;
    }

    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    resetScreen();

    // Start the game loop
    while (window.isOpen())
    {
        sf::Time elapsed = glock.getElapsedTime();
        sf::sleep(sf::milliseconds(15-elapsed.asMilliseconds()));
        elapsed = glock.restart();
        
        timey += 1;
        
        if (elapsed1.asSeconds() > musLength)
         {
             curMusic = musics[rand() % 2];
             if (!music.openFromFile(resourcePath()+"resources/Music/"+curMusic))
                 return -1;
             musLength = music.getDuration().asSeconds();
             sf::Time elapsed1 = musTimer.getElapsedTime();
         }
            

        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed) {
                
                switch (event.key.code)
                {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                        
                    case sf::Keyboard::Space:
                        if (drawWinScreen != 0){
                            lc1.reset(true);
                            lc2.reset(false);
                            timey = 0;
                            boost.randomizePosition(boost.speedPos);
                            boost.randomizePosition(boost.invincePos);
                            resetScreen();
                            drawWinScreen = 0;
                        }
                        break;
                        
                    case sf::Keyboard::Left:
                        if (lc1.direction[0] != right[0])
                            lc1.turn(left);
                        break;
                        
                    case sf::Keyboard::Right:
                        if (lc1.direction[0] != left[0])
                            lc1.turn(right);
                        
                    case sf::Keyboard::Up:
                        if (lc1.direction[0] != down[0])
                            lc1.turn(up);
                        break;
                        
                    case sf::Keyboard::Down:
                        if (lc1.direction[0] != up[0])
                            lc1.turn(down);
                        break;
                    
                    case sf::Keyboard::A:
                        if (lc2.direction[0] != right[0])
                            lc2.turn(left);
                        break;
                        
                    case sf::Keyboard::D:
                        if (lc2.direction[0] != left[0])
                            lc2.turn(right);
                        
                    case sf::Keyboard::W:
                        if (lc2.direction[0] != down[0])
                            lc2.turn(up);
                        break;
                        
                    case sf::Keyboard::S:
                        if (lc2.direction[0] != up[0])
                            lc2.turn(down);
                        break;
                        
                    case sf::Keyboard::LShift:
                        if (lc2.speeds > 0 && !lc2.speedActive){
                            lc2.speedLeft = lc2.boostAmount;
                            lc2.speedActive = true;
                            lc2.speeds -= 1;
                        }
                        break;
                        
                    case sf::Keyboard::RShift:
                        if (lc1.speeds > 0 && !lc1.speedActive){
                            lc1.speedLeft = lc1.boostAmount;
                            lc1.speedActive = true;
                            lc1.speeds -= 1;
                        }
                        break;
                        
                    case sf::Keyboard::LAlt:
                        if (lc2.invincibles > 0 && !lc2.invinceActive){
                            lc2.invinceLeft = lc2.boostAmount;
                            lc2.invinceActive = true;
                            lc2.invincibles -= 1;
                        }
                        break;
                        
                    case sf::Keyboard::RAlt:
                        if (lc1.invincibles > 0 && !lc1.invinceActive){
                            lc1.invinceLeft = lc1.boostAmount;
                            lc1.invinceActive = true;
                            lc1.invincibles -= 1;
                        }
                        break;
                }
                 /*
                if (event.key.code == sf::Keyboard::Left && lc1.direction[0] != right[0]){
                    lc1.turn(left);
                } else if (event.key.code == sf::Keyboard::Right && lc1.direction[0] != left[0]) {
                    lc1.turn(right);
                } else if (event.key.code == sf::Keyboard::Up && lc1.direction[1] != down[1]) {
                    lc1.turn(up);
                    
                } else if (event.key.code == sf::Keyboard::Down && lc1.direction[1] != up[1]) {
                    lc1.turn(down);
                }
                
                if (event.key.code == sf::Keyboard::A && lc2.direction[0] != right[0]){
                    lc2.turn(left);
                } else if (event.key.code == sf::Keyboard::D && lc2.direction[0] != left[0]) {
                    lc2.turn(right);
                } else if (event.key.code == sf::Keyboard::W && lc2.direction[1] != down[1]) {
                    lc2.turn(up);
                } else if (event.key.code == sf::Keyboard::S && lc2.direction[1] != up[1]) {
                    lc2.turn(down);
                }
                */
                
                
                
                
            }

        }
        
        // surface.clear(sf::Color::red);
        //surface.draw(sprite);
        //surface.draw(text);
        if (drawWinScreen == 0)
        {
            boostManager();
            
            if (timey%800 == 0)
            {
                boost.randomizePosition(boost.speedPos);
                boost.randomizePosition(boost.invincePos);
            }
            
            if (lc1.newPos[0] == boost.speedPos[0] && lc1.newPos[1] == boost.speedPos[1])
            {
                lc1.speeds += 1;
                boost.randomizePosition(boost.speedPos);
                std::cout << "sus";
            } else if (lc2.newPos[0] == boost.speedPos[0] && lc2.newPos[1] == boost.speedPos[1]){
                lc2.speeds += 1;
                boost.randomizePosition(boost.speedPos);
            }
            
            if (lc1.newPos[0] == boost.invincePos[0] && lc1.newPos[1] == boost.invincePos[1])
            {
                lc1.invincibles += 1;
                boost.randomizePosition(boost.invincePos);
            } else if (lc2.newPos[0] == boost.invincePos[0] && lc2.newPos[1] == boost.invincePos[1]){
                lc2.invincibles += 1;
                boost.randomizePosition(boost.invincePos);
            }
            
            lc2.draw();
            lc1.draw();
            interaction();
        } else if(drawWinScreen == 1) {
            winScreen();
        } else {
            
        }
        
        
        
        
        
        surface.display();
            
        // Clear screen
        window.clear();
            
        // Draw the texture
        sf::Sprite sprite(surface.getTexture());
        window.draw(sprite);
        window.draw(lc1.disc);
        window.draw(lc2.disc);


        // Update the window
        window.display();
    }
    //music.stop();
    return EXIT_SUCCESS;
}

