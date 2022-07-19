#include <iostream>
#include <string>
#include <cmath>
#include <CImg.h>

#include "quickcg.h"

#include "vector2d.h"
#include "map.h"
#include "player.h"
#include "ini.h"
#include "timer.h"
#include "loadpng.h"


#include "main.h"

bool done = false;

Uint32 buffer[screenHeight][screenWidth];

void clearBuffer()
{
    for (int y = 0; y < screenHeight; y++)
    {
        for (int x = 0; x < screenWidth; x++)
        {
            buffer[y][x] = 0;
        }
    }
}


std::vector<Uint32> texture[3];

Timer timer;

ButtonKeys Keys;

Player player;

int level = 1;

int** map;

int getMapTile(int x, int y) { return map[y][x]; }

void getKeys()
{
    Keys.w = QuickCG::keyDown(SDL_SCANCODE_W);
    Keys.a = QuickCG::keyDown(SDL_SCANCODE_A);
    Keys.s = QuickCG::keyDown(SDL_SCANCODE_S);
    Keys.d = QuickCG::keyDown(SDL_SCANCODE_D);

    Keys.m = QuickCG::keyDown(SDL_SCANCODE_M);

    Keys.tab = QuickCG::keyDown(SDL_SCANCODE_TAB);
    Keys.esc = QuickCG::keyDown(SDL_SCANCODE_ESCAPE);
    Keys.lshift = QuickCG::keyDown(SDL_SCANCODE_LSHIFT);

    Keys.up = QuickCG::keyDown(SDL_SCANCODE_UP);
    Keys.down = QuickCG::keyDown(SDL_SCANCODE_DOWN);
    Keys.left = QuickCG::keyDown(SDL_SCANCODE_LEFT);
    Keys.right = QuickCG::keyDown(SDL_SCANCODE_RIGHT);
}

void movePlayer(double moveSpeed)
{
    Vector2d newPos = translate(player.position, player.direction, moveSpeed);

    Vector2i newMapPos;

    newMapPos.x = (int) newPos.x;
    newMapPos.y = (int) newPos.y;

    if (getMapTile(newMapPos.x, player.position.y) == 0)
    {
        player.position.x = newPos.x;
    }

    if (getMapTile(player.position.x, newMapPos.y) == 0)
    {
        player.position.y = newPos.y;
    }
}


void handleInput(Player &player)
{

    double deltaMoveSpeed = MOVESPEED * timer.frameTime;
    double deltaRotSpeed  = ROTSPEED  * timer.frameTime;


    // left or right
    if (Keys.a == 1) { player.rotate(-deltaRotSpeed); }
    if (Keys.d == 1) { player.rotate( deltaRotSpeed); }


    if (Keys.lshift == 1) { deltaMoveSpeed *= 1.7f; }

    if (Keys.w == 1) { movePlayer( deltaMoveSpeed); }
    if (Keys.s == 1) { movePlayer(-deltaMoveSpeed); }


    if (Keys.esc == 1) { done = true; }
}

