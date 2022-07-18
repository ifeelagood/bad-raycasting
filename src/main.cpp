#include "main.h"
#include "vector2d.h"
#include "player.h"
#include "map.h"
#include "ini.h"
#include "lodepng.h"
#include <GL/freeglut_std.h>
#include <GL/gl.h>


Player player;

Vector2d pos;
Vector2d dir;
Vector2d plane;

typedef struct { int w,a,s,d; } ButtonKeys;
ButtonKeys Keys;

int level = 1;

int MAP_WIDTH, MAP_HEIGHT;

int** map;
mINI::INIFile file("maps/maps.ini");
mINI::INIStructure ini;


int getMapTile(int x, int y) { return map[y][x]; }


void ButtonDown(unsigned char key, int x, int y)
{

    if (key == 'w') { Keys.w = 1; }
    if (key == 'a') { Keys.a = 1; }
    if (key == 's') { Keys.s = 1; }
    if (key == 'd') { Keys.d = 1; }

}

void ButtonUp(unsigned char key, int x, int y)
{
    if (key == 'w') { Keys.w = 0; }
    if (key == 'a') { Keys.a = 0; }
    if (key == 's') { Keys.s = 0; }
    if (key == 'd') { Keys.d = 0; }
}

void resize(int w, int h)
{
    glutReshapeWindow(WIDTH, HEIGHT);
}

void HandleInput()
{
    // left or right
    if (Keys.a == 1) { player.rotate(-ROTSPEED); }
    if (Keys.d == 1) { player.rotate( ROTSPEED); }

    // forward or back
    if (Keys.w == 1)
    {
        Vector2d newPos = translate(player.pos, player.dir, MOVESPEED);
        Vector2i newMapPos;

        newMapPos.x = (int) newPos.x;
        newMapPos.y = (int) newPos.y;

        if (getMapTile(newMapPos.x, player.pos.y) == 0)
        {
            player.pos.x = newPos.x;
        }

        if (getMapTile(player.pos.x, newMapPos.y) == 0)
        {
            player.pos.y = newPos.y;
        }
    }

    if (Keys.s == 1)
    {
        Vector2d newPos = translate(player.pos, player.dir, -MOVESPEED);
        Vector2i newMapPos;

        newMapPos.x = (int) newPos.x;
        newMapPos.y = (int) newPos.y;

        if (getMapTile(newMapPos.x, player.pos.y) == 0)
        {
            player.pos.x = newPos.x;
        }

        if (getMapTile(player.pos.x, newMapPos.y) == 0)
        {
            player.pos.y = newPos.y;
        }
    }
}

std::vector<float> loadPNG(const char* filename, unsigned int &width, unsigned int &height)
{
    std::vector<unsigned char> rawImage; //the raw pixels


    unsigned error = lodepng::decode(rawImage, width, height, filename);

    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    std::vector<float> image(width*height*4);

    for (int i = 0; i < width*height*4; i++)
    {
        image[i] = (float) rawImage[i] / 255.0f;
    }

    return image;
}

