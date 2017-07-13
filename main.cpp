#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <parser.h>

#define TITLE "Te3DPlot"
#define PATH_TO_SETTINGS_FILE "settings.dat"
#define SCALE 1.1

typedef enum
{
    WINDOW_WIDTH = 500,
    WINDOW_HEIGHT = 500,
    WINDOW_X = 50,
    WINDOW_Y = 50,
    TIMER_DELAY = 10,
    ANGLE_STEP = 5,
    AXSIS_STEP  = 5
} Constants;

std::string input;
std::vector<std::pair<char,double>> vars;

struct Point
{
    double x, y, z;
};

double Xmin_l, Xmax_l, dX_l, Ymin_l, Ymax_l, dY_l;
std::vector<std::vector<Point>> points_l;
double Ymin_t, Ymax_t, dY_t, Xmin_t, Xmax_t, dX_t;
std::vector<std::vector<Point>> points_t;
double left, right, bottom, top, near_val, far_val;
float red, green, blue;
float background_red, background_green, background_blue;

void timer(int i = 0);
void display();
void drawWord(const std::string &word, float x_, float y_, float space, int red, int green,
              int blue);
void loadSettings(std::string pathToFile);
void reloadAllSettings();
void calculatePoints();
double f(double x, double y);
void mySpecialKeyboardFunction(int key, int x, int y);
void myKeyboardFunction(unsigned char key, int x, int y);


int main(int argc, char *argv[])
{
    loadSettings(PATH_TO_SETTINGS_FILE);
    calculatePoints();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(WINDOW_X, WINDOW_Y);
    glutCreateWindow(TITLE);
    glClearColor(background_red, background_green, background_blue,1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(left, right, bottom, top, near_val, far_val);
    //glOrtho(-500, 500, -500, 500, -500, 500);
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0, 0, -1000);
    glutDisplayFunc(display);
    glutSpecialFunc(mySpecialKeyboardFunction);
    glutKeyboardFunc(myKeyboardFunction);
    //glutIdleFunc(idleFunc);
    //glRotatef(-30, 1,0,0);
    timer();
    glutMainLoop();
    return 0;
}

void timer(int i)
{
    display();
    glutTimerFunc(TIMER_DELAY, timer, 0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(red,green,blue);

    //Vertical lines

    for(const auto &line: points_l)
    {
        glBegin(GL_LINE_STRIP);
        for(const auto &p: line)
        {
            glVertex3d(p.x, p.y, p.z);
        }
        glEnd();
    }
    //Horizontal lines

    for(const auto &line: points_t)
    {
        glBegin(GL_LINE_STRIP);
        for(const auto &p: line)
        {
            glVertex3d(p.x, p.y, p.z);
        }
        glEnd();
    }

    //Draw axes

    glBegin(GL_LINES);

        //X-axis

        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-700, 0, 0);
        glVertex3f(700, 0, 0);

        //Y-axis

        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0, -700, 0);
        glVertex3f(0, +700, 0);

        //Z-axis

        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0, 0, -500);
        glVertex3f(0, 0, +500);
    glEnd();

        //Draw axis labels

        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos3f(730.0f, 0, 10);
        char ch = 'X';
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)ch);
        glColor3f(0.0f, 1.0f, 0.0f);
        glRasterPos3f(0, 730.0f, 0);
        ch = 'Y';
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)ch);
        glColor3f(0.0f, 0.0f, 1.0f);
        glRasterPos3f(0,0,530.0f);
        ch = 'Z';
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)ch);

        //drawWord(input, 50, 50, 50, 255, 255, 0);

    glutSwapBuffers();
}

void drawWord(const std::string &word, float x_, float y_, float space, int red, int green,
              int blue)
{
    if(red < 0) red = 0;
    if(red > 255) red = 255;
    if(green < 0) green = 0;
    if(green > 255) green = 255;
    if(blue < 0) blue = 0;
    if(blue > 255) blue = 255;

    glColor3f(red / 255.f, green / 255.f, blue / 255.f);
    for(unsigned int i = 0; i < word.size(); ++i)
    {
        glRasterPos2f(x_ + i * space, y_);
        char ch = word[i];
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)ch);
    }
}

