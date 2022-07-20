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


uint32_t** createBuffer()
{
    int& h = config.ScreenHeight;
    int& w = config.ScreenWidth;

    uint32_t** buffer = new uint32_t*[h];

    for (int i = 0; i < h; i++)
    {
        buffer[i] = new uint32_t[w];
    }

    return buffer;
}

void deleteBuffer(uint32_t** buffer)
{
    int& h = config.ScreenHeight;
    // int& w = config.ScreenWidth;

    for (int i = 0; i < h; i++)
    {
            delete[] buffer[i];
    }

    delete[] buffer;
}

void clearBuffer(uint32_t** buffer)
{
    int& h = config.ScreenHeight;
    int& w = config.ScreenWidth;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            buffer[y][x] = 0;
        }
    }
}

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

    double deltaMoveSpeed = config.MoveSpeed * timer.frameTime;
    double deltaRotSpeed  = config.RotationSpeed * timer.frameTime;


    // left or right
    if (Keys.a == 1) { player.rotate(-deltaRotSpeed); }
    if (Keys.d == 1) { player.rotate( deltaRotSpeed); }


    if (Keys.lshift == 1) { deltaMoveSpeed *= 1.7f; }

    if (Keys.w == 1) { movePlayer( deltaMoveSpeed); }
    if (Keys.s == 1) { movePlayer(-deltaMoveSpeed); }


    if (Keys.esc == 1) { done = true; }
}

double DDA(Vector2d &rayPosition, Vector2d &rayDirection, int &side, int &tile)
{
    Vector2i mapPosition(int(rayPosition.x), int(rayPosition.y));

    Vector2d rayStep;

    // calculate the distance of x for 1 unit of y and vise versa
    rayStep.x = (rayDirection.x == 0) ? 1e30 : std::abs(1 / rayDirection.x);
    rayStep.y = (rayDirection.y == 0) ? 1e30 : std::abs(1 / rayDirection.y);

    // find step direction for map and set initial nextSideDistace based on direction and position
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
    bool hit = false;
    while (!hit)
    {
        // move in direction based on larger distance
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

    // calculate the wall distance, as euclidian distance produces fisheye
    double perpendicularWallDistance;
    if (side == 0) { perpendicularWallDistance = (nextSideDistance.x - rayStep.x); }
    else           { perpendicularWallDistance = (nextSideDistance.y - rayStep.y); }

    return perpendicularWallDistance;
}

void drawRays3D(uint32_t** buffer)
{
    for (int x = 0; x < config.ScreenWidth; x++)
    {
        // determine vector for ray
        double cameraX = 2 * (double)x / (double)config.ScreenWidth - 1;

        Vector2d rayPosition = player.position;
        Vector2d rayDirection = player.direction + player.cameraPlane * cameraX;

        int side,tile;
        // calculate DDA step

        double perpendicularWallDistance = DDA(rayPosition, rayDirection, side, tile);

        // calculate wall height
        int lineHeight = (int)(config.ScreenHeight / perpendicularWallDistance);


        // ???????
        int pitch = 100;

        // determine y1 and y2 (top and bottom pixel)
        int y1 = -lineHeight / 2 + config.ScreenHeight / 2 + pitch;
        int y2 =  lineHeight / 2 + config.ScreenHeight / 2 + pitch;

        if (y1 < 0)             { y1 = 0;                }
        if (y2 >= config.ScreenHeight) { y2 = config.ScreenHeight - 1; }

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
        double texOffset = (y1 - pitch - (config.ScreenHeight / 2) + (lineHeight / 2)) * texStep;

        int texID = tile - 1; // so we can use 0th texture

        // render
        for (int y = y1; y < y2; y++)
        {
            // cast texture coordinate to an integer, -1 incase of overflow
            int texY =  (int)texOffset & (texHeight - 1);
            texOffset += texStep;

            uint32_t color = texture[texID][texHeight * texY + texX];

            // if(side == 1) { color = (color >> 1) & 8355711; }

            buffer[y][x] = color;
        }
    }
}

void drawDebug()
{
    std::string fps_str = "FPS: " + std::to_string(timer.fps);
    std::string avg_str = "AVG FPS / 10s: " + std::to_string(timer.getAverageFPS());
    std::string ftm_str = "Frametime: " + std::to_string(timer.frameTime);
    std::string dir_str = "Directon: [" + std::to_string(player.direction.x) + ',' + std::to_string(player.direction.y) + ']';
    std::string cam_str = "Camera: [" + std::to_string(player.cameraPlane.x) + ',' + std::to_string(player.cameraPlane.y) + ']';

    QuickCG::printString(fps_str, 10, 10);
    QuickCG::printString(avg_str, 10, 20);
    QuickCG::printString(ftm_str, 10, 30);
    QuickCG::printString(dir_str, 10, 40);
    QuickCG::printString(cam_str, 10, 50);

}

void display(uint32_t** buffer)
{
    while (!done)
    {
        // user input
        QuickCG::readKeys();
        getKeys();
        handleInput(player);
        // rendering
        drawRays3D(buffer);
        QuickCG::drawBufferP2P(buffer);
        clearBuffer(buffer);

        timer.update(QuickCG::getTicks());
        if (Keys.tab == 1) { drawDebug(); }
        QuickCG::redraw();

    }
}


int main()
{
    // create buffer
    uint32_t** buffer = createBuffer();


    // initialise player
    player.position = Vector2d(1.5, 1.5);
    player.direction = Vector2d(-1.0, 0.0);

    player.cameraFix(); // fix camera vector perpendicular to player direction
    player.rotate(180);


    // initialise map ini and map
    map = loadMap("maps/map1.txt", mapWidth, mapHeight);


    // initialise texture
    texture.resize(config.TextureCount); // resize outer vector to fit textures
    for (int i = 0; i < config.TextureCount; i++) { loadPNG(texture[i], texWidth, texHeight, config.TexturePaths[i]); }


    // initialise window
    QuickCG::screen(config.ScreenWidth, config.ScreenHeight, 0, "raycaster");


    // start display loop
    display(buffer);

}