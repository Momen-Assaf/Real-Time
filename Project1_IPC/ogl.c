#include "header.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float HORIZONTAL_SPACE = 20.0;
const float VERTICAL_SPACE = 20.0;
const float BOX_HEIGHT = 20.0;
const float BOX_SPACING = 5.0;
const float BOTTOM_SPACE = 10.0;

float smallCircleRadius = 5.0;
float smallCircleSpeed = 2.0;

typedef struct SmallCircle
{
    float x;
    float y;
    bool movingToBlueBox;
    int id;
    int targetCashierID;
} SmallCircle;

SmallCircle smallCircles[8];

typedef struct Cashier
{
    float x1, y1, x2, y2;
    int id;
} Cashier;

Cashier cashiers[MAX_CASHIER];

void drawCircle(float x, float y, float radius)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int j = 0; j <= 100; ++j)
    {
        float theta = 2.0f * M_PI * (float)j / 100.0f;
        float cx = radius * cosf(theta);
        float cy = radius * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

void drawSmallCircles()
{
    glColor3f(0.0, 0.0, 0.0); // Black

    for (int i = 0; i < MAX_CUS; ++i)
    {
        drawCircle(smallCircles[i].x, smallCircles[i].y, smallCircleRadius);
    }
}

void updateSmallCircles() {
    for (int i = 0; i < MAX_CUS; ++i) {
        if (smallCircles[i].movingToBlueBox) {
            float targetX = 0.8 * WINDOW_WIDTH - HORIZONTAL_SPACE - 30.0;
            float targetY = WINDOW_HEIGHT - VERTICAL_SPACE - (i + 1) * (BOX_HEIGHT + BOX_SPACING + BOTTOM_SPACE) - 30.0;

            if (smallCircles[i].x < targetX) {
                smallCircles[i].x += smallCircleSpeed;
            } else {
                // Reached the target position in the blue box, move to the top
                smallCircles[i].movingToBlueBox = false;
            }
        } else if (smallCircles[i].y < WINDOW_HEIGHT - VERTICAL_SPACE - 30.0) {
            // Move upwards along the border
            smallCircles[i].y += smallCircleSpeed;
        } else if (smallCircles[i].x > HORIZONTAL_SPACE + 30.0) {
            // Continue moving towards the left
            smallCircles[i].x -= smallCircleSpeed;
        } else if (smallCircles[i].y > WINDOW_HEIGHT - VERTICAL_SPACE - 50.0) {

            if(smallCircles[i].targetCashierID>0){
                // Move down by 50 pixels after reaching the top-left position
                smallCircles[i].movingToBlueBox = false;
                smallCircles[i].y -= 50.0;
                smallCircles[i].movingToBlueBox = false;
                smallCircleSpeed = 0;
            }
            else{
                smallCircles[i].movingToBlueBox = false;
            }
            
            
        } else {
            // Stay there
            smallCircles[i].movingToBlueBox = false;
        }
    }
}


void drawRectangle(float x1, float y1, float x2, float y2)
{
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void initializeCashiers()
{
    float cashierSpacing = 10.0;
    for (int i = 0; i < MAX_CASHIER; ++i)
    {
        cashiers[i].x1 = HORIZONTAL_SPACE + 12;
        cashiers[i].y1 = WINDOW_HEIGHT - VERTICAL_SPACE - 12 - i * (BOX_HEIGHT + BOX_SPACING + cashierSpacing);
        cashiers[i].x2 = HORIZONTAL_SPACE + 50;
        cashiers[i].y2 = WINDOW_HEIGHT - VERTICAL_SPACE - 50 - i * (BOX_HEIGHT + BOX_SPACING + cashierSpacing);
        cashiers[i].id = i + 1;
        drawRectangle(cashiers[i].x1, cashiers[i].y1, cashiers[i].x2, cashiers[i].y2);
    }
}

void displayCircles()
{
    drawSmallCircles();
}

void displayBoxes()
{
    glColor3f(1.0, 0.0, 0.0); // Red
    drawRectangle(0.8 * WINDOW_WIDTH - HORIZONTAL_SPACE, VERTICAL_SPACE,
                  WINDOW_WIDTH - HORIZONTAL_SPACE, WINDOW_HEIGHT - VERTICAL_SPACE);

    glColor3f(0.0, 0.0, 1.0); // Blue
    for (int i = 0; i < 5; ++i)
    {
        drawRectangle(0.8 * WINDOW_WIDTH - HORIZONTAL_SPACE,
                      WINDOW_HEIGHT - VERTICAL_SPACE - (i + 1) * (BOX_HEIGHT + BOX_SPACING + BOTTOM_SPACE),
                      WINDOW_WIDTH - HORIZONTAL_SPACE,
                      WINDOW_HEIGHT - VERTICAL_SPACE - i * (BOX_HEIGHT + BOX_SPACING + BOTTOM_SPACE));
    }

    glColor3f(0.0, 1.0, 0.0); // Green
    drawRectangle(HORIZONTAL_SPACE, VERTICAL_SPACE,
                  0.8 * WINDOW_WIDTH - 2 * HORIZONTAL_SPACE, WINDOW_HEIGHT - VERTICAL_SPACE);

    glColor3f(0.0, 0.0, 0.0); // Black
    initializeCashiers();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    displayCircles();
    displayBoxes();

    glFlush();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
}

void animate(int value)
{
    updateSmallCircles();
    glutPostRedisplay();
    glutTimerFunc(16, animate, 0); // 60 frames per second
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Moving Circles");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, animate, 0);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    while (1 == 1)
    {
        int cash;
        FILE *file = fopen("ogl.txt", "r"); // Open the file in read mode

        if (file != NULL)
        {
            fscanf(file, "%d", &cash); // Read the integer value from the file
            fclose(file);              // Close the file
        }
        float greenBoxCenterX = (0.8 * WINDOW_WIDTH - HORIZONTAL_SPACE - 30.0 + HORIZONTAL_SPACE + 30.0) / 2.0;

        for (int i = 0; i < MAX_CUS; ++i)
        {
            smallCircles[i].x = greenBoxCenterX + i * (smallCircleRadius * 2 + 5.0);
            smallCircles[i].y = VERTICAL_SPACE + smallCircleRadius;
            smallCircles[i].movingToBlueBox = true;
            smallCircles[i].id = i + 1; // Assign unique IDs to small circles

            // Set the target cashier ID for the second black circle (you can change the index as needed)
            if(i == 1){

            
                smallCircles[i].targetCashierID = cash; // Set the target cashier ID (1-indexed)
            }
        }

        glutMainLoop();
    }
    return 0;
}