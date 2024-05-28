#define SDL_MAIN_HANDLED
#include "drawingObjects.h"

#include <GL/GLU.h>

//function to initialize the display, returns nonzero iff error
int initialize();

//main program tick, returns nonzero while program should continue running
int mainLoop();

//function to refresh the display
void updateDisplay();

//process events which resize the display, return nonzero if this event did
int checkResize(SDL_Event);
//process events which move the camera, return nonzero if this event did
int checkMotion(SDL_Event);
//process mouse click events for painting on the display, return nonzero if this one did
int checkPainting(SDL_Event);
//determine whether this event should end the program
int checkQuits(SDL_Event);

//convenience function to translate display pixels to world coordinates
void  screenToWorld(double *x, double *y);

#include <vector>

using std::vector;


#define MOVE_STEP (0.05)
#define ZOOM_STEP (1.1)

//begin globals 
SDL_Window    *window;
SDL_GLContext  context;

//current width/height of window
int width = 720;
int height = 720;

//current camera information
double centerX = 0.0;
double centerY = 0.0;
double scaleFactor = 1.0;

//all current drawables 
vector<Drawable *> objects;
//end globals

int main(int argc, char **argv) {
    if(initialize()) { return 1; }
    updateDisplay();

    while(mainLoop()) {}
    
    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}

int initialize() {
    if(SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL initialization failed. Error: %s", SDL_GetError());
        return 1;
    }
    
    window = SDL_CreateWindow("Circle Dropper",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              width, height, 
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(!window) {
        SDL_Log("Window creation failed. Error: %s", SDL_GetError());
        return 1;
    }

    context = SDL_GL_CreateContext(window);
    if(!context) {
        SDL_Log("Context creation failed. Error: %s", SDL_GetError());
        return 1;
    }
    
    glMatrixMode(GL_PROJECTION);
    glClearColor(0.8, 0.8, 0.8, 1.0);
    
    objects.push_back(new Circle(0.5, 0, 0, 0, 0, 0.5));
    
    return 0;
}

int mainLoop() {
    static SDL_Event e;
    static int redraw = 1;

    redraw = 0;
    while(SDL_PollEvent(&e)) {

        if(checkResize(e)) { redraw = 1; }

        if(checkMotion(e)) { redraw = 1; }

        if(checkPainting(e)) { redraw = 1; }

        if(checkQuits(e)) { return 0; }
    }

    if(redraw) {
        updateDisplay();
    }

    return 1;
}

void updateDisplay() {
    static double aspectRatio;
    
    //refresh basic display information
    glViewport(0, 0, width, height);
    aspectRatio = ((double) width) / height;

    //refresh camera transformation
    //this is done here as checkResize or checkMotion may both effect a change 
    glLoadIdentity();
    glOrtho(centerX - (aspectRatio * scaleFactor),
            centerX + (aspectRatio * scaleFactor),
            centerY - (scaleFactor),
            centerY + (scaleFactor),
            -1, 1);

    
    glClear(GL_COLOR_BUFFER_BIT);
    
    for(int i = 0; i < objects.size(); i++) {
        objects[i]->draw();
    }

    glFlush();
    
    SDL_GL_SwapWindow(window);
}

int checkResize(SDL_Event e) {
    if((e.type == SDL_WINDOWEVENT) && (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
        SDL_GL_GetDrawableSize(window, &width, &height);
        return 1;
    }
    return 0;
}

int checkMotion(SDL_Event e) {
    if(e.type == SDL_KEYDOWN) {
        switch(e.key.keysym.sym) {
            case SDLK_w:
            case SDLK_UP:
                centerY += MOVE_STEP * scaleFactor;
                break;
            case SDLK_a:
            case SDLK_LEFT:
                centerX -= MOVE_STEP * scaleFactor;
                break;
            case SDLK_s:
            case SDLK_DOWN:
                centerY -= MOVE_STEP * scaleFactor;
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                centerX += MOVE_STEP * scaleFactor;
                break;
            case SDLK_q:
            case SDLK_KP_MINUS:
                scaleFactor *= ZOOM_STEP;
                break;
            case SDLK_e:
            case SDLK_KP_PLUS:
                scaleFactor /= ZOOM_STEP;
                break;
            default:
                return 0;
                break;
        }
        return 1;
    }
    return 0;
}

int checkPainting(SDL_Event e) {
    if(e.type == SDL_MOUSEBUTTONDOWN) {
        double x = e.button.x;
        //sdl measurements are top-down, using bottom-up with opengl
        double y = height - e.button.y;
        screenToWorld(&x, &y);
        
        int i;
        for(i = 0; i < objects.size(); i++) {
            if(objects[i]->onClick(x, y)) {
                if(objects[i]->getState() == DRAWABLE_EXPIRED) {
                    //order of objects need not be preserved
                    //accelerate removal via pulling the last thing to the should-be-empty spot
                    objects[i] = objects.back();
                    objects.pop_back();
                }
                return 1;
            }
        }
        
        objects.push_back(new Circle(0.25, x, y, 0, 0.5, 0));
        return 1;
    }

    return 0;
}

int checkQuits(SDL_Event e) {
    if(e.type == SDL_QUIT) {
        return 1;
    }
    if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        return 1;
    }
    return 0;
}


void screenToWorld(double *x, double *y) {
    double z = 0;
    double model[16];
    double proj[16];
    int view[4]; 

    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, view);

    gluUnProject(*x, *y, z, model, proj, view, x, y, &z);
}

