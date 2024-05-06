#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <GL/glut.h>

// Define a constant for PI
#define PI 3.14159265
// Forward declaration of shootBallsTimer function
void shootBallsTimer(int value);

// Structure to represent a ball
typedef struct {
    float posX;
    float posY;
    bool active;
    float velocityY;
    float velocityX;
} Ball;

// Structure to manage multiple rectangles
typedef struct {
    float horizontalOffset; // Offset for the horizontal movement
    float velocity;         // Speed and direction of the movement
    float startY;           // Vertical start position for the rectangle and boxes
} MovingRect;

// Structure to represent a small box
typedef struct {
    float posX;
    float posY;
    bool loaded;
} SmallBox;

// Global array of moving rectangles
MovingRect rects[2]; // Initialize two rectangles
float rectWidth = 0.3f;  // Total width of each rectangle

// Arrays to hold the positions of the dots
float dotX[20];
float dotY[20];
int dotNumbers[20];

// Define the larger black rectangle area
float blackRectMinX = -0.2f;
float blackRectMaxX = 0.9f;
float blackRectMinY = -0.2f;
float blackRectMaxY = -0.6f;

// Loading parameters
#define NUM_BOXES 6
bool boxLoaded[NUM_BOXES];
int loadingTime[NUM_BOXES];
int currentBoxIndex = 0;
bool loadingInProgress = false;

// Control variables for multiple cycles
int cyclesCompleted = 0;
int totalCycles = 3; // Number of cycles to complete

// Modify the structure definition of BlackBox to include width and height
typedef struct {
    float posX;
    float posY;
    float width;
    float height;
    bool active;
    int color; // Whether the box is active or not
    // Add more properties as needed
} BlackBox;
// Global array of smaller black boxes
BlackBox blackBoxes[15]; // Initialize three smaller black boxes
// Adjust the size of the black boxes to match the size of the dots
void initBlackBoxes() {
    // Define the dimensions of each black box to match the size of the small white boxes
    float boxWidth = 0.02f;
    float boxHeight = 0.02f;

    // Initialize properties for each black box inside the black rectangle
    for (int i = 0; i < 6; i++) {
        blackBoxes[i].width = boxWidth;
        blackBoxes[i].height = boxHeight;
        blackBoxes[i].active = true; // Activate the box
        blackBoxes[i].color = 0; // It's a black box

        // Draw Black Boxes : Spliting workers.
        blackBoxes[i].posX = blackRectMinX + (i + 0.5f) * (blackRectMaxX - blackRectMinX) / 12.0f - 0.3f * boxWidth; 
        blackBoxes[i].posY = blackRectMaxY - 0.8f * (blackRectMaxY - blackRectMinY) + 0.5f * boxHeight;
    }

    // Initialize properties for each purple box inside the black rectangle
    float purpleBoxStartX = blackRectMinX + (blackRectMaxX - blackRectMinX) - 1.5f * boxWidth; // Start X position inside the black rectangle
    float purpleBoxStartY = blackRectMinY + 0.5f * (blackRectMaxY - blackRectMinY) - 1.5f * boxHeight; // Start Y position inside the black rectangle

    for (int i = 0; i < 4; i++) {
        blackBoxes[i + 6].width = boxWidth;
        blackBoxes[i + 6].height = boxHeight;
        blackBoxes[i + 6].active = true; // Activate the box
        blackBoxes[i + 6].color = 1; // It's a purple box

        // Calculate the position of each purple box inside the black rectangle
        blackBoxes[i + 6].posX = purpleBoxStartX;
        blackBoxes[i + 6].posY = purpleBoxStartY + i * (boxHeight + 0.01f); // Vertical positioning with 0.02f spacing
    }

    // Initialize properties for each yellow box (Collecting workers)
    float yellowBoxStartX = blackRectMaxX + -1.2f; // Start X position for the yellow boxes
    float yellowBoxStartY = blackRectMinY + -0.3f; // Start Y position for the yellow boxes

    for (int i = 0; i < 5; i++) {
        blackBoxes[i + 10].width = boxWidth;
        blackBoxes[i + 10].height = boxHeight;
        blackBoxes[i + 10].active = true; // Activate the box
        blackBoxes[i + 10].color = 2; // It's a yellow box

        // Calculate the position of each yellow box vertically
        blackBoxes[i + 10].posX = yellowBoxStartX;
        blackBoxes[i + 10].posY = yellowBoxStartY + i * (boxHeight + 0.02f); // Vertical positioning with 0.02f spacing
    }
}



