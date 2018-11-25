#include <bits/stdc++.h>
#include <GL/glut.h>
using namespace std;

void display(void);
void polygon(int a, int b, int c , int d);
void DrawCube();
void printtext(int x, int y, string String);
 
int WindowHeight = 1000;
int WindowWidth = 1000;
void Printer(){
  int count=0;
  string ch;
  cin>>ch;
  while(ch!=48){
      cin>>ch;
      sprintf(string,ch);
      printtext(100,700+count,string);
  }
} 
void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27)
		exit(0);
    else if(key==32){
        Printer();
    }
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitWindowPosition(0, 0);
 
	glutCreateWindow("Metro Works");
 
	glutDisplayFunc(display);
 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 1, 1, 100);
 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
 
	gluLookAt(2, 2, 10, 2, 0, 0, 0, 1, 0);
 
	glutMainLoop();
	return 0;
}
 
 
void printtext(int x, int y, string String)
{
//(x,y) is from the bottom left of the window
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, WindowWidth, 0, WindowHeight, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glPushAttrib(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(x,y);
    for (int i=0; i<String.size(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String[i]);
    }
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
 
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
	glEnable(GL_DEPTH_TEST);
 
    char string[64];
    sprintf(string, "Metro Works");
    printtext(400,980,string);

    sprintf(string, "Enter the Source Station:-");
    printtext(100,750,string);

    sprintf(string, "Enter the Destination Station:-");
    printtext(100,650,string);


    glutKeyboardFunc(processNormalKeys);
 
	glutSwapBuffers();
}