void draw()
{
    for (int x = 0; x < WIDTH; x += RESOLUTION)
    {
        // determine vector for ray
        double cameraX = 2 * double(x) / double(WIDTH) - 1;

        Vector2d rayPosition = player.pos;
        Vector2d rayDirection = player.dir + player.cameraPlane * cameraX;

        Vector2i mapPosition(int(rayPosition.x), int(rayPosition.y));

        // calculate length to next line
        Vector2d delta;

        delta.x = sqrt(1 + (rayDirection.y * rayDirection.y) / (rayDirection.x * rayDirection.x));
        delta.y = sqrt(1 + (rayDirection.x * rayDirection.x) / (rayDirection.y * rayDirection.y));

        // determine which direction to step

        // is it horizontal or vertical?
        Vector2d nextSideDistance;
        Vector2i step;

        if (rayDirection.x < 0)
        {
            step.x = -1;
            nextSideDistance.x = (rayPosition.x - mapPosition.x) * delta.x; // position - map position * distance to side
        }
        else
        {
            step.x = 1;
            nextSideDistance.x = (mapPosition.x - rayPosition.x + 1.0f) * delta.x;
        }

        if (rayDirection.y < 0)
        {
            step.y = -1;
            nextSideDistance.y = (rayPosition.y - mapPosition.y) * delta.y;
        }
        else
        {
            step.y = 1;
            nextSideDistance.y = (mapPosition.y - rayPosition.y + 1.0f) * delta.y;
        }

        // perform DDA loop
        bool hit = 0;
        int side;

        while (hit == 0)
        {
            // jump to next map square
            if (nextSideDistance.x < nextSideDistance.y)
            {
                nextSideDistance.x += delta.x;
                mapPosition.x += step.x;
                side = 0;
                glColor3f(0.0, 0.9, 0.0);
            }
            else
            {
                nextSideDistance.y += delta.y;
                mapPosition.y += step.y;
                side = 1;
                glColor3f(0.0, 0.7, 0.0);
            }

            if (getMapTile(mapPosition.x, mapPosition.y) > 0) { hit = 1; }
        }

        // calculate distance of perpendicular ray

		double perpendicularWallDistance;
		if (side == 0) perpendicularWallDistance = (mapPosition.x - rayPosition.x + (1 - step.x) / 2 ) / rayDirection.x;
		else perpendicularWallDistance = (mapPosition.y - rayPosition.y + (1 - step.y) / 2 ) / rayDirection.y;

        perpendicularWallDistance = abs(perpendicularWallDistance);

        // calculate wall height
        int lineHeight = (int) (HEIGHT / perpendicularWallDistance);

        // create quad points
        // int x1, y1, x2, y2;

        // y1 = (-lineHeight / 2) + (HEIGHT / 2);
        // y2 = ( lineHeight / 2) + (HEIGHT/ 2);
        // x1 = x - (RESOLUTION / 2);
        // x2 = x + (RESOLUTION / 2);

        // //render

        // glBegin(GL_QUADS);
        //     glVertex2i(x1, y1);
        //     glVertex2i(x2, y1);
        //     glVertex2i(x2, y2);
        //     glVertex2i(x1, y2);
        // glEnd();

        for (int y = 0; y < lineHeight; y++)
        {

        }
    }
}

void drawTexture()
{
    const char* imagePath = "wolftex/bluestone.png";
    unsigned int imageWidth, imageHeight;
    std::vector<float> image = loadPNG(imagePath, imageWidth, imageHeight);


    int o = 0;
    for (int x = 0; x < imageWidth; x++)
    {
        for (int y = 0; y < imageHeight; y++)
        {
            float r,g,b,a;
            r = image[o  ];
            g = image[o+1];
            b = image[o+2];
            a = image[o+3];

            glPointSize(1);
            glColor3f(r,g,b);
            glBegin(GL_POINTS);
                glVertex2i(x,y);
            glEnd();

            o += 4;

        }
    }
}


void display()
{
    glutPostRedisplay();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    HandleInput();
    draw();
    glutSwapBuffers();
}

void timer(int)
{
    glutPostRedisplay();
    HandleInput();
    glutTimerFunc(1000/FPS, timer, 0);
}


void init()
{
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, WIDTH, HEIGHT, 0);

    // initialise player
    player.pos = Vector2d(1, 1);
    player.dir = Vector2d(-1, 0);
    player.cameraPlane = Vector2d(0, double(HEIGHT)/double(WIDTH));

    player.rotate(135);

    // initialise glut timer
    timer(1);

    // init ini
    file.read(ini);


    std::string& currentMap = ini["maps"][std::to_string(level)];

    map = loadMap(currentMap, MAP_WIDTH, MAP_HEIGHT);

}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(0, 0);

    glutCreateWindow("raycaster");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(ButtonDown);
    glutKeyboardUpFunc(ButtonUp);
    glutMainLoop();
}