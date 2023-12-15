#include "header.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int NUM_CIRCLES = 5;
const float CIRCLE_RADIUS = 20.0;
const float HORIZONTAL_SPACE = 20.0;
const float VERTICAL_SPACE = 20.0;
const float BOX_HEIGHT = 20.0;
const float BOX_SPACING = 5.0;
const float BOTTOM_SPACE = 10.0;

float smallCircleRadius = 5.0;
const int numSmallCircles = 8;

float smallCircleSpeed = 2.0; // Adjust the speed of small circles

typedef struct SmallCircle {
    float x;
    float y;
    bool movingToBlueBox;
}SmallCircle;

SmallCircle smallCircles[8];

void drawCircle(float x, float y, float radius) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int j = 0; j <= 100; ++j) {
        float theta = 2.0f * M_PI * (float)j / 100.0f;
        float cx = radius * cosf(theta);
        float cy = radius * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

void drawSmallCircles() {
    glColor3f(0.0, 0.0, 0.0); // Black

    for (int i = 0; i < numSmallCircles; ++i) {
        drawCircle(smallCircles[i].x, smallCircles[i].y, smallCircleRadius);
    }
}

void updateSmallCircles() {
    for (int i = 0; i < numSmallCircles; ++i) {
        if (smallCircles[i].movingToBlueBox) {
            // Move towards the blue box
            float targetX = 0.8 * WINDOW_WIDTH - HORIZONTAL_SPACE - 30.0;
            float targetY = WINDOW_HEIGHT - VERTICAL_SPACE - (i + 1) * (BOX_HEIGHT + BOX_SPACING + BOTTOM_SPACE) - 30.0;

            if (smallCircles[i].x < targetX) {
                smallCircles[i].x += smallCircleSpeed;
            } else {
                // Arrived at the target position in the blue box
                smallCircles[i].movingToBlueBox = false;
            }
        } else {
            // Move towards the specific location in the black box
            float targetX = HORIZONTAL_SPACE + 30.0; // Adjust the target X position
            float targetY = WINDOW_HEIGHT - VERTICAL_SPACE - 8 - i * (BOX_HEIGHT + BOX_SPACING + 10.0) - 30.0; // Adjust the target Y position

            // Move towards the target position
            if (smallCircles[i].x > targetX) {
                smallCircles[i].x -= smallCircleSpeed;
            } else {
                smallCircles[i].movingToBlueBox = true;
            }
        }
    }
}

void drawRectangle(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void drawQuadrilateral(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glVertex2f(x4, y4);
    glEnd();
}

void displayCircles() {
    

    drawSmallCircles();
}

void displayBoxes() {

    // Draw red box
    glColor3f(1.0, 0.0, 0.0); // Red
    drawRectangle(0.8 * WINDOW_WIDTH - HORIZONTAL_SPACE, VERTICAL_SPACE,
                  WINDOW_WIDTH - HORIZONTAL_SPACE, WINDOW_HEIGHT - VERTICAL_SPACE);

    //blue boxes inside the red box
    glColor3f(0.0, 0.0, 1.0); // Blue
    for (int i = 0; i < 5; ++i) {
        drawRectangle(0.8 * WINDOW_WIDTH - HORIZONTAL_SPACE,
                      WINDOW_HEIGHT - VERTICAL_SPACE - (i + 1) * (BOX_HEIGHT + BOX_SPACING + BOTTOM_SPACE),
                      WINDOW_WIDTH - HORIZONTAL_SPACE,
                      WINDOW_HEIGHT - VERTICAL_SPACE - i * (BOX_HEIGHT + BOX_SPACING + BOTTOM_SPACE));
    }

    // Draw green box on the left side
    glColor3f(0.0, 1.0, 0.0); // Green
    drawRectangle(HORIZONTAL_SPACE, VERTICAL_SPACE,
                  0.8 * WINDOW_WIDTH - 2 * HORIZONTAL_SPACE, WINDOW_HEIGHT - VERTICAL_SPACE);

    // Draw dark green table in the green box
    glColor3f(0.0, 0.5, 0.0); // Dark Green
    drawQuadrilateral(HORIZONTAL_SPACE + 10, VERTICAL_SPACE + 10,
                      HORIZONTAL_SPACE + 30, VERTICAL_SPACE + 10,
                      HORIZONTAL_SPACE + 30, WINDOW_HEIGHT - VERTICAL_SPACE - 10,
                      HORIZONTAL_SPACE + 10, WINDOW_HEIGHT - VERTICAL_SPACE - 10);

    //Draw multiple small boxes (cashiers)
    glColor3f(0.0, 0.0, 0.0); // Black
    float cashierSpacing = 10.0;
    for (int i = 0; i < MAX_CASHIER; ++i) {
        drawRectangle(HORIZONTAL_SPACE + 12, WINDOW_HEIGHT - VERTICAL_SPACE - 12 - i * (BOX_HEIGHT + BOX_SPACING + cashierSpacing),
                      HORIZONTAL_SPACE + 28, WINDOW_HEIGHT - VERTICAL_SPACE - 8 - i * (BOX_HEIGHT + BOX_SPACING + cashierSpacing));
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    displayCircles();
    displayBoxes();

    glFlush();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
}

void animate(int value) {
    updateSmallCircles();
    glutPostRedisplay();
    glutTimerFunc(16, animate, 0);//60 frames per second
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Moving Circles");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, animate, 0); //animation loop

    glClearColor(1.0, 1.0, 1.0, 1.0);

    for (int i = 0; i < numSmallCircles; ++i) {
        smallCircles[i].x = 400.0 + 100.0 * cosf(2.0f * M_PI * (float)i / (float)numSmallCircles);
        smallCircles[i].y = 300.0 + 100.0 * sinf(2.0f * M_PI * (float)i / (float)numSmallCircles);
        smallCircles[i].movingToBlueBox = true;
    }

    glutMainLoop();

    return 0;
}



//gcc -o my_program oGl.c -lGL -lGLU -lglut -lm