void drawBlackBoxes() {
    glColor3f(0.0f, 0.0f, 0.0f); // Black color
    for (int i = 0; i < 15; i++) {
        if (blackBoxes[i].active) {
            if (blackBoxes[i].color ==0) {
                glColor3f(0.0f, 0.0f, 0.0f); // Black color
            } else if(blackBoxes[i].color ==1){
                glColor3f(0.5f, 0.0f, 0.5f); // Purple color
            }else { glColor3f(1.0f, 1.0f, 0.0f); //yallow
            }

            glBegin(GL_QUADS);
            glVertex2f(blackBoxes[i].posX, blackBoxes[i].posY);
            glVertex2f(blackBoxes[i].posX + blackBoxes[i].width, blackBoxes[i].posY);
            glVertex2f(blackBoxes[i].posX + blackBoxes[i].width, blackBoxes[i].posY + blackBoxes[i].height);
            glVertex2f(blackBoxes[i].posX, blackBoxes[i].posY + blackBoxes[i].height);
            glEnd(); // Close the glBegin(GL_QUADS) call
        }
    }
}




// Global array of balls
Ball balls[6]; // Initialize six balls
// Draw the small boxes inside the white rectangle
/// Adjust the smallBoxes array initialization to match the number of loading slots
SmallBox smallBoxes[NUM_BOXES]; // Change to match the number of loading slots
void drawSmallBoxes() {
    glColor3f(1.0f, 1.0f, 1.0f); // White color
    for (int i = 0; i < NUM_BOXES; i++) {
        if (!smallBoxes[i].loaded) {
            glBegin(GL_QUADS);
            glVertex2f(smallBoxes[i].posX, smallBoxes[i].posY);
            glVertex2f(smallBoxes[i].posX + 0.02f, smallBoxes[i].posY);
            glVertex2f(smallBoxes[i].posX + 0.02f, smallBoxes[i].posY + 0.02f);
            glVertex2f(smallBoxes[i].posX, smallBoxes[i].posY + 0.02f);
            glEnd();
        }
    }
}


// Initialize the balls
void initBalls() {
    for (int i = 0; i < 6; i++) {
        balls[i].active = false;
    }
}

// Initialize dot numbers
void initDotNumbers() {
    // Initialize random numbers for each dot
    for (int i = 0; i < 20; i++) {
        dotNumbers[i] = rand() % 10 + 1; // Generate a random number between 1 and 10
    }
}


// Initialize loading parameters
void initLoadingParameters() {
    // Initialize loading parameters for each box
    for (int i = 0; i < NUM_BOXES; i++) {
        boxLoaded[i] = false;
        loadingTime[i] = rand() % 300 + 100; // Random loading time between 100 and 399 frames
    }
}

// Initialize small boxes positions
void initSmallBoxes() {
    // Define the dimensions of the white moving rectangle
    float rectMinX = -0.15f;
    float rectMaxX = 0.15f;
    float rectMinY = 0.5f;
    float rectMaxY = 0.7f;

    // Calculate the center of the white moving rectangle
    float centerX = (rectMinX + rectMaxX) / 2.0f;
    float centerY = (rectMinY + rectMaxY) / 2.0f;

    // Calculate the width and height of the white moving rectangle
    float rectWidth = rectMaxX - rectMinX;
    float rectHeight = rectMaxY - rectMinY;

    // Calculate the dimensions of each small box
    float boxSizeX = 0.02f;
    float boxSizeY = 0.02f;

    // Calculate the spacing between small boxes (reduced)
    float spaceX = (rectWidth - 3 * boxSizeX) / 4.0f; // Reduced spacing
    float spaceY = (rectHeight - 2 * boxSizeY) / 3.0f; // Reduced spacing

    // Initialize small boxes inside the white moving rectangle
    for (int i = 0; i < NUM_BOXES; i++) {
        // Calculate the position of each small box
        int row = i / 3;
        int col = i % 3;
        float posX = centerX - rectWidth / 2.0f + spaceX + col * (boxSizeX + spaceX);
        float posY = centerY + rectHeight / 2.0f - spaceY - row * (boxSizeY + spaceY);
        smallBoxes[i].posX = posX;
        smallBoxes[i].posY = posY;
        smallBoxes[i].loaded = false;
    }
}