void loadSettings(std::string pathToFile)
{
    enum LoadState {
        EXPR,
        LENGTHWISE_LINES,
        TRANSVERSAL_LINES,
        FRUSTUM,
        SURFACE_COLOR,
        BACKGROUND_COLOR,
        STOP
    };
    LoadState ls = STOP;
    std::ifstream fi(pathToFile);
    std::stringstream ss;
    if(fi.is_open())
    {
        while(!fi.eof())
        {
            std::string line;
            std::getline(fi, line);
            if(line.empty()) continue;
            if(line == "[Expression]")
            {
                ls = EXPR;
            }
            else if(line == "[Lengthwise lines Xmin Xmax dX Ymin Ymax dY]")
            {
                ls = LENGTHWISE_LINES;
            }
            else if(line == "[Transversal lines Ymin Ymax dY Xmin Xmax dX]")
            {
                ls = TRANSVERSAL_LINES;
            }
            else if(line == "[Frustum left right bottom top near_val far_val]")
            {
                ls = FRUSTUM;
            }
            else if(line == "[Surface color RGB]")
            {
                ls = SURFACE_COLOR;
            }
            else if(line == "[Background color RGB]")
            {
                ls = BACKGROUND_COLOR;
            }
            else
            {
                ls = STOP;
            }
            std::getline(fi,line);
            switch(ls)
            {
                case EXPR:
                    input = line;
                    break;
                case LENGTHWISE_LINES:
                    ss.clear();
                    ss << line;
                    ss >> Xmin_l >> Xmax_l >> dX_l >> Ymin_l >> Ymax_l >> dY_l;
                    break;
                case TRANSVERSAL_LINES:
                    ss.clear();
                    ss << line;
                    ss >> Ymin_t >> Ymax_t >> dY_t >> Xmin_t >> Xmax_t >> dX_t;
                    break;
                case FRUSTUM:
                    ss << line;
                    ss >> left >> right >> bottom >> top >> near_val >> far_val;
                    break;
                case SURFACE_COLOR:
                    ss.clear();
                    ss << line;
                    ss >> red >> green >> blue;
                    red /= 255.f;
                    green /= 255.f;
                    blue /= 255.f;
                    break;
                case BACKGROUND_COLOR:
                    ss.clear();
                    ss << line;
                    ss >> background_red >> background_green >> background_blue;
                    background_red /= 255.f;
                    background_green /= 255.f;
                    background_blue /= 255.f;
                    break;
                default:
                    break;
            }
            if(ls == STOP) break;
        }
        fi.close();
    }
    else
    {
        std::cout << "Could not open the file!" << std::endl;
    }

}

void calculatePoints()
{
    //Lengthwise lines

    points_l.clear();
    for(double x = Xmin_l; x < Xmax_l; x += dX_l)
    {
        std::vector<Point> tmp;
        for(double y = Ymin_l; y < Ymax_l; y += dY_l)
        {
            Point p{x, y, f(x,y)};
            tmp.push_back(p);
        }
        points_l.push_back(tmp);
    }

    //Transversal lines

    points_t.clear();
    for(double y = Ymin_t; y < Ymax_t; y += dY_t)
    {
        std::vector<Point> tmp;
        for(double x = Xmin_t; x < Xmax_t; x += dX_t)
        {
            Point p{x, y, f(x,y)};
            tmp.push_back(p);
        }
        points_t.push_back(tmp);
    }
}

double f(double x, double y)
{
    std::pair<char, double> p_x('X', x);
    std::pair<char, double> p_y('Y', y);
    vars.clear();
    vars.push_back(p_x);
    vars.push_back(p_y);
    Parser p(input,vars);
    return p.calculateExpression();
}

void mySpecialKeyboardFunction(int key, int x, int y)
{
    /* if(glutGetModifiers() == GLUT_ACTIVE_CTRL)
    {
        glRotatef(ANGLE_STEP, 1,0,0);
    }*/
    switch(key)
    {
        case GLUT_KEY_UP :
            glTranslatef(0, +AXSIS_STEP, 0);
            break;
        case GLUT_KEY_DOWN:
            glTranslatef(0, -AXSIS_STEP, 0);
            break;
        case GLUT_KEY_LEFT :
            glTranslatef(-AXSIS_STEP, 0, 0);
            break;
        case GLUT_KEY_RIGHT:
            glTranslatef(+AXSIS_STEP, 0, 0);
            break;
        case GLUT_KEY_PAGE_UP:
            glTranslatef(0, 0, +AXSIS_STEP);
            break;
        case GLUT_KEY_PAGE_DOWN:
            glTranslatef(0, 0, -AXSIS_STEP);
            break;

    }
}

void myKeyboardFunction(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'x':
            glRotatef(+ANGLE_STEP, 1,0,0);
            break;
        case 'X':
            glRotatef(-ANGLE_STEP, 1,0,0);
            break;
        case 'y' :
            glRotatef(+ANGLE_STEP, 0,1,0);
            break;
        case 'Y' :
            glRotatef(-ANGLE_STEP, 0,1,0);
            break;
        case 'z' :
            glRotatef(ANGLE_STEP, 0,0,1);
            break;
        case 'Z' :
            glRotatef(-ANGLE_STEP, 0,0,1);
            break;
        case '+' :
            glScalef(SCALE,SCALE,SCALE);
            break;
        case '-' :
            glScalef(1/ SCALE, 1 / SCALE, 1 / SCALE);
            break;
        case 'l' :
            reloadAllSettings();
            break;
    }
}

void reloadAllSettings()
{
    loadSettings(PATH_TO_SETTINGS_FILE);
    calculatePoints();
    glClearColor(background_red, background_green, background_blue,1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(left, right, bottom, top, near_val, far_val);
    glMatrixMode(GL_MODELVIEW);
}
