#include "drawingObjects.h"
#include <math.h>

//internal helper function, returns square of euclid distance between points
static double distance(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return (dx * dx) + (dy * dy);
}

int Drawable::getState() {
    return state;
}

int Circle::onClick(double x, double y) {
    if(distance(x, y, center[0], center[1]) < radiusSquared) {
        //click was on this circle -- if shift pressed, delete circle
        if(SDL_GetModState() & KMOD_SHIFT) {
            state = DRAWABLE_EXPIRED;
            free(vertices);
            numVertices = 0;
        }
        return 1;
    }
    return 0;
}

void Circle::draw() {
    glBegin(GL_TRIANGLE_FAN);
    glColor3d(color[0], color[1], color[2]);
    
    for(int i = 0; i < numVertices; i++) {
        glVertex2dv(vertices[i]);
    }

    glEnd();
}

Circle::Circle(double r, double x, double y, double red, double grn, double blu) {
    radius = r;
    radiusSquared = r * r;
    center[0] = x;
    center[1] = y;
    color[0] = red;
    color[1] = grn;
    color[2] = blu;
    state = DRAWABLE_READY;
    
    //TODO -- restructure circle drawing
    //could make number of vertices depend on size of circle
    //  larger circle needs more vertices to look not-smooth
    //could also make the positions of each vertex be drawn from a file-static list
    //  don't need to recalculate the trig each time
    numVertices = 64;
    vertices = (vec2d *) malloc(sizeof(vec2d) * numVertices);
    double stepAngle = 3.14159265358 * 2.0 / numVertices;
    double theta;
    for(int i = 0; i < numVertices; i++) {
        theta = i * stepAngle;
        vertices[i][0] = center[0] + cos(theta) * radius;
        vertices[i][1] = center[1] + sin(theta) * radius;
    }
}