double DDA(int x, Vector2d &rayPosition, Vector2d &rayDirection, int &side, int &tile)
{
    Vector2i mapPosition(int(rayPosition.x), int(rayPosition.y));

    Vector2d rayStep;

        rayStep.x = (rayDirection.x == 0) ? 1e30 : std::abs(1 / rayDirection.x);
        rayStep.y = (rayDirection.y == 0) ? 1e30 : std::abs(1 / rayDirection.y);


        // is it horizontal or vertical?
        Vector2d nextSideDistance;
        Vector2i mapStep;

        if (rayDirection.x < 0)
        {
            mapStep.x = -1;
            nextSideDistance.x = (player.position.x - mapPosition.x) * rayStep.x;
        }
        else
        {
            mapStep.x = 1;
            nextSideDistance.x = (mapPosition.x - player.position.x + 1.0f) * rayStep.x;
        }

        if (rayDirection.y < 0)
        {
            mapStep.y = -1;
            nextSideDistance.y = (player.position.y - mapPosition.y) * rayStep.y;
        }
        else
        {
            mapStep.y = 1;
            nextSideDistance.y = (mapPosition.y - player.position.y + 1.0f) * rayStep.y;
        }

        // perform DDA loop
        bool hit = 0;


        while (hit == 0)
        {
            // jump to next map square
            if (nextSideDistance.x < nextSideDistance.y)
            {
                nextSideDistance.x += rayStep.x;
                mapPosition.x += mapStep.x;
                side = 0;
            }
            else
            {
                nextSideDistance.y += rayStep.y;
                mapPosition.y += mapStep.y;
                side = 1;
            }

            tile = getMapTile(mapPosition.x, mapPosition.y);

            hit = (tile > 0);
        }

        // calculate distance of perpendicular ray

        // std::cout << nextSideDistance.x << ',' << nextSideDistance.y << '\n';

		double perpendicularWallDistance;
        if (side == 0) { perpendicularWallDistance = (nextSideDistance.x - rayStep.x); }
		else           { perpendicularWallDistance = (nextSideDistance.y - rayStep.y); }

    return perpendicularWallDistance;
}
void drawRays3D(unsigned long mapWidth, unsigned long mapHeight)
{
    #ifdef USE_MULTICORE
    #pragma omp parallel for
    #endif
    for (int x = 0; x < screenWidth; x++)
    {
        // determine vector for ray
        double cameraX = 2 * (double)x / (double)screenWidth - 1;

        Vector2d rayPosition = player.position;
        Vector2d rayDirection = player.direction + player.cameraPlane * cameraX;

        int side,tile;
        // calculate DDA step

        double perpendicularWallDistance = DDA(x, rayPosition, rayDirection, side, tile);

        // calculate wall height
        int lineHeight = (int)(screenHeight / perpendicularWallDistance);


        // ???????
        int pitch = 100;

        // determine y1 and y2 (top and bottom pixel)
        int y1 = -lineHeight / 2 + screenHeight / 2 + pitch;
        int y2 =  lineHeight / 2 + screenHeight / 2 + pitch;

        if (y1 < 0)             { y1 = 0;                }
        if (y2 >= screenHeight) { y2 = screenHeight - 1; }

        // texture calculations

        double hitX;

        if (side == 0) { hitX = player.position.y + perpendicularWallDistance * rayDirection.y; } // calculate exact hit position of the ray
        else           { hitX = player.position.x + perpendicularWallDistance * rayDirection.x; }
        hitX -= floor((hitX)); // minus the integer part

        // define x coordinate of texture

        int texX = int(hitX * double(texWidth));
        if (side == 0 && rayDirection.x > 0) { texX = texWidth - texX - 1; }
        if (side == 1 && rayDirection.y < 0) { texX = texWidth - texX - 1; }

        // more texture calculations
        double texStep = 1.0 * texHeight / lineHeight;
        double texOffset = (y1 - pitch - (screenHeight / 2) + (lineHeight / 2)) * texStep;

        int texID = tile - 1; // so we can use 0th texture

        // render
        for (int y = y1; y < y2; y++)
        {
            // cast texture coordinate to an integer, -1 incase of overflow
            int texY =  (int)texOffset & (texHeight - 1);
            texOffset += texStep;

            Uint32 color = texture[texID][texHeight * texY + texX];

            // if(side == 1) { color = (color >> 1) & 8355711; }

            buffer[y][x] = color;
        }
    }
}



void drawDebug()
{
    std::string fps_str = "FPS: " + std::to_string(timer.fps);
    std::string ftm_str = "Frametime: " + std::to_string(timer.frameTime);
    std::string dir_str = "Directon: [" + std::to_string(player.direction.x) + ',' + std::to_string(player.direction.y) + ']';
    std::string cam_str = "Camera: [" + std::to_string(player.cameraPlane.x) + ',' + std::to_string(player.cameraPlane.y) + ']';



    QuickCG::printString(fps_str, 10, 10);
    QuickCG::printString(ftm_str, 10, 20);
    QuickCG::printString(dir_str, 10, 30);
    QuickCG::printString(cam_str, 10, 40);

}

void leaveMyCPUALONE()
{
    SDL_Delay(5);
}


void display(unsigned long mapWidth, unsigned long mapHeight)
{
    while (!done)
    {
        leaveMyCPUALONE();
        // user input
        QuickCG::readKeys();
        getKeys();
        handleInput(player);
        // rendering
        drawRays3D(mapWidth, mapHeight);
        QuickCG::drawBuffer(buffer[0]);
        clearBuffer();

        timer.update(QuickCG::getTicks());
        if (Keys.tab == 1) { drawDebug(); }
        QuickCG::redraw();

    }
}


int main(int argc, char* argv[])
{


    // initialise player

    player.position = Vector2d(1.5, 1.5);
    player.direction = Vector2d(-1.0, 0.0);

    // perpendicular to direction
    player.cameraFix();

    player.rotate(180);


    // initialise map ini and map
    mINI::INIFile file("maps/maps.ini");
    mINI::INIStructure mapini;

    file.read(mapini);

    std::string& currentMap = mapini["maps"][std::to_string(level)];

    unsigned long mapWidth, mapHeight;

    map = loadMap(currentMap, mapWidth, mapHeight);

    // initialise textures

    int texCount = 3;

    for (int i = 0; i < texCount; i++) { texture[i].resize(texWidth * texHeight); }

    int tw, th;

    loadPNG(texture[0], tw, th, "wolftex/greystone.png");
    loadPNG(texture[1], tw, th, "wolftex/mossy.png");
    loadPNG(texture[2], tw, th, "wolftex/bluestone.png");


    // initialise window
    QuickCG::screen(screenWidth, screenHeight, 0, "raycaster");

    // start display loop
    display(mapWidth, mapHeight);

}