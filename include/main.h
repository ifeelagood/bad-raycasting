#define RESOLUTION 4

#define ROTSPEED 120
#define MOVESPEED 1.5

#define MAX_FPS 240
#define FRAME_DELAY 1000 / MAX_FPS


#define screenWidth 1920
#define screenHeight 1080

#define texWidth 64
#define texHeight 64

// uncomment to use multiprocessing
// #define USE_MULTICORE


typedef struct { bool w,a,s,d,m,up,down,left,right,tab,esc,lshift; } ButtonKeys;