void initGL() {
    glEnable(GL_DEPTH_TEST); // Enable depth testing for z-culling
    srand(time(NULL)); // Initialize random seed

    // Initialize properties for the moving rectangles
    rects[0].horizontalOffset = 0.9f;  // Start from the right edge
    rects[0].velocity = -0.01f;         // Move towards the left
    rects[0].startY = -0.2f;            // Initial vertical position

    rects[1].horizontalOffset = -0.9f; // Start from the left edge
    rects[1].velocity = 0.01f;         // Move towards the right
    rects[1].startY = -0.5f;           // Initial vertical position

    // Initialize dot positions
    float boxStartX = 0.2f; // Start X position of the box
    float boxStartY = -0.95f; // Start Y position of the box
    float boxSize = 0.4f;   // Size of the box

    int index = 0;
    // Calculate positions for dots within the box
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 5; col++) {
            dotX[index] = boxStartX + col * (boxSize / 2.1f);
            dotY[index] = boxStartY + row * (boxSize / 6.0f);
            index++;
        }
    }
    initBlackBoxes();
    // Initialize random numbers for each dot
    initDotNumbers();

    // Initialize loading parameters
    initLoadingParameters();

    // Initialize small boxes positions
    initSmallBoxes();

    // Initialize balls
    initBalls();
   
    // Start the timer to shoot balls randomly
    int initialDelay = rand() % 3000 + 1000;  // Random initial delay between 1 and 4 seconds
    glutTimerFunc(initialDelay, shootBallsTimer, 0);
}


// Draw the balls
void drawBalls() {
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for balls
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 6; i++) {
        if (balls[i].active) {
            glVertex2f(balls[i].posX, balls[i].posY);
        }
    }
    glEnd();
}



// Update the position of the balls
void updateBalls() {
    for (int i = 0; i < 6; i++) {
        if (balls[i].active) {
            balls[i].posY += balls[i].velocityY; // Move the ball upwards
            balls[i].posX += balls[i].velocityX; // Move the ball upwards

            // Check if the ball reaches the top of the screen
            if (balls[i].posY >= 1.6f) { 
                balls[i].active = false; // Deactivate the ball
            }
        }
    }
}

// Implement an empty area like a circle in the left-bottom small
void drawEmptyArea() {
    // YELLOW
    glColor3f(1.0f, 1.0f, 0.5f); // Yellow color for the empty area
    
    // Define the center position for the circle
    float centerX = -0.65f;
    float centerY = -0.65f;
    float radius = 0.35f;

    // Draw the filled circle
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float angle = i * 2.0f * PI / 360.0f;
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

    
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    // Draw the bottom half in darker green
    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.4f, 0.0f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(-1.0f, 0.0f);
    glEnd();

    // Draw the top half in blue
    glBegin(GL_QUADS);
    glColor3f(0.529f, 0.808f, 0.922f);
    glVertex2f(-1.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();


    // Draw three strikes on the right between the green and blue halves
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for strikes

    // First strike
    glBegin(GL_LINES);
    glVertex2f(0.8f, -0.1f); // Start point of the first strike
    glVertex2f(0.8f, 0.1f);  // End point of the first strike
    glEnd();

    // Second strike
    glBegin(GL_LINES);
    glVertex2f(0.7f, -0.1f); // Start point of the second strike
    glVertex2f(0.7f, 0.1f);  // End point of the second strike
    glEnd();

    // Third strike
    glBegin(GL_LINES);
    glVertex2f(0.6f, -0.1f); // Start point of the third strike
    glVertex2f(0.6f, 0.1f);  // End point of the third strike
    glEnd();

    // Draw a larger black empty rectangle
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(blackRectMinX, blackRectMinY);
    glVertex2f(blackRectMaxX, blackRectMinY);
    glVertex2f(blackRectMaxX, blackRectMaxY);
    glVertex2f(blackRectMinX, blackRectMaxY);
    glEnd();
    // Draw the smaller black boxes
    drawBlackBoxes();

   


    // Draw a text in the corner of the black empty rectangle say "Storage area"
    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(0.7f, -0.3f);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'S');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 't');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'o');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'r');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'a');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'g');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'e');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'a');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'r');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'e');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'a');


    // Draw the the empty rectangle
    drawEmptyArea();


    // Draw blue dots inside the black rectangle
    for (int i = 0; i < 20; i++) {
        if (dotNumbers[i] > 0) {
            // Set color to blue if the dot number is greater than 0
            glColor3f(0.0f, 0.0f, 1.0f);
        } else {
            // Set color to red if the dot number is 0
            glColor3f(1.0f, 0.0f, 0.0f);
        }
        glPointSize(7.0f);
        glBegin(GL_POINTS);
        glVertex2f(dotX[i], dotY[i]);
        glEnd();

        // Draw numbers above dots
        glColor3f(0.0f, 0.0f, 0.0f); // Set color to black for numbers
        glRasterPos2f(dotX[i] - 0.015f, dotY[i] + 0.02f); // Position above the dot
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, '0' + dotNumbers[i]); // Render the random number
    }

    // Draw rectangles and boxes
    for (int i = 0; i < 1; i++) {
        glLoadIdentity();
        glTranslatef(rects[i].horizontalOffset, rects[i].startY, 0.0f);

        // Draw the moving empty rectangle
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(5.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-0.15f, 0.5f);
        glVertex2f(0.15f, 0.5f);
        glVertex2f(0.15f, 0.7f);
        glVertex2f(-0.15f, 0.7f);
        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
       
        // Draw small boxes
        drawSmallBoxes();
    }

    // Draw balls
    drawBalls();
    



    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

