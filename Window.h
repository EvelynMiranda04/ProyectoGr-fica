#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	// ====================================================================================
	bool getAccionF() { return accionF; }
	void apagarAccionF() { accionF = false; }

	bool getAccionG() { return accionG; }
	void apagarAccionG() { accionG = false; }

	bool getAccionH() { return accionH; }
	void apagarAccionH() { accionH = false; }

	bool getAccionJ() { return accionJ; }
	void apagarAccionJ() { accionJ = false; }

	bool* getStatusLucesSpot() { return statusLucesSpot; }
	int getContadorTeclaB() { return contadorTeclaB; }

	bool getAccionE() { return accionE; }
	void apagarAccionE() { accionE = false; }

	bool getAccionR() { return accionR; }
	void apagarAccionR() { accionR = false; }

	bool getAccionT() { return accionT; }
	void apagarAccionT() { accionT = false; }

	// ====================================================================================
	
	bool getShouldClose() { return  glfwWindowShouldClose(mainWindow); }
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	// ====================================================================================
	// Nuevas variables booleanas
	bool accionF;
	bool accionG;
	bool accionH;
	bool accionJ;
	bool statusLucesSpot[8];	// Estado de las 8 luces
	int contadorTeclaB;			// Animación 2 (Avance locomotoras)
	bool accionE;
	bool accionR;
	bool accionT;
	// ====================================================================================
	
	
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

