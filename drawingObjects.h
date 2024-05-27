#ifndef DRAWINGOBJECTS_H
#define DRAWINGOBJECTS_H

#include "SDL.h"
#include "SDL2/SDL_opengl.h"

//constants representing states of a drawable object
#define  DRAWABLE_EXPIRED  (0)
#define  DRAWABLE_READY    (1)

//convenience typedef -- 2d points may be used frequently, reduce chance for error
typedef double vec2d[2];

//abstract class representing objects which can be drawn
class Drawable {
    public:
        //function for an object to process a click at a given location
        //returns nonzero iff the object was clicked on
        virtual int onClick(double x, double y) = 0;
        //function to draw an object
        virtual void draw() = 0;
        int getState();
    private:
    protected:
        int state;
};

//drawable specification for circles
class Circle : public Drawable {
    public:
        Circle(double r, double x, double y, double red, double grn, double blu);
        int onClick(double x, double y);
        void draw();
    private:
        double radius, radiusSquared, color[3];
        int numVertices;
        vec2d center, *vertices;
};



#endif