// Define states for the rectangle movement
typedef enum {
    MOVING_LEFT,
    LOADING,
    MOVING_RIGHT,
    FILLING
} RectangleState;

// Define the current state of the rectangle movement
RectangleState currentState = MOVING_LEFT;

void update(int value) {
    // Update the position of each rectangle 

    for (int i = 0; i < 2; i++) {
        if (rects[i].horizontalOffset >= -0.8f && rects[i].horizontalOffset <= -0.4f && rects[i].startY <= -0.2f) {
            rects[i].velocity = 0.0f;
            // Start loading animation for the current rectangle
            if (!boxLoaded[currentBoxIndex]) {
                loadingTime[currentBoxIndex]--;
                if (loadingTime[currentBoxIndex] <= 0) {
                    // Finish loading current box and move to the next one
                    boxLoaded[currentBoxIndex] = true;
                    currentBoxIndex++;
                }
            }
        } else {
            // Continue movement if the rectangle has not reached the black rectangle
            rects[i].horizontalOffset += rects[i].velocity;

            // Check if the rectangle has moved back to its initial position
            if (rects[i].horizontalOffset >= 0.9f || rects[i].horizontalOffset <= -0.9f) {
                // Reset loading parameters and start moving the rectangle again
                initLoadingParameters();
                if (i == 0) {
                    rects[i].velocity = -0.01f;
                } else {
                    rects[i].velocity = 0.01f;
                }
            }
        }
    }

// Update the loading animation for each box
for (int j = 0; j < NUM_BOXES; j++) {
    if (boxLoaded[j]) {
        // Move the loaded box off the white rectangle with spacing between loaded boxes
        float spacing = 0.02f; // Adjust this value to control the spacing between loaded boxes
        float landingPosition = blackRectMaxY + spacing + j * (0.003f + spacing) - 0.0005f; // Adjust the landing position
        
        if (smallBoxes[j].posY > landingPosition) {
            // If the box is above its designated landing position, move it downward until it reaches that position
            smallBoxes[j].posY -= 0.02f;
        }
    }
}


    // Decrement dot numbers every 5 seconds until they reach 0
    static int counter = 0;
    if (counter % 300 == 0) { // 300 frames at 60 fps = 5 seconds
        for (int i = 0; i < 20; i++) {
            if (dotNumbers[i] > 0) {
                dotNumbers[i]--;
            }
        }
    }
    counter++;
    // Update balls
    updateBalls();

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);  // Re-register the timer callback
}


void shootBallRandom() {
    // Generate random angle between 0 and 2*pi
    float angle = ((float)(rand() % 628)) / 100.0f; // Dividing by 100 to convert to radians

    // Generate random initial velocity magnitude between 0.02 and 0.04
    float initialVelocity = ((float)(rand() % 200) + 200.0f) / 10000.0f; // Random value between 0.02 and 0.04

    // Calculate initial velocity components
    float velocityX = initialVelocity * cos(angle);
    float velocityY = initialVelocity * sin(angle);

    // Generate a random index within the range of strikes
    int strikeIndex = rand() % 3;

    // Define the X position based on the selected strike
    float posX = 1.7f - strikeIndex * 0.1f; // Random position between 0.6 and 0.8

    // Define the Y position at the head of the selected strike
    float posY = 0.7f;

    // Find an inactive ball and set its position and velocity
    for (int i = 0; i < 6; i++) {
        if (!balls[i].active) {
            balls[i].posX = posX;
            balls[i].posY = posY;
            balls[i].active = true;
            balls[i].velocityY = velocityY; // Set the Y component of velocity
            balls[i].velocityX = velocityX; // Set the X component of velocity
            break; // Exit the loop after shooting one ball
        }
    }
}



// Timer function to shoot balls randomly at regular intervals
void shootBallsTimer(int value) {
    // Shoot a ball randomly
    shootBallRandom();

    // Set the timer to call this function again after a random delay
    int delay = rand() % 3000 + 1000;  // Random delay between 1 and 4 seconds
    glutTimerFunc(delay, shootBallsTimer, 0);
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1300, 500);
    glutCreateWindow("OpenGL Test");
    initGL();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);  // Initial registration of the timer function
    glutMainLoop();
    return 0;
}