#include <iostream>
#include <string>
#include <cmath>
#include <omp.h>
#include <CImg.h>
#include <new>

#include "quickcg.h"

#include "vector2d.h"
#include "map.h"
#include "ini.h"
#include "loadpng.h"
#include "config.h"
#include "timer.h"
#include "player.h"

#include "main.h"



Config config("config.ini");

int mapWidth, mapHeight;
int texWidth, texHeight;

bool done = false;

std::vector<std::vector<uint32_t>> texture;

fTimer timer;
ButtonKeys Keys;
Player player;

int level = 1;

int** map;


void putBuffer(uint32_t value, int x, int y, uint32_t* buffer)
{
    buffer[y*config.ScreenWidth+x] = value;
}

uint32_t* createBuffer()
{
    int& h = config.ScreenHeight;
    int& w = config.ScreenWidth;

    uint32_t* buffer = new uint32_t[w*h];

    return buffer;
}

void deleteBuffer(uint32_t* buffer)
{
    delete[] buffer;
}

void clearBuffer(uint32_t* buffer)
{
    int& h = config.ScreenHeight;
    int& w = config.ScreenWidth;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            putBuffer(0, x, y, buffer);
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

void movePlayer(double moveSpeed, bool strafe)
{
    Vector2d moveDirection = player.direction;

    if (strafe)
    {
        moveDirection.rotateByDeg(-90);
    }

    Vector2d newPos = translate(player.position, moveDirection, moveSpeed);
    Vector2d newPaddedPos = translate(newPos, moveDirection, moveSpeed*5);
    Vector2i newMapPos;

    newMapPos.x = (int) newPaddedPos.x;
    newMapPos.y = (int) newPaddedPos.y;

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


    if (Keys.lshift == 1)
    {
        deltaMoveSpeed *= config.SprintMultSpeed;
        deltaRotSpeed  *= config.SprintMultRotation;
    }

    // left or right
    if (Keys.left  == 1) { player.rotate(-deltaRotSpeed); }
    if (Keys.right == 1) { player.rotate( deltaRotSpeed); }

    if (Keys.w == 1) { movePlayer( deltaMoveSpeed, false); }
    if (Keys.s == 1) { movePlayer(-deltaMoveSpeed, false); }
    if (Keys.a == 1) { movePlayer( deltaMoveSpeed, true); }
    if (Keys.d == 1) { movePlayer(-deltaMoveSpeed, true); }


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
        nextSideDistance.y = (mapPosition.y - player.position.y + 1.0f) * rayStep.y; //
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

void drawRayFlat(uint32_t* buffer, int x, int y1, int y2)
{
    // int w = config.ScreenWidth;
    int h = config.ScreenHeight;
    uint32_t color = 0x00FF00FF;

    // drawing 90 degrees to fill consecutively for caching
    std::fill_n(buffer + x*h + y1, (y2 - y1), color); // everything from y1 to y2 as color
}

void drawRay(uint32_t* buffer, int x, int y1, int y2, int texID, double texX, double texOffset, double texStep)
{
    // int w = config.ScreenWidth;
    int h = config.ScreenHeight;

    for (int y = y1; y < y2; y++)
    {
        // cast texture coordinate to an integer, -1 incase of overflow
        int texY = (int) texOffset & (texHeight - 1);
        texOffset += texStep;

        // 90 degree cache hack
        uint32_t color = texture[texID][texX * texHeight + texY];

        buffer[x*h+y] = color;
    }
}

void calculateHitCoord(double& hitX, double distance, Vector2d& rayDirection, int side)
{
        if (side == 0) { hitX = player.position.y + distance * rayDirection.y; } // calculate exact hit position of the ray
        else           { hitX = player.position.x + distance * rayDirection.x; }
        hitX -= floor((hitX)); // minus the integer part
}

int calculateTextureColumn(double hitX, Vector2d& rayDirection, int side)
{
    int texX = int(hitX * double(texWidth));
    if (side == 0 && rayDirection.x > 0) { texX = texWidth - texX - 1; }
    if (side == 1 && rayDirection.y < 0) { texX = texWidth - texX - 1; }
    return texX;
}

void castRays(uint32_t* buffer)
{
    int res = config.VerticalResolution;

    // #pragma omp parallel for
    for (int x = 0; x < config.ScreenWidth; x += res)
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

        int L2 = lineHeight / 2;
        int H2 = config.ScreenHeight / 2;

        // ???????
        int pitch = 50;

        // determine y1 and y2 (top and bottom pixel)
        int y1 = -L2 + H2 + pitch;
        int y2 =  L2 + H2 + pitch;

        if (y1 < 0) { y1 = 0; }
        if (y2 >= config.ScreenHeight) { y2 = config.ScreenHeight - 1; }

        // texture calculations

        double hitX;
        calculateHitCoord(hitX, perpendicularWallDistance, rayDirection, side);

        // define x coordinate of texture
        int texX = calculateTextureColumn(hitX, rayDirection, side);

        // more texture calculations
        double texStep = 1.0 * texHeight / lineHeight;
        double texOffset = (y1 - pitch - H2 + L2) * texStep;

        int texID = tile - 1; // so we can use 0th texture

        // draw each line
        for (int dx = x; dx < x+res; dx++)
        {
            drawRay(buffer, dx, y1, y2, texID, texX, texOffset, texStep);
        }


        // render

        // drawRayFlat(buffer, x, y1, y2);
    }
}

void drawDebug(uint32_t* buffer)
{
    std::string fps_str = "FPS: " + std::to_string(timer.fps);
    // std::string avg_str = "AVG FPS / 10s: " + std::to_string(timer.getAverageFPS());
    std::string ftm_str = "Frametime: " + std::to_string(timer.frameTime);
    std::string dir_str = "Directon: [" + std::to_string(player.direction.x) + ',' + std::to_string(player.direction.y) + ']';
    std::string cam_str = "Camera: [" + std::to_string(player.cameraPlane.x) + ',' + std::to_string(player.cameraPlane.y) + ']';

    QuickCG::printString(buffer, fps_str, 10, 10);
    //QuickCG::printString(avg_str, 10, 20);
    QuickCG::printString(buffer, ftm_str, 10, 20);
    QuickCG::printString(buffer, dir_str, 10, 30);
    QuickCG::printString(buffer, cam_str, 10, 40);

}

void drawBackground(uint32_t* buffer)
{
    int SH = config.ScreenHeight;
    int SW = config.ScreenWidth;
    int SW2 = SW/2 + 50;

    // roof
    for (int y = 0; y < SH; y++)
    {
        for (int x = 0; x < SW2; x++)
        {
            putBuffer(0x4D4D4DFF, x, y, buffer);
        }
    }

    // floor
    for (int y = 0; y < SH; y++)
    {
        for (int x = SW2; x < SW; x++)
        {
            putBuffer(0x888C8DFF, x, y, buffer);
        }
    }
}

void drawCrosshair(uint32_t* buffer, int size, int width)
{
    uint32_t color = 0x00FF00FF;

    int SH2 = config.ScreenHeight/2;
    int SW2 = config.ScreenWidth/2;

    // left->right, for y width
    int hx1, hx2, hy1, hy2;
    hx1 = SW2-size; hy1 = SH2-width;
    hx2 = SW2+size; hy2 = SH2+width;

    for (int y = hy1; y < hy2; y++)
    {
        for (int x = hx1; x < hx2; x++)
        {
            putBuffer(color, x, y, buffer);
        }
    }

    // top->bottom, for x width
    int vx1, vx2, vy1, vy2;
    vx1 = SW2-width; vy1 = SH2-size;
    vx2 = SW2+width; vy2 = SH2+size;

    for (int y = vy1; y < vy2; y++)
    {
        for (int x = vx1; x < vx2; x++)
        {
            putBuffer(color, x, y, buffer);
        }
    }

}

void display(uint32_t* buffer)
{
    while (!done)
    {
        // user input
        QuickCG::readKeys();
        getKeys();
        handleInput(player);
        // rendering
        drawBackground(buffer);
        castRays(buffer);
        drawCrosshair(buffer, 5, 1);
        // QuickCG::drawBufferAVX(buffer);
        // QuickCG::drawBuffer2(buffer);

        if (Keys.tab == 1) { drawDebug(buffer); }
        QuickCG::redraw(buffer);
        clearBuffer(buffer);
        timer.update(QuickCG::getTicks());


    }
}


int main()
{
    // create buffer
    uint32_t* buffer = createBuffer();


    // initialise player
    player.position = Vector2d(1.5, 1.5);
    player.direction = Vector2d(-1.0, 0);

    // fix camera perpendicular to view direction
    player.cameraPlane.x =  player.direction.y;
    player.cameraPlane.y = -player.direction.x;

    // scale the length of the camera vector by
    // half of the aspect ratio
    double r = (double) config.ScreenWidth / (double) config.ScreenHeight;
    double cameraMult = r / 2.0;
    player.cameraPlane *= cameraMult;

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