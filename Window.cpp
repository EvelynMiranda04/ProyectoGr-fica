#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
	mouseFirstMoved = true;
	// ====================================================================================
	// Inicialización nuevas banderas
	accionF = false;
	accionG = false;
	accionH = false;
	accionJ = false;
	for (int i = 0; i < 8; i++) { statusLucesSpot[i] = false; } // Todas inician apagadas
	contadorTeclaB = 0;
	// ====================================================================================
	
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	mouseFirstMoved = true;

	xChange = 0.0f;
	yChange = 0.0f;
	lastX = 0.0f;
	lastY = 0.0f;

	// ====================================================================================
	// Inicialización nuevas banderas
	accionF = false;
	accionG = false;
	accionH = false;
	accionJ = false;
	for (int i = 0; i < 8; i++) { statusLucesSpot[i] = false; } // Todas inician apagadas
	contadorTeclaB = 0;
	// ====================================================================================
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}


int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "PROYECTO FINAL COMPUTACION GRAFICA :3", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}



void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	// ====================================================================================
	// 1. CONTROLES DEL SISTEMA Y VENTANA
	// ====================================================================================
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// ====================================================================================
	// 2. CONTROLES DE CAMARA
	// ====================================================================================
	// Captura de banderas tipo "Trigger" (un solo pulso)
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_F) theWindow->accionF = true;
		if (key == GLFW_KEY_G) theWindow->accionG = true;
		if (key == GLFW_KEY_H) theWindow->accionH = true;
		if (key == GLFW_KEY_J) theWindow->accionJ = true;
	}

	// ====================================================================================
	// 3. ON/OFF DE LUCES SPOT (Teclas 1-8)
	// ====================================================================================
	// Teclas del 1 al 8 para controlar Luces Spot
	if (key >= GLFW_KEY_1 && key <= GLFW_KEY_8 && action == GLFW_PRESS) {
		int indiceLuz = key - GLFW_KEY_1; // Convierte la tecla (1-8) a índice (0-7)
		theWindow->statusLucesSpot[indiceLuz] = !theWindow->statusLucesSpot[indiceLuz];
	}

	// ====================================================================================
	// 4. CONTROLES DE ANIMACIONES (ESFERA Y DISPARO)
	// ====================================================================================

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_E) theWindow->accionE = true;
		if (key == GLFW_KEY_R) theWindow->accionR = true;
		if (key == GLFW_KEY_T) theWindow->accionT = true;
	}

	// ====================================================================================
	// 5. ANIMACION LOCOMOTORAS (Tecla B)
	// ====================================================================================
	// [B]: Gatillo para la máquina de estados de los trenes (Ciclo 0-1-2-3-4)
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		theWindow->contadorTeclaB++;

		if (theWindow->contadorTeclaB > 4) {
			theWindow->contadorTeclaB = 0; // Reinicio automático al llegar al final
		}

		printf("Estas visualizando la fase %d de la animación de la locomotora\n", theWindow->contadorTeclaB);
	}

	// ====================================================================================
	// 7. REGISTRO DE ESTADO GENERAL (Cámara y teclas de pulsación continua)
	// ====================================================================================
	if (key >= 0 && key < 1024)
	{
		// Si se presiona, registramos como verdadero en el arreglo
		if (action == GLFW_PRESS) {
			theWindow->keys[key] = true;
		}
		// Si se suelta, registramos como falso
		else if (action == GLFW_RELEASE) {
			theWindow->keys[key] = false;
		}
	}
}



void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
