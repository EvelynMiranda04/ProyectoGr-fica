// ====================================================================================
// PROYECTO FINAL - MOTOR GRÁFICO BASE
// ====================================================================================

// ====================================================================================
// 1. LIBRERÍAS Y DEPENDENCIAS
// Incluimos STB para cargar imágenes, GLM para matemáticas de matrices y vectores,
// y las librerías estándar de OpenGL (GLEW/GLFW).
// ====================================================================================
#define STB_IMAGE_IMPLEMENTATION
#include <cmath>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//Sonido
#include <irrKlang.h>
using namespace irrklang;
ISoundEngine* SoundEngine = createIrrKlangDevice();


// ====================================================================================
// 2. CLASES DEL MOTOR
// Cabeceras propias que encapsulan la complejidad de OpenGL.
// ====================================================================================
#include "Window.h"
#include "Camera.h"
#include "Shader_light.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"   
#include "Skybox.h" 
#include "Sphere.h"
#include "Model.h"
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

const float toRadians = 3.14159265f / 180.0f;

// ====================================================================================
// 3. VARIABLES DE ANIMACIÓN Y CONTROL GLOBAL
// Variables que se actualizarán cada frame en el Game Loop (Offsets, rotaciones, etc).
// ====================================================================================

// Control de Tiempo (DeltaTime) para animaciones fluidas independientes de los FPS
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// ==========================================
// 3.1 - VARIABLES AVATAR (Juan Pablo)
// ==========================================
float tiempoAnimacion = 0.0f;
float velocidadAnimacion = 0.15f;
float rotacionAvatar = 0.0f;
float amplitudArticulacion = 35.0f;
float distanciaRecorrida = 0.0f;
float cuerpoPosX = 120.0f;
float cuerpoPosZ = -30.0f;
float cuerpoRotY = 0.0f; // Empezamos en 0 (piezas 180 interno)
bool estaMoviendo = true;
float cronometroEspera = 0.0f;
float velocidadTrayecto = 0.10f;
float rotacionGlobal;
float radRot;
bool isMoving;
float velocity;
float deltaX;
float deltaY;
glm::vec3 posAvatar;
glm::vec3 forwardAvatar;
glm::vec3 up;
glm::vec3 rightAvatar;
glm::mat4 viewMatrix;
glm::vec3 posCamara;
glm::vec3 targetCamara;

// ==========================================
// 3.2 - VARIABLES CICLO DYN (Evelyn)
// ==========================================
float temporizadorSkybox = 0.0f;		// Reloj maestro del cielo
int indiceSkyboxActual = 0;				// Textura actual del cielo [0-7]
float duracionEtapaSkybox = 250.0f;		// VELOCIDAD DEL CICLO DÍA-NOCHE
unsigned int lucesPuntualesActivas;		// Luminarias de calle a encender

// ==========================================
// 3.3 - MÁQUINA DE ESTADOS An2: (Evelyn)
// ==========================================
// --- TREN CHICO ---
int subEtapaTrenChico = 1;				// [1-5]
float temporizadorTrenChico = 0.0f;		// Reloj individual tren ch
float distanciaTrenChico = 0.0f;		// Progreso q llevamos en las vías1
float trenChicoPosY = 0.0f;				// Altura inicial tren 1
float trenChicoPosZ = 7.26f;			// Vía Z inicial tren 1
float trenChicoRotY = 0.0f;				// Rotación inicial tren 1
// --- TREN LARGO ---
int subEtapaTrenLargo = 6;				// [6-10]
float temporizadorTrenLargo = 0.0f;		// Reloj individual tren lr
float distanciaTrenLargo = 0.0f;		// Progreso q llevamos en las vías2
int gatilloB;							// Contador de presiones tecla B
float progreso;							// Porcentaje de avance (0.0 a 1.0)
float cx, cz, cRotY;					// Coord temporales tren chico
float lx, lz, lRotY;					// Coord temporales tren largo
float posX, posZ, rotY;					// Coord temporales para vagones

// ==========================================
// 3.4 - MÁQUINA DE ESTADOS An5: (Evelyn)
// ==========================================
// --- HUMO: ANIMACIÓN CÍCLICA ---
float temporizadorCicloHumo = 0.0f;		// 200.0f unidades por ciclo
float offsetYHumo = 0.0f;				// -3Y o +3Y
float escalaHumoAnimada = 0.0f;			// Escala final después de cálculos
glm::mat4 matrizHumoChico = glm::mat4(1.0f);	// Guardado de posición de la cabina 
glm::mat4 matrizHumoLargo = glm::mat4(1.0f);	// Guardado de posición de la cabina 
float pSuave;							// Curva de suavizado (Ease-in/out)
float escalaBase;						// Tamaño base de humo sin el latido
float offsetY;							// Desplazamiento temporal en Y
float offsetZ;							// Desplazamiento temporal en Z

// ==========================================
// 3.5 - Luces Spot: (Evelyn)
// ==========================================
glm::mat4 trenChicoMat;					// Matriz ancla faro chico
glm::vec3 posFaroChico;					// Coordenada 3D faro chico
glm::vec3 dirFaroChico;					// Vector de dirección faro chico
glm::mat4 trenLargoMat;					// Matriz ancla faro largo
glm::vec3 posFaroLargo;					// Coordenada 3D faro largo
glm::vec3 dirFaroLargo;					// Vector de dirección faro largo
int i;									// Iterador estándar de bucles
unsigned int contadorSpotActivas;		// Total de faros/edificios a renderizar


// ==========================================
// 3.6 - RELOJ ANIMACION BASICA: (Tony)
// ==========================================
float rotAutoGearMinuto = 0.0f;
float rotAutoGearHora = 0.0f;
int   autoMinuteCount = 0;
float autoStepTimer = 0.0f;
float autoStepInterval = 60.0f; // 1 segundo por paso (ajustable)
// Engranes decorativos del segundo reloj
float autoGearBackRotation = 0.0f;
float autoGearRotSpeed = 1.0f; // Velocidad de giro ajustable

// ==========================================
// 3.7 - RELOJ ANIMACION POR KEYFRAMES: (Tony)
// ==========================================      
	// 13 keyframes = 12 segmentos de 30° (una vuelta completa + cierre).
const int N_KF = 13;
// --- Tablas de keyframes (rotación en Z, en grados) ----------------
float kf_Gear1[N_KF] = { 0,  30,  60,  90, 120, 150, 180, 210, 240, 270, 300, 330, 360 };
float kf_Gear2[N_KF] = { 0, -30, -60, -90,-120,-150,-180,-210,-240,-270,-300,-330,-360 };
float kf_Gear3[N_KF] = { 0,  30,  60,  90, 120, 150, 180, 210, 240, 270, 300, 330, 360 };
float kf_GearMin[N_KF] = { 0, -30, -60, -90,-120,-150,-180,-210,-240,-270,-300,-330,-360 };
float kf_GearHor[N_KF] = { 0, -2.5f, -5.0f, -7.5f, -10.0f, -12.5f,
						  -15.0f,-17.5f,-20.0f,-22.5f,-25.0f,-27.5f, -30.0f };
float rotGear1 = 0.0f, rotGear2 = 0.0f, rotGear3 = 0.0f;
float rotGearMin = 0.0f, rotGearHor = 0.0f;
float incGear1 = 0.0f, incGear2 = 0.0f, incGear3 = 0.0f;
float incGearMin = 0.0f, incGearHor = 0.0f;
// --- Control de pasos 
int   curr_step = 0;     // paso actual dentro del segmento
int   max_steps = 60;    // frames por segmento (60 aprox 1 s a 60 fps)
int   kf_idx = 0;     // índice del keyframe en el que vamos
bool  firstKF = true;  // bandera para inicializar incrementos


// ==========================================
// 3.8 - CUCKOO ANIMACION POR KEYFRAMES: (Tony)
// ==========================================
// 5 keyframes: péndulo va al centro -> izq -> centro -> der -> centro (un ciclo)
const int N_KF_CK = 5;
float ck_kf_pendulo[N_KF_CK] = { 0.0f, -20.0f, 0.0f, 20.0f, 0.0f };
float ck_kf_pajaro[N_KF_CK] = { 0.0f, -15.0f, 0.0f, -15.0f, 0.0f };
float ck_kf_cad3[N_KF_CK] = { 0.0f,  0.3f,  0.0f, -0.3f,  0.0f };
float ck_kf_cad4[N_KF_CK] = { 0.0f, -0.3f,  0.0f,  0.3f,  0.0f };
// Variables de estado del cuckoo
float ck_rotPendulo = 0.0f;
float ck_rotPajaro = 0.0f;
float ck_trasCad3 = 0.0f;
float ck_trasCad4 = 0.0f;
float ck_incPendulo = 0.0f, ck_incPajaro = 0.0f;
float ck_incCad3 = 0.0f, ck_incCad4 = 0.0f;
int  ck_curr_step = 0;
int  ck_max_steps = 40;   // frames por segmento (0.67 s a 60 fps, ajustable)
int  ck_kf_idx = 0;
bool ck_firstKF = true;

// ==========================================
// 3.9 - CHOQUE ESFERA ANIMACIÓN COMPLEJA (Rubén)
// ==========================================

float movEsferaX = 0.0f;
float movEsferaZ = 0.0f;
float rotEsferaY = 0.0f;
float rotEsferaZ = 0.0f;
float velocidadAvance = 0.2f;
float puntoDestinoX = 435.0f;
float anguloCaida = 80.0f;
float rotArbol = 0.0f;
float velocidadCaida;
bool arbol_cayendo = false;


// ==========================================
// 3.10 - DISPARO LATA (Rubén)
// ==========================================
// VARIABLES DE ESTADO (Cambian continuamente en la animación)
// ==========================================================
bool disparoActivo = false;
float tiempoSalto = 0.0f;
float lataPosZ = 0.0f;
float lataPosY = 0.0f;
float rotLata = 0.0f;
int rebotesContados = 0;
float alturaBaseActual = 4.8f;
float velocidadSaltoActual = 10.0f;
//  CONSTANTES DE CONFIGURACIÓN (Ajustes fijos del mundo)
float VEL_SALTO_ORIGINAL = 10.0f;
float ALTURA_ORIGINAL = 4.8f;
float velocidadHorizontalBase = 5.0f;
// VARIABLES AUXILIARES DE CÁLCULO
float factorGiro = 0.0f;
float semillaAleatoria = 0.0f;
float factorRestitucion = 0.0f;
float dispersionGiro = 0.0f;

// ==========================================
// 3.11 - POROS SALTANDO (Evelyn)
// ==========================================
// TInicio desfasados
float temporizadoresPoros[5] = { 0.0f, 72.0f, 144.0f, 36.0f, 90.0f };
float alturasPoros[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float duracionSubidaPoro = 40.0f;	// Subida rápida
float duracionBajadaPoro = 60.0f;	// Bajada lenta
float duracionTotalPoro = 100.0f;	// Ciclo total
float alturaMaximaPoro = 1.5f;		// Altura máxima en Y
// Variables temporales de trabajo
int pIter;
float progresoPoro;

// ==========================================
// 3.12 - CAMBIO DE HOJA (Evelyn)
// ==========================================
float rotacionPagina = 0.0f;
float velocidadPagina = 01.0f; // Velocidad del pase de página (grados por segundo)

// ==========================================
// 3.13 - ESTO ES LITERATURA (Evelyn)
// ==========================================
float temporizadorLiteratura = 0.0f;
float litPosZ = -36.35f;
float litEscalaX = 0.0f;
float litEscalaY = 0.0f;
float litEscalaZ = 0.0f;
// Tiempos (La etapa 2 dura exactamente el doble)
float litDuracionE1 = 100.0f;	// Nace y avanza
float litDuracionE2 = 200.0f;	// Se mantiene y avanza
float litDuracionE3 = 100.0f;	// Se encoge y desaparece
float litDuracionTotal = 400.0f;// 100 + 200 + 100
float progresoLit;


// ==========================================
//  3.14 - SONIDO (Rubén)
// ==========================================
//  3.14.1 - APUNTADORES PARA LOS TRACKS DE AUDIO
ISound* musicaFondo = nullptr;
ISound* ruidoAmbiente = nullptr;
ISound* hollow = nullptr;
ISound* ziggs = nullptr;
ISound* miku = nullptr;
ISound* smiling = nullptr;
ISound* disparo = nullptr;
ISound* arbol = nullptr;
//  3.14.2 - CONFIGURACIÓN DE POSICIONES Y UMBRALES (Constantes de mapa)
glm::vec3 posHollow = glm::vec3(120.0f, 0.0f, -50.0f);
glm::vec3 posZiggs = glm::vec3(-111.0, 0.0f, -50.0f);
glm::vec3 posSmiling = glm::vec3(121.5f, 0.0f, 60.0f);
glm::vec3 posMiku = glm::vec3(-120.0f, 0.0f, 40.0f);
glm::vec3 posArbol = glm::vec3(195.0f, 0.0f, 95.0f);
float umbralSonido = 30.0f;
//  3.14.3 - VARIABLES DINÁMICAS DE CÓMPUTO (Espacio de memoria único)
glm::vec3 camPos = glm::vec3(0.0f);
glm::vec3 camDir = glm::vec3(0.0f);
float distanciaHollow = 0.0f;
float distanciaZiggs = 0.0f;
float distanciaMiku = 0.0f;
float distanciaSmiling = 0.0f;
float distanciaArbol = 0.0f;





// ====================================================================================
// 4. DECLARACIÓN DE OBJETOS DE LA ESCENA
// Vectores, cámara, texturas, modelos y materiales que existirán en el entorno.
// ====================================================================================
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;
Skybox arregloSkyboxes[8];	// Cambiamos enttre 8 variantes
Material Material_brillante;
Material Material_opaco;

// ==========================================
// 4.1 - TEXTURAS Y MODELOS (Evelyn)
// ==========================================
// === ESCENARIO ===
Model Piso;
Model M01, M02, M03, M04, M14;			// VIAS - ESTACION - CABINA - ANDEN - REJAS
Model M05, M06, M08, M21,M22;			// EDIFICIOS STEAMPUNK
Model M12, M23;							// ESTRUCTURAS (Edificios no tan steampunk)
Model M28_1, M28_2, M28_3, M28_4;		// EDIFICIOS "Steampunk" Editados en blender
// --- DECORACIÓN 1
Model M15, M16, M17, M18, M19;			// FUENTE - NAVE - COCHE 1 - BASURA - BANCA		
Model M25, M26, M31, M32, M33_1;		// LIBRERÍA - PINO - COCHE 2 - CEREZO - SEÑAL F1
Model M33_2, M34;							// SEÑAL F2, CARTEL
// --- DESCARTADOS
Model M27, M28, M29, M30;				// Usados dentro de blender
Model M07, M09, M10, M11, M13, M20;		// Por su no uso
// === RUNATERRA (League of Legends) ===
// --- NPC's 
Model LOL_00;							// ZIGGS
Model LOL_01, LOL_02, LOL_04, LOL_05;	// PORO - ANIVIA - PENGU - CRAB
Model LOL_12, LOL_13, LOL_14, LOL_15;	// XAYAH - ANNIE - TRISTANA - SEJUANI
// --- DECORACIÓN 2
Model LOL_03, LOL_06, LOL_07, LOL_08;	// TORRETA - WARD - MAESTRÍA - MAZO JAYCE
Model LOL_09, LOL_10, LOL_11, LOL_16;	// AHRI - BASE - LUMINARIA - MAZO POPPY
Model M35_1, M35_2, M36_1, M36_2;		// RELOJ?
Texture texturaHumo, texturaPoro;		// HUMO - PORO BLUSH
Texture texturaLiteratura;				// Esto es literatura
// ==========================================
// 4.2 - TEXTURAS Y MODELOS (Juan Pablo)
// ==========================================
// AVATAR
Model hw_cuerpo;
Model hw_cabeza;
Model hw_espada;
Model hw_PiernaDerecha;
Model hw_PiernaIzquierda;
Model hw_BrazoDerecho;
Model hw_BrazoIzquierdo;
// BANCA HW
Model hw_banca;
//Hornet HW
Model hornet;
//Cristal HW
Model hw_libros;
//Cornifer HW
Model cornifer;
//Poste HW
Model hw_poste;
//Sherma
Model hw_sherma;
//Shakra
Model hw_marissa;

// ==========================================
// 4.3 - TEXTURAS Y MODELOS (Tony)
// ==========================================
Model Miku;
Model Rin;
Model Len;
Model Teto;
Model Luka;
Model Stage;
Model Piano;
Model Instruments;
Model Magnet;
Model Leeks;
Model Chairs;
Model GearBase;
Model Gear1;
Model Gear2;
Model Gear3;
Model GearHora;
Model GearMinuto;
Model lamparaTony;
Model Scene;
Model CuckooBase;
Model Cuckoo1;
Model Cuckoo2;
Model Cuckoo3;
Model Cuckoo4;
Model Globo;
Model Library;
Model Library2;
Model Library3;
Model Library4;
Model LibraryE;

// ==========================================
// 4.4 - TEXTURAS Y MODELOS (Ruben)
// ==========================================
Model DJSpit_Paredes_M;
Model DJSpit_Basura_M;
Model DJSpit_Escenario_M;
Model DJSpit_Oficina_M;
Model DJSpit_Lata_M;
Model Lampara_Ruben_M;




// ====================================================================================
// 5. SISTEMA DE ILUMINACIÓN
// Arreglos de luces que soporta el Shader.
// ====================================================================================
DirectionalLight arregloLucesDireccionales[8];	// Luces sincronizadas con el Skybox
PointLight pointLights[MAX_POINT_LIGHTS];		// Focos puntuales
SpotLight spotLights[MAX_SPOT_LIGHTS];			// Linternas/Faros direccionales
// Rutas de Shaders
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";


// ====================================================================================
// 6. FUNCIONES DE UTILIDAD Y GEOMETRÍA
// ====================================================================================

// ==========================================
// 6.1 - Normales promedio
// ==========================================
// Calcula normales dinámicas para mallas creadas a mano (Sombreado Phong suave)
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

// ==========================================
// 6.2 - Crear objetos
// ==========================================
void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}

// ==========================================
// 6.3 - Crear Shaders
// ==========================================
void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

// ==========================================
// 6.4 - Evaluar ruta tren chico (Evelyn)
// ==========================================
void CalcularPosicionRutaTren(float distancia, float& outX, float& outZ, float& outRot) {
	if (distancia <= 317.5f) { // Etapa 1 y 2 (-319 a -1.5)
		outX = -319.0f + distancia;
		outZ = 7.26f;
		outRot = 0.0f;
	}
	else if (distancia <= 437.0f) { // Etapa 3 (-1.5 a 118)
		outX = -1.5f + (distancia - 317.5f);
		outZ = 7.26f;
		outRot = 0.0f;
	}
	else if (distancia <= 451.0f) { // Etapa 4 - Curva Mini 1 (118 a 132)
		float factor = (distancia - 437.0f) / 14.0f;
		outX = 118.0f + factor * (132.0f - 118.0f);
		outZ = 7.26f;
		outRot = factor * (-10.0f);
	}
	else if (distancia <= 464.88f) { // Etapa 4 - Curva Mini 2 (132 a 144.6)
		float factor = (distancia - 451.0f) / 13.88f;
		outX = 132.0f + factor * (144.6f - 132.0f);
		outZ = 7.26f + factor * (13.1f - 7.26f);
		outRot = -10.0f + factor * (-23.6f); // Llega a -33.6
	}
	else if (distancia <= 482.93f) { // Etapa 4 - Curva Mini 3 (144.6 a 150.1)
		float factor = (distancia - 464.88f) / 18.05f;
		outX = 144.6f + factor * (150.1f - 144.6f);
		outZ = 13.1f + factor * (30.3f - 13.1f);
		outRot = -33.6f + factor * (-36.4f); // Llega a -70
	}
	else if (distancia <= 494.88f) { // Etapa 4 - Curva Mini 4 (150.1 a 151.1)
		float factor = (distancia - 482.93f) / 11.95f;
		outX = 150.1f + factor * (151.1f - 150.1f);
		outZ = 30.3f + factor * (42.25f - 30.3f);
		outRot = -70.0f + factor * (-20.0f); // Llega a -90
	}
	else { // Etapa 5 (Avance por Z)
		float avance = distancia - 494.88f;
		outX = 151.1f;
		outZ = 42.25f + avance; // El avance ahora llegará hasta 350.0f
		outRot = -90.0f;
	}
}

// ==========================================
// 6.5 - Evaluar ruta tren largo (Evelyn)
// ==========================================
void CalcularPosicionRutaTrenLargo(float distancia, float& outX, float& outZ, float& outRot) {
	if (distancia <= 316.0f) { // Etapa 6 y 7 (330 a 14)
		outX = 330.0f - distancia; // Va hacia los negativos en X
		outZ = -8.0f;
		outRot = 180.0f;
	}
	else if (distancia <= 451.0f) { // Etapa 8 (14 a -121)
		outX = 14.0f - (distancia - 316.0f);
		outZ = -8.0f;
		outRot = 180.0f;
	}
	else if (distancia <= 464.1f) { // Etapa 9 - Curva Mini 1 (-121 a -134.1)
		float factor = (distancia - 451.0f) / 13.1f;
		outX = -121.0f - factor * 13.1f;
		outZ = -8.0f;
		outRot = 180.0f + factor * (-10.0f); // Gira de 180° a 170°
	}
	else if (distancia <= 477.6f) { // Etapa 9 - Curva Mini 2
		float factor = (distancia - 464.1f) / 13.5f;
		outX = -134.1f - factor * 11.8f; // Llega a -145.9
		outZ = -8.0f - factor * 6.6f;    // Llega a -14.6
		outRot = 170.0f + factor * (-23.6f); // Gira de 170° a 146.4°
	}
	else if (distancia <= 497.7f) { // Etapa 9 - Curva Mini 3
		float factor = (distancia - 477.6f) / 20.1f;
		outX = -145.9f - factor * 5.1f;  // Llega a -151.0
		outZ = -14.6f - factor * 19.5f;  // Llega a -34.1
		outRot = 146.4f + factor * (-36.4f); // Gira a 110°
	}
	else if (distancia <= 511.3f) { // Etapa 9 - Curva Mini 4
		float factor = (distancia - 497.7f) / 13.6f;
		outX = -151.0f - factor * 1.0f;  // Llega a -152.0
		outZ = -34.1f - factor * 13.55f; // Llega a -47.65
		outRot = 110.0f + factor * (-20.0f); // Gira a 90° exactos
	}
	else { // Etapa 10 (Avance por Z hacia -500)
		float avance = distancia - 511.3f;
		outX = -152.0f;
		outZ = -47.65f - avance;
		outRot = 90.0f;
	}
}



// ====================================================================================
// 7. FUNCIÓN PRINCIPAL (MAIN)
// ====================================================================================
int main()
{

	// =========================================================
	// --- 7.1. Inicialización de Ventana y Cámara y Sonido ---
	// =========================================================
	mainWindow = Window(1800, 900); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CreateShaders();
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);
	if (SoundEngine) {
		ruidoAmbiente = SoundEngine->play2D("Sounds/ruido.mp3", true, false, true);
		musicaFondo = SoundEngine->play2D("Sounds/inventor.mp3", true, false, true);
		if (ruidoAmbiente) {
			ruidoAmbiente->setVolume(0.04f);
		}
		if (musicaFondo) {
			musicaFondo->setVolume(0.04f);
		}
		// Audio 1: En la oficina (Ej. Coordenadas 0, 0, 0)
		hollow = SoundEngine->play3D("Sounds/hollow.mp3", vec3df(120.0f, 0.0f, -50.0f), true);
		// Audio 2: Junto al tambo / la lata (Ej. Cerca de la base Y=4.8)
		ziggs = SoundEngine->play3D("Sounds/ziggs.mp3", vec3df(-111.0, 0.0f, -50.0f), true);
		// Audio 3: En la vía del tren largo (Ej. Usando tus coordenadas del mesh de fondo)
		smiling = SoundEngine->play3D("Sounds/smiling.mp3", vec3df(121.5f, 0.0f, 60.0f), true);
		// Audio 4: En los Poros Corazones (Ej. Coordenadas de tu mesh de la texturaPoro)
		miku = SoundEngine->play3D("Sounds/miku.mp3", vec3df(-120.0f, 0.0f, 40.0f), true);
		if (hollow) {
			hollow->setMinDistance(10.0f);
			hollow->setMaxDistance(20.0f);
		}
		if (ziggs) {
			ziggs->setVolume(0.4f);
			ziggs->setMinDistance(10.0f);
			ziggs->setMaxDistance(20.0f);
		}
		if (smiling) {
			smiling->setMinDistance(10.0f);
			smiling->setMaxDistance(20.0f);
		}
		if (miku) {
			miku->setVolume(0.2f);
			miku->setMinDistance(10.0f);
			miku->setMaxDistance(20.0f);
		}
		if (disparo) {
			disparo->setMinDistance(20.0f);
			disparo->setMaxDistance(40.0f);
		}
		if (arbol) {
			arbol->setMinDistance(80.0f);
			arbol->setMaxDistance(100.0f);
		}
	}



	// =========================================================
	// --- 7.2. Carga de Assets (Texturas y Modelos 3D) ---
	// =========================================================
	// -----------------------> 7.2.1 - Evelyn
	Piso = Model();				Piso.LoadModel("Models/PISO.obj");
	M01 = Model();				M01.LoadModel("Models/01.obj");
	M02 = Model();				M02.LoadModel("Models/02.obj");
	M03 = Model();				M03.LoadModel("Models/03.obj");
	M04 = Model();				M04.LoadModel("Models/04.obj");
	M05 = Model();				M05.LoadModel("Models/05.obj");
	M06 = Model();				M06.LoadModel("Models/06.obj");
	M08 = Model();				M08.LoadModel("Models/08.obj");
	M12 = Model();				M12.LoadModel("Models/12.obj");
	M14 = Model();				M14.LoadModel("Models/14.obj");
	M15 = Model();				M15.LoadModel("Models/15.obj");
	M16 = Model();				M16.LoadModel("Models/16.obj");
	M17 = Model();				M17.LoadModel("Models/17.obj");
	M18 = Model();				M18.LoadModel("Models/18.obj");
	M19 = Model();				M19.LoadModel("Models/19.obj");
	M20 = Model();				M20.LoadModel("Models/20.obj");
	M21 = Model();				M21.LoadModel("Models/21.obj");
	M22 = Model();				M22.LoadModel("Models/22.obj");
	M23 = Model();				M23.LoadModel("Models/23.obj");
	M25 = Model();				M25.LoadModel("Models/25.obj");
	M26 = Model();				M26.LoadModel("Models/26.obj");
	M27 = Model();				M27.LoadModel("Models/27.obj");
	M28_1 = Model();			M28_1.LoadModel("Models/28-1.obj");
	M28_2 = Model();			M28_2.LoadModel("Models/28-2.obj");
	M28_3 = Model();			M28_3.LoadModel("Models/28-3.obj");
	M28_4 = Model();			M28_4.LoadModel("Models/28-4.obj");
	M29 = Model();				M29.LoadModel("Models/29.obj");
	M30 = Model();				M30.LoadModel("Models/30.obj");
	M31 = Model();				M31.LoadModel("Models/31.obj");
	M32 = Model();				M32.LoadModel("Models/32.obj");
	M33_1 = Model();			M33_1.LoadModel("Models/33-1.obj");
	M33_2 = Model();			M33_2.LoadModel("Models/33-2.obj");
	M34 = Model();				M34.LoadModel("Models/34.obj");
	M35_1 = Model();			M35_1.LoadModel("Models/35_1.obj");
	M35_2 = Model();			M35_2.LoadModel("Models/35_2.obj");
	M36_1 = Model();			M36_1.LoadModel("Models/36_1.obj");
	M36_2 = Model();			M36_2.LoadModel("Models/36_2.obj");
	LOL_00 = Model();			LOL_00.LoadModel("Models/LOL_00.obj");
	LOL_01 = Model();			LOL_01.LoadModel("Models/LOL_01.obj");
	LOL_02 = Model();			LOL_02.LoadModel("Models/LOL_02.obj");
	LOL_03 = Model();			LOL_03.LoadModel("Models/LOL_03.obj");
	LOL_04 = Model();			LOL_04.LoadModel("Models/LOL_04.obj");
	LOL_05 = Model();			LOL_05.LoadModel("Models/LOL_05.obj");
	LOL_06 = Model();			LOL_06.LoadModel("Models/LOL_06.obj");
	LOL_07 = Model();			LOL_07.LoadModel("Models/LOL_07.obj");
	LOL_08 = Model();			LOL_08.LoadModel("Models/LOL_08.obj");
	LOL_09 = Model();			LOL_09.LoadModel("Models/LOL_09.obj");
	LOL_10 = Model();			LOL_10.LoadModel("Models/LOL_10.obj");
	LOL_11 = Model();			LOL_11.LoadModel("Models/LOL_11.obj");
	LOL_12 = Model();			LOL_12.LoadModel("Models/LOL_12.obj");
	LOL_13 = Model();			LOL_13.LoadModel("Models/LOL_13.obj");
	LOL_14 = Model();			LOL_14.LoadModel("Models/LOL_14.obj");
	LOL_15 = Model();			LOL_15.LoadModel("Models/LOL_15.obj");
	LOL_16 = Model();			LOL_16.LoadModel("Models/LOL_16.obj");
	texturaHumo = Texture("Textures/HUMO.tga");				texturaHumo.LoadTextureA();
	texturaPoro = Texture("Textures/35_1.png");				texturaPoro.LoadTextureA();
	texturaLiteratura = Texture("Textures/Literatura.png");	texturaLiteratura.LoadTextureA();

	// -----------------------> 7.2.2 - Juan Pablo
	hw_cuerpo = Model();			hw_cuerpo.LoadModel("Models/hw_cuerpo.obj");
	hw_cabeza = Model();			hw_cabeza.LoadModel("Models/hw_cabeza.obj");
	hw_espada = Model();			hw_espada.LoadModel("Models/hw_espada.obj");
	hw_PiernaDerecha = Model();		hw_PiernaDerecha.LoadModel("Models/hw_PiernaDerecha.obj");
	hw_PiernaIzquierda = Model();	hw_PiernaIzquierda.LoadModel("Models/hw_PiernaIzquierda.obj");
	hw_BrazoDerecho = Model();		hw_BrazoDerecho.LoadModel("Models/hw_BrazoDerecho.obj");
	hw_BrazoIzquierdo = Model();	hw_BrazoIzquierdo.LoadModel("Models/hw_BrazoIzquierdo.obj");

	hw_banca.LoadModel("Models/hw_banca.obj");
	hornet.LoadModel("Models/hornet_mod_1.obj");
	hw_libros.LoadModel("Models/hw_libros.obj");
	cornifer.LoadModel("Models/hw_cornifer.obj");
	hw_poste.LoadModel("Models/hw_poste.obj");
	hw_sherma.LoadModel("Models/hw_sherma.obj");
	hw_marissa.LoadModel("Models/hw_marissa.obj");

	// -----------------------> 7.2.3 - Tony
	Miku = Model(); Miku.LoadModel("Models/MikuM.obj");
	Rin = Model(); Rin.LoadModel("Models/Rin.obj");
	Len = Model(); Len.LoadModel("Models/Len.obj");
	Teto = Model(); Teto.LoadModel("Models/Teto.obj");
	Luka = Model(); Luka.LoadModel("Models/Luka.obj");
	Stage = Model(); Stage.LoadModel("Models/Stage.obj");
	Piano = Model(); Piano.LoadModel("Models/Piano.obj");
	Instruments = Model(); Instruments.LoadModel("Models/Instruments.obj");
	Magnet = Model(); Magnet.LoadModel("Models/Magnet.obj");
	Leeks = Model(); Leeks.LoadModel("Models/Leeks.obj");
	Chairs = Model(); Chairs.LoadModel("Models/Chairs.obj");
	GearBase = Model(); GearBase.LoadModel("Models/Gear_base.obj");
	Gear1 = Model(); Gear1.LoadModel("Models/Gear1.obj");
	Gear2 = Model(); Gear2.LoadModel("Models/Gear2.obj");
	Gear3 = Model(); Gear3.LoadModel("Models/Gear3.obj");
	GearHora = Model(); GearHora.LoadModel("Models/GearHora.obj");
	GearMinuto = Model(); GearMinuto.LoadModel("Models/GearMinuto.obj");
	lamparaTony = Model(); lamparaTony.LoadModel("Models/lamparaTony.obj");
	Scene = Model(); Scene.LoadModel("Models/Scene.obj");
	CuckooBase = Model(); CuckooBase.LoadModel("Models/CuckooBase.obj");
	Cuckoo1 = Model(); Cuckoo1.LoadModel("Models/Cuckoo1.obj");
	Cuckoo2 = Model(); Cuckoo2.LoadModel("Models/Cuckoo2.obj");
	Cuckoo3 = Model(); Cuckoo3.LoadModel("Models/Cuckoo3.obj");
	Cuckoo4 = Model(); Cuckoo4.LoadModel("Models/Cuckoo4.obj");
	Globo = Model(); Globo.LoadModel("Models/Globo.obj");
	Library = Model(); Library.LoadModel("Models/Library.obj");
	Library2 = Model(); Library2.LoadModel("Models/Library2.obj");
	Library3 = Model(); Library3.LoadModel("Models/Library3.obj");
	Library4 = Model(); Library4.LoadModel("Models/Library4.obj");
	LibraryE = Model(); LibraryE.LoadModel("Models/LibraryE.obj");



	// -----------------------> 7.2.4 - Ruben
	DJSpit_Paredes_M = Model();			DJSpit_Paredes_M.LoadModel("Models/djspit-paredes.obj");
	DJSpit_Basura_M = Model();			DJSpit_Basura_M.LoadModel("Models/djspit-basura.obj");
	DJSpit_Escenario_M = Model();		DJSpit_Escenario_M.LoadModel("Models/djspit-escenario.obj");
	DJSpit_Oficina_M = Model();			DJSpit_Oficina_M.LoadModel("Models/smiling-oficina.obj");
	DJSpit_Lata_M = Model();			DJSpit_Lata_M.LoadModel("Models/djspit-lata.obj");
	Lampara_Ruben_M = Model();			Lampara_Ruben_M.LoadModel("Models/puerta.obj");



	// =========================================================
	// --- 7.3. Carga de Skybox (Evelyn) --- y Materiales
	// =========================================================
	std::string prefijosSkybox[8] = {
		"11Amanecer", "12Amanecer", "21Dia", "22Dia",
		"31Atardecer", "32Atardecer", "41Noche", "42Noche"
	};
	for (i = 0; i < 8; i++) {
		std::vector<std::string> skyboxFaces;
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_px.png"); // +X Derecha
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_nx.png"); // -X Izquierda
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_ny.png"); // -Y Abajo (PRIMERO)
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_py.png"); // +Y Arriba (DESPUÉS)
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_pz.png"); // +Z Atrás
		skyboxFaces.push_back("Textures/Skybox/" + prefijosSkybox[i] + "_nz.png"); // -Z Frente
		arregloSkyboxes[i] = Skybox(skyboxFaces);
	}
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	// =========================================================
	// --- 7.4. Configuración Inicial de Iluminación (Evelyn) --
	// =========================================================

	// ----- 7.4.1 - Arreglos auxiliares de posiciones (X, Y, Z)
	glm::vec3 posicionesPostes[8] = {
		glm::vec3(-86.0f, 0.0f, -85.0f),  // Poste 1
		glm::vec3(84.0f, 0.0f, -85.0f),   // Poste 2
		glm::vec3(84.0f, 0.0f, 84.0f),    // Poste 3
		glm::vec3(-86.0f, 0.0f, 84.0f),   // Poste 4
		glm::vec3(-86.0f, 0.0f, 0.0f),    // Poste 5
		glm::vec3(84.0f, 0.0f, 0.0f),     // Poste 6
		glm::vec3(0.0f, 0.0f, 75.0f),     // Poste 7
		glm::vec3(0.0f, 0.0f, -75.0f)     // Poste 8
	};
	glm::vec3 posicionesEdificiosM28[6] = {
		glm::vec3(75.0f, 0.0f, 125.0f),   // M28_1 (Espejeado)
		glm::vec3(-75.0f, 0.0f, -125.0f), // M28_1 (NO Espejeado)
		glm::vec3(-45.0f, 0.0f, 125.0f),  // M28_3 (Espejeado)
		glm::vec3(45.0f, 0.0f, -125.0f),  // M28_3 (NO Espejeado)
		glm::vec3(15.0f, 0.0f, 125.0f),   // M28_2 (Espejeado)
		glm::vec3(-15.0f, 0.0f, -125.0f)  // M28_4 (NO Espejeado)
	};

	// ----- 7.4.2: LUZ DIRECCIONAL GLOBAL (Sincronizada con los Skyboxes)
	// 0: 11Amanecer (Amarillo claro)
	arregloLucesDireccionales[0] = DirectionalLight(1.0f, 0.9f, 0.6f, 0.1f, 0.1f, 0.0f, -1.0f, 0.0f);
	// 1: 12Amanecer (Casi blanco)
	arregloLucesDireccionales[1] = DirectionalLight(1.0f, 0.95f, 0.9f, 0.22f, 0.28f, 0.0f, -1.0f, 0.0f);
	// 2: 21Dia (Casi blanco)
	arregloLucesDireccionales[2] = DirectionalLight(1.0f, 0.95f, 0.9f, 0.44f, 0.44f, 0.0f, -1.0f, 0.0f);
	// 3: 22Dia (Blanco)
	arregloLucesDireccionales[3] = DirectionalLight(1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.0f, -1.0f, 0.0f);
	// 4: 31Atardecer (Naranja Claro)
	arregloLucesDireccionales[4] = DirectionalLight(1.0f, 0.7f, 0.4f, 0.44f, 0.44f, 0.0f, -1.0f, 0.0f);
	// 5: 32Atardecer (Naranja)
	arregloLucesDireccionales[5] = DirectionalLight(1.0f, 0.5f, 0.1f, 0.22f, 0.28f, 0.0f, -1.0f, 0.0f);
	// 6: 41Noche (Azul oscuro)
	arregloLucesDireccionales[6] = DirectionalLight(0.1f, 0.1f, 0.5f, 0.1f, 0.1f, 0.0f, -1.0f, 0.0f);
	// 7: 42Noche (Morado oscuro)
	arregloLucesDireccionales[7] = DirectionalLight(0.3f, 0.1f, 0.4f, 0.05f, 0.05f, 0.0f, -1.0f, 0.0f);

	// ----- 7.4.3: LUCES PUNTUALES (Point Lights)
	unsigned int pointLightCount = 0;
	for (i = 0; i < 8; i++) {
		pointLights[i] = PointLight(1.0f, 0.9f, 0.5f,	// Color amarillento (1.0, 0.9, 0.5)
			0.5f, 1.0f,									// Intensidad ambiental y difusa
			posicionesPostes[i].x, posicionesPostes[i].y + 15.0f, posicionesPostes[i].z,	//+15Y
			1.0f, 0.05f, 0.01f);						// Atenuación suave
	}
	pointLightCount = 8;

	// ----- 7.4.4: LUCES SPOT
	unsigned int spotLightCount = 0;
	// Edificios M28 (Índices 0 a 5) - Luces fijas con dirección hacia el piso
	for (i = 0; i < 6; i++) {
		offsetY = 33.0f;							// +33Y
		// Para Z: +125 restamos 14  // -125 sumamos 14
		offsetZ = (posicionesEdificiosM28[i].z > 0) ? -14.0f : 14.0f;
		// Calculamos posicion de la luz con jerarquía (Edificio + Offset)
		glm::vec3 posicionLuz = posicionesEdificiosM28[i] + glm::vec3(0.0f, offsetY, offsetZ);
		spotLights[i] = SpotLight(1.0f, 0.9f, 0.5f,  // Color amarillo claro
			1.0f, 1.0f,                              // Intensidades (Ambiental, Difusa)
			posicionLuz.x, posicionLuz.y, posicionLuz.z, // Posición
			0.0f, -1.0f, 0.0f,                       // Dirección: Apuntando hacia piso
			1.0f, 0.045f, 0.0075f,                   // Atenuación 
			45.0f);                                  // Ángulo de apertura del cono
	}
	// Luces móviles (Índices 6 y 7) - Luces con dirección hacia adelante (Z negativo)
	spotLights[6] = SpotLight(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.045f, 0.0075f, 30.0f);
	spotLights[7] = SpotLight(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.045f, 0.0075f, 30.0f);
	spotLightCount = 8;


	// =========================================================
	// --- 7.5. Ubicaciones del Shader (Uniforms)
	// =========================================================
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	// ----- 7.5.1: Inicialización de matrices y vectores de apoyo
	GLfloat   now = 0.0f;
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 modelaux = glm::mat4(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	glm::vec3 lowerLight = glm::vec3(0.0f, 0.0f, 0.0f);
	
	// ----- 7.5.2:  VARIABLE DE CONTROL DE CÁMARAS (Juan Pablo)
	// 0 = F (1ra Persona), 1 = G (3ra Persona), 2 = H (Aérea), 3 = J (Libre)
	int tipoCamara = 3; // Iniciamos en 3 (Libre) por default


	// ====================================================================================
	// 8. GAME LOOP (Ciclo Principal de Renderizado)
	// ====================================================================================
	lastTime = glfwGetTime();
	while (!mainWindow.getShouldClose())
	{
		// Control de cuadros por segundo (Cálculo del DeltaTime)
		now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// --- 8.1 FÍSICAS Y ANIMACIONES MATEMÁTICAS
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.1: LÓGICA DE CAMBIO DE SKYBOX (Evelyn)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		temporizadorSkybox += deltaTime;	// Tiempo transcurrido entre cada frame
		// Al superar uan etapa (250f)
		if (temporizadorSkybox >= duracionEtapaSkybox) {
			temporizadorSkybox -= duracionEtapaSkybox;
			indiceSkyboxActual++;			// Pasamos al proximo skybox
			if (indiceSkyboxActual >= 8) {	// Reiniciamos al Amanecer (0)
				indiceSkyboxActual = 0;
			}
		}


		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.2: LÓGICA DE ANIMACION B1 - Reloj (Tony)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// --------------  Reloj animacion básica -------------------------------------
		autoGearBackRotation += autoGearRotSpeed * deltaTime;
		autoStepTimer += deltaTime;
		if (autoStepTimer >= autoStepInterval) {
			autoStepTimer = 0.0f;
			rotAutoGearMinuto -= 30.0f; // Salto de 30 grados
			if (rotAutoGearMinuto <= -360.0f) rotAutoGearMinuto += 360.0f;
			autoMinuteCount++;
			if (autoMinuteCount >= 12) {
				autoMinuteCount = 0;
				rotAutoGearHora -= 30.0f;
				if (rotAutoGearHora <= -360.0f) rotAutoGearHora += 360.0f;
			}
		}
		//  --------------  Reloj animacion por keyframes -------------------------------------
		if (firstKF) {
			incGear1 = (kf_Gear1[1] - kf_Gear1[0]) / (float)max_steps;
			incGear2 = (kf_Gear2[1] - kf_Gear2[0]) / (float)max_steps;
			incGear3 = (kf_Gear3[1] - kf_Gear3[0]) / (float)max_steps;
			incGearMin = (kf_GearMin[1] - kf_GearMin[0]) / (float)max_steps;
			incGearHor = (kf_GearHor[1] - kf_GearHor[0]) / (float)max_steps;
			firstKF = false;
		}
		if (curr_step < max_steps)
		{
			rotGear1 += incGear1;
			rotGear2 += incGear2;
			rotGear3 += incGear3;
			rotGearMin += incGearMin;
			rotGearHor += incGearHor;
			curr_step++;
		}
		else
		{
			curr_step = 0;
			kf_idx++;
			if (kf_idx >= N_KF - 1)
			{
				// Cerramos el ciclo regresamos al keyframe 0 y reseteamos
				// las rotaciones para que no haya drift acumulado
				kf_idx = 0;
				rotGear1 = kf_Gear1[0];
				rotGear2 = kf_Gear2[0];
				rotGear3 = kf_Gear3[0];
				rotGearMin = kf_GearMin[0];
				rotGearHor = kf_GearHor[0];
			}
			// Recalcular incrementos para el nuevo segmento
			incGear1 = (kf_Gear1[kf_idx + 1] - kf_Gear1[kf_idx]) / (float)max_steps;
			incGear2 = (kf_Gear2[kf_idx + 1] - kf_Gear2[kf_idx]) / (float)max_steps;
			incGear3 = (kf_Gear3[kf_idx + 1] - kf_Gear3[kf_idx]) / (float)max_steps;
			incGearMin = (kf_GearMin[kf_idx + 1] - kf_GearMin[kf_idx]) / (float)max_steps;
			incGearHor = (kf_GearHor[kf_idx + 1] - kf_GearHor[kf_idx]) / (float)max_steps;
		}

		// ------------------------------ Cuckoo animacion por keyframes-------------------------
		if (ck_firstKF) {
			ck_incPendulo = (ck_kf_pendulo[1] - ck_kf_pendulo[0]) / (float)ck_max_steps;
			ck_incPajaro = (ck_kf_pajaro[1] - ck_kf_pajaro[0]) / (float)ck_max_steps;
			ck_incCad3 = (ck_kf_cad3[1] - ck_kf_cad3[0]) / (float)ck_max_steps;
			ck_incCad4 = (ck_kf_cad4[1] - ck_kf_cad4[0]) / (float)ck_max_steps;
			ck_firstKF = false;
		}
		if (ck_curr_step < ck_max_steps)
		{
			ck_rotPendulo += ck_incPendulo;
			ck_rotPajaro += ck_incPajaro;
			ck_trasCad3 += ck_incCad3;
			ck_trasCad4 += ck_incCad4;
			ck_curr_step++;
		}
		else
		{
			ck_curr_step = 0;
			ck_kf_idx++;
			if (ck_kf_idx >= N_KF_CK - 1)
			{
				// Cierre del ciclo — resetear a keyframe 0 sin drift
				ck_kf_idx = 0;
				ck_rotPendulo = ck_kf_pendulo[0];
				ck_rotPajaro = ck_kf_pajaro[0];
				ck_trasCad3 = ck_kf_cad3[0];
				ck_trasCad4 = ck_kf_cad4[0];
			}
			// Recalcular incrementos para el nuevo segmento
			ck_incPendulo = (ck_kf_pendulo[ck_kf_idx + 1] - ck_kf_pendulo[ck_kf_idx]) / (float)ck_max_steps;
			ck_incPajaro = (ck_kf_pajaro[ck_kf_idx + 1] - ck_kf_pajaro[ck_kf_idx]) / (float)ck_max_steps;
			ck_incCad3 = (ck_kf_cad3[ck_kf_idx + 1] - ck_kf_cad3[ck_kf_idx]) / (float)ck_max_steps;
		}

		
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.3: LÓGICA DE ANIMACION B2 - Locomotora (Evelyn)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		gatilloB = mainWindow.getContadorTeclaB();	// ¿Cuantas veces hemos apretado B?
		// RESET AUTOMÁTICO AL VOLVER A 0
		if (gatilloB == 0) {
			// Reseteamos tren chico y largo
			subEtapaTrenChico = 1;
			temporizadorTrenChico = 0.0f;
			distanciaTrenChico = 0.0f;
			subEtapaTrenLargo = 6;
			temporizadorTrenLargo = 0.0f;
			distanciaTrenLargo = 0.0f;
		}
		// Maquina del tren chico (Índices 1 a 5)
		if (gatilloB >= 1) {
			// --- ETAPA 1: Avanzar y frenar suavemente ---
			if (subEtapaTrenChico == 1) {
				temporizadorTrenChico += deltaTime;
				progreso = temporizadorTrenChico / duracionEtapaSkybox;
				if (progreso >= 1.0f) progreso = 1.0f;
				// Función seno:Para frenado suave
				pSuave = sin(progreso * 1.570796f);	// 1.570796f es PI/2
				distanciaTrenChico = 317.5f * pSuave;		// 317.5 dist_total a recorrer en esta etapa
				if (progreso >= 1.0f && gatilloB >= 2) {	// ¿B se apreto ya dos veces y llegamos a la meta?
					subEtapaTrenChico = 2;
					temporizadorTrenChico = 0.0f;			// Reset del reloj para nueva etapa
				}
			}
			// --- ETAPA 2: Espera en el andén ---
			else if (subEtapaTrenChico == 2) {
				temporizadorTrenChico += deltaTime;
				// Solo espera inactivo durante la mitad del tiempo de un skybox
				if (temporizadorTrenChico >= (duracionEtapaSkybox / 2.0f)) {
					subEtapaTrenChico = 3;
					temporizadorTrenChico = 0.0f;
				}
			}
			// --- ETAPA 3: Arranque lento (Ease-In) en X ---
			else if (subEtapaTrenChico == 3) {
				temporizadorTrenChico += deltaTime;
				progreso = temporizadorTrenChico / duracionEtapaSkybox;
				if (progreso >= 1.0f) progreso = 1.0f;
				// Función cuadrática: Inicia lento (cerca de 0) y acelera progresivamente
				pSuave = progreso * progreso;			// Curva cuadrática para arranque lento
				distanciaTrenChico = 317.5f + (119.5f * pSuave); // 119.5 es la distancia extra a recorrer
				// Transición automática al llegar a la curva
				if (progreso >= 1.0f) {
					subEtapaTrenChico = 4;
					temporizadorTrenChico = 0.0f;
				}
			}
			// --- ETAPA 4: Curva ---
			else if (subEtapaTrenChico == 4) {
				temporizadorTrenChico += deltaTime;
				progreso = temporizadorTrenChico / (duracionEtapaSkybox / 2.0f);
				if (progreso >= 1.0f) progreso = 1.0f;
				distanciaTrenChico = 437.0f + (57.88f * progreso); // 57.88 es la distancia extra a recorrer
				if (progreso >= 1.0f) {
					subEtapaTrenChico = 5;
					temporizadorTrenChico = 0.0f;
				}
			}
			// --- ETAPA 5: Avance en Z ---
			else if (subEtapaTrenChico == 5) {
				temporizadorTrenChico += deltaTime;
				progreso = temporizadorTrenChico / duracionEtapaSkybox;
				if (progreso >= 1.0f) progreso = 1.0f;
				distanciaTrenChico = 494.88f + (307.75f * progreso); // 307.75 es la distancia extra a recorrer
			}
		}

		// Maquina del tren largo (Índices 6 a 10)
		if (gatilloB >= 3) {
			// --- ETAPA 6: Avanzar y frenar suavemente ---
			if (subEtapaTrenLargo == 6) {
				temporizadorTrenLargo += deltaTime;
				progreso = temporizadorTrenLargo / duracionEtapaSkybox;
				if (progreso >= 1.0f) progreso = 1.0f;
				pSuave = sin(progreso * 1.570796f);
				distanciaTrenLargo = 316.0f * pSuave; // 316.0 es la distancia de 330 a 14
				// Transición a Etapa 7 SOLO si ya apretamos la B por cuarta vez
				if (progreso >= 1.0f && gatilloB >= 4) {
					subEtapaTrenLargo = 7;
					temporizadorTrenLargo = 0.0f;
				}
			}
			// --- ETAPA 7: Espera en el andén ---
			else if (subEtapaTrenLargo == 7) {
				temporizadorTrenLargo += deltaTime;
				if (temporizadorTrenLargo >= (duracionEtapaSkybox / 2.0f)) {
					subEtapaTrenLargo = 8;
					temporizadorTrenLargo = 0.0f;
				}
			}
			// --- ETAPA 8: Arranque lento en X ---
			else if (subEtapaTrenLargo == 8) {
				temporizadorTrenLargo += deltaTime;
				progreso = temporizadorTrenLargo / duracionEtapaSkybox;
				if (progreso >= 1.0f) progreso = 1.0f;
				pSuave = progreso * progreso;
				distanciaTrenLargo = 316.0f + (135.0f * pSuave); // 135 es la distancia de 14 a -121
				if (progreso >= 1.0f) {
					subEtapaTrenLargo = 9;
					temporizadorTrenLargo = 0.0f;
				}
			}
			// --- ETAPA 9: La Curva de 90° (Derecha) ---
			else if (subEtapaTrenLargo == 9) {
				temporizadorTrenLargo += deltaTime;
				progreso = temporizadorTrenLargo / (duracionEtapaSkybox / 2.0f);
				if (progreso >= 1.0f) progreso = 1.0f;
				distanciaTrenLargo = 451.0f + (60.3f * progreso); // 60.3 es la distancia extra a recorrer
				if (progreso >= 1.0f) {
					subEtapaTrenLargo = 10;
					temporizadorTrenLargo = 0.0f;
				}
			}
			// --- ETAPA 10: Avance recto en Z ---
			else if (subEtapaTrenLargo == 10) {
				temporizadorTrenLargo += deltaTime;
				progreso = temporizadorTrenLargo / duracionEtapaSkybox;
				if (progreso >= 1.0f) progreso = 1.0f;
				distanciaTrenLargo = 511.3f + (452.35f * progreso);	// 452.35 es la distancia extra a recorrer
			}
		}


	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// ----- 8.1.4-5: LÓGICA DE ANIMACION - Esfera y Árbol (Ruben)
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//Reset de animación
		if (mainWindow.getAccionE()) {
			movEsferaX = 0.0f;
			movEsferaZ = 0.0f;
			rotEsferaY = 0.0f;
			rotEsferaZ = 0.0f;
			rotArbol = 0.0f;
			arbol_cayendo = false;
			mainWindow.apagarAccionE();
		}
		if (movEsferaX < puntoDestinoX) {
			//Avance y movimiento de serpenteo
			movEsferaX += velocidadAvance * deltaTime;
			movEsferaZ = 4.0f * sin(movEsferaX * 0.1f);
			//Rotación sobre su propio eje
			rotEsferaZ -= 10.0f * deltaTime;
			rotEsferaY = 10.0f * sin(movEsferaX * 0.1f);
		}
		else {
			if (rotArbol < anguloCaida) {
				if (rotArbol < 15.0f) {
					velocidadCaida = 0.1f;
				}
				else {
					velocidadCaida = 0.5f;
				}
				rotArbol += velocidadCaida * deltaTime;
			}
			if (!arbol_cayendo) {
				if (SoundEngine) {
					SoundEngine->play3D("Sounds/arbol.mp3", vec3df(195.0f, 0.0f, 95.0f), false);
				}
				arbol_cayendo = true;
			}
			//Forzar la posición exacta al llegar para evitar desfases en la animación
			movEsferaX = puntoDestinoX;
		}


		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.6: LÓGICA DE ANIMACION A2 - Humo (Evelyn)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		temporizadorCicloHumo += deltaTime;
		if (temporizadorCicloHumo >= 200.0f) {	// El ciclo completo dura 200.0f
			temporizadorCicloHumo -= 200.0f;	
		}
		escalaBase = 0.0f;
		// ETAPA 1 y 2 (Primeros 50 frames): De -3Y a 0Y expandiendo escala de 0 a 5.
		if (temporizadorCicloHumo <= 50.0f) {
			progreso = temporizadorCicloHumo / 50.0f; // Normalizamos de 0.0 a 1.0
			offsetYHumo = -3.0f + (progreso * 3.0f);        // Sube de -3 a 0
			escalaBase = progreso * 5.0f;                   // Crece de 0 a 5
		}
		// ETAPA 3 (Siguientes 100 frames): Mantenemos 0Y y escala 5
		else if (temporizadorCicloHumo <= 150.0f) {
			offsetYHumo = 0.0f;
			escalaBase = 5.0f;
		}
		// ETAPA 4 (Últimos 50 frames): De 0Y a +3Y encogiendo escala de 5 a 0
		else {
			progreso = (temporizadorCicloHumo - 150.0f) / 50.0f; // Normalizamos de 0.0 a 1.0
			offsetYHumo = progreso * 3.0f;               // Sube de 0 a +3
			escalaBase = 5.0f - (progreso * 5.0f);       // Se encoge de 5 a 0
		}
		// Agregamos seno encima para ver el humo "vibrando"
		escalaHumoAnimada = escalaBase + (sin(glfwGetTime() * 3.0f) * 1.5f * (escalaBase / 5.0f));
		// Prevenir escalas negativas
		if (escalaHumoAnimada < 0.0f) escalaHumoAnimada = 0.0f;


		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.7: LÓGICA DE ANIMACION A3 - Disparo (Ruben)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		if (mainWindow.getAccionT()) {
			disparoActivo = true;
			if (SoundEngine) {
				SoundEngine->play3D("Sounds/disparo.mp3", vec3df(121.5f, 0.0f, 60.0f), false);
			}
			mainWindow.apagarAccionT();
		}
		if (mainWindow.getAccionR()) {
			lataPosZ = 0.0f;
			alturaBaseActual = ALTURA_ORIGINAL;
			lataPosY = alturaBaseActual;
			rotLata = 0.0f;
			tiempoSalto = 0.0f;
			rebotesContados = 0;
			velocidadSaltoActual = VEL_SALTO_ORIGINAL; // Restaura la fuerza del salto original
			mainWindow.apagarAccionR();
		}
		if (disparoActivo) {
			tiempoSalto += deltaTime * 0.05f;
			velocidadHorizontalBase = 5.0f;
			factorGiro = (rebotesContados == 0) ? 500.0f : 180.0f;
			rotLata += factorGiro * deltaTime;
			if (rebotesContados == 0) {
				// --- PARÁBOLA 1 (Desde el tambo al suelo) ---
				lataPosZ = 0.0f + (velocidadHorizontalBase * tiempoSalto);
				lataPosY = alturaBaseActual + (velocidadSaltoActual * tiempoSalto) - (0.5f * 9.81f * tiempoSalto * tiempoSalto);
			}
			else {
				// --- PARÁBOLA 2 ---
				lataPosZ += 0.5 * deltaTime;
				// Ecuación del rebote desde el suelo
				lataPosY = 0.0f + (velocidadSaltoActual * tiempoSalto) - (0.5f * 9.81f * tiempoSalto * tiempoSalto);
			}
			// Detección de cuando la lata llega al suelo, aún no ha rebotado y se verifica
			// que el tiempo haya avanzado para que no se cicle de forma infinita en 0
			if (lataPosY <= 0.0f && rebotesContados < 1 && tiempoSalto > 0.05f) {
				lataPosY = 0.0f;
				tiempoSalto = 0.0f;
				alturaBaseActual = 0.0f;
				rebotesContados++;
				semillaAleatoria = static_cast<float>(rand() % 100) / 100.0f;
				// Reducimos la fuerza del segundo salto de forma aleatoria (entre 50% y 70% con tus nuevos valores)
				factorRestitucion = 0.50f + (semillaAleatoria * 0.20f);
				velocidadSaltoActual *= factorRestitucion;
				// Un poco de ruido para que la lata no siempre caiga en el mismo lugar
				dispersionGiro = sinf(semillaAleatoria * 3.1416f) * 1.5f;
				lataPosZ += dispersionGiro;
			}
			// Forzar la posición final en Y de la lata
			if (rebotesContados >= 1 && lataPosY <= 0.0f && tiempoSalto > 0.1f) {
				lataPosY = 0.0f;
				disparoActivo = false; // Termina la animación por completo
			}
		}


		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.8: LÓGICA DE ANIMACION POROS SALTANDO (Evelyn)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		for (pIter = 0; pIter < 5; pIter++) {
			temporizadoresPoros[pIter] += deltaTime;
			// Reinicio del ciclo (reteniendo los milisegundos extra para precisión)
			if (temporizadoresPoros[pIter] >= duracionTotalPoro) {
				temporizadoresPoros[pIter] -= duracionTotalPoro;
			}
			// ETAPA 1: Salto hacia arriba
			if (temporizadoresPoros[pIter] <= duracionSubidaPoro) {
				progresoPoro = temporizadoresPoros[pIter] / duracionSubidaPoro;
				if (progresoPoro > 1.0f) progresoPoro = 1.0f; // Blindaje del progreso
				alturasPoros[pIter] = alturaMaximaPoro * sin(progresoPoro * 1.570796f);
			}
			// ETAPA 2: Caída hacia abajo (Ease-In)
			else {
				progresoPoro = (temporizadoresPoros[pIter] - duracionSubidaPoro) / duracionBajadaPoro;
				if (progresoPoro > 1.0f) progresoPoro = 1.0f; // Blindaje del progreso
				alturasPoros[pIter] = alturaMaximaPoro * cos(progresoPoro * 1.570796f);
			}
			// Forzado a no sobrepasar el suelo
			if (alturasPoros[pIter] < 0.0f) {
				alturasPoros[pIter] = 0.0f;
			}
		}

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.9: LÓGICA DE ANIMACION - PÁGINA DEL LIBRO (Evelyn)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		rotacionPagina += velocidadPagina * deltaTime;
		// Bucle 360 grados
		if (rotacionPagina >= 360.0f) {
			rotacionPagina -= 360.0f;
		}
		
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ----- 8.1.10: LÓGICA DE ANIMACION - LITERATURA (Evelyn)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		temporizadorLiteratura += deltaTime;
		if (temporizadorLiteratura >= litDuracionTotal) {
			temporizadorLiteratura -= litDuracionTotal;
		}
		// --- ETAPA 1: Nace (Escala 0 a normal) y avanza de -36.35 a -33.85 ---
		if (temporizadorLiteratura <= litDuracionE1) {
			progresoLit = temporizadorLiteratura / litDuracionE1;
			if (progresoLit > 1.0f) progresoLit = 1.0f;
			// SUMAMOS 2.5f para acercarnos al origen
			litPosZ = -36.35f + (2.5f * progresoLit);
			// Crece de 0 hasta tu escala original (3.5, 3.5, 2.0)
			litEscalaX = 3.5f * progresoLit;
			litEscalaY = 3.5f * progresoLit;
			litEscalaZ = 2.0f * progresoLit;
		}
		// --- ETAPA 2: Mantiene escala normal y avanza de -33.85 a -31.85 ---
		else if (temporizadorLiteratura <= (litDuracionE1 + litDuracionE2)) {
			progresoLit = (temporizadorLiteratura - litDuracionE1) / litDuracionE2;
			if (progresoLit > 1.0f) progresoLit = 1.0f;
			// Partimos de -33.85 y SUMAMOS 2.0f para acercarnos más
			litPosZ = -33.85f + (2.0f * progresoLit);
			// Se mantiene en su tamaño máximo
			litEscalaX = 3.5f;
			litEscalaY = 3.5f;
			litEscalaZ = 2.0f;
		}
		// --- ETAPA 3: Se encoge a 0 y avanza de -31.85 a -30.35 ---
		else {
			progresoLit = (temporizadorLiteratura - litDuracionE1 - litDuracionE2) / litDuracionE3;
			if (progresoLit > 1.0f) progresoLit = 1.0f;
			// Partimos de -31.85 y SUMAMOS 1.5f para llegar al punto final más cercano (-30.35)
			litPosZ = -31.85f + (1.5f * progresoLit);
			// Se encoge desde el máximo hasta 0
			litEscalaX = 3.5f - (3.5f * progresoLit);
			litEscalaY = 3.5f - (3.5f * progresoLit);
			litEscalaZ = 2.0f - (2.0f * progresoLit);
		}



		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// --- 8.2 LOGICA DE CONTROL DE CÁMARAS Y MOVIMIENTO DEL AVATAR (Juan Pablo)
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// Capturar eventos de la ventana
		glfwPollEvents();
		// --- 1. ACTUALIZAR ESTADO DE CÁMARA ---
		if (mainWindow.getAccionF()) { tipoCamara = 0; mainWindow.apagarAccionF(); }
		if (mainWindow.getAccionG()) { tipoCamara = 1; mainWindow.apagarAccionG(); }
		if (mainWindow.getAccionH()) { tipoCamara = 2; mainWindow.apagarAccionH(); }
		if (mainWindow.getAccionJ()) { tipoCamara = 3; mainWindow.apagarAccionJ(); }
		// Leer el ratón una sola vez
		deltaX = mainWindow.getXChange();
		deltaY = mainWindow.getYChange();
		// --- 2. LÓGICA DEL RATÓN ---
		if (tipoCamara == 3) {
			// [J] CÁMARA LIBRE: Controla su propio giro
			camera.mouseControl(deltaX, deltaY);
		}
		else {
			// CÁMARAS ANCLADAS: El ratón gira la cintura/cuerpo del personaje
			// Ajusta el 0.3f si quieres que el ratón gire al personaje más rápido o más lento
			cuerpoRotY -= deltaX * 0.3f;
		}
		isMoving = false;
		// --- 3. CÁLCULO DE VECTORES DIRECCIONALES DEL AVATAR ---
		posAvatar = glm::vec3(cuerpoPosX, 1.0f, cuerpoPosZ);
		rotacionGlobal = 180.0f + cuerpoRotY;
		radRot = rotacionGlobal * toRadians;
		// Vector que apunta hacia el frente del personaje
		forwardAvatar = glm::vec3(sin(radRot), 0.0f, cos(radRot));
		up = glm::vec3(0.0f, 1.0f, 0.0f);
		// Vector que apunta hacia la derecha del personaje
		rightAvatar = glm::normalize(glm::cross(up, forwardAvatar));
		// --- 4. CONTROL DE MOVIMIENTO (A, W, S, D) Y ANIMACIÓN ---
		velocity = 0.15f * deltaTime; //la velocidad de caminar
		if (tipoCamara == 3) {
			// Si es cámara libre, el WASD mueve a la cámara por el aire, no al avatar
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
		}
		else {
			// Si es cámara de jugador, el WASD mueve al avatar
			if (mainWindow.getsKeys()[GLFW_KEY_W]) {
				cuerpoPosX += forwardAvatar.x * velocity;
				cuerpoPosZ += forwardAvatar.z * velocity;
				isMoving = true;
			}
			if (mainWindow.getsKeys()[GLFW_KEY_S]) {
				cuerpoPosX -= forwardAvatar.x * velocity;
				cuerpoPosZ -= forwardAvatar.z * velocity;
				isMoving = true;
			}
			if (mainWindow.getsKeys()[GLFW_KEY_D]) {
				cuerpoPosX -= rightAvatar.x * velocity;
				cuerpoPosZ -= rightAvatar.z * velocity;
				isMoving = true;
			}
			if (mainWindow.getsKeys()[GLFW_KEY_A]) {
				cuerpoPosX += rightAvatar.x * velocity; 
				cuerpoPosZ += rightAvatar.z * velocity;
				isMoving = true;
			}
		}
		// --- 5. ANIMACIÓN CONDICIONADA A MOVIMIENTO ---
		if (isMoving) {
			// Usamos una función seno basada en el tiempo para oscilar brazos/piernas de forma natural
			// 10.0f = Qué tan rápido mueve los brazos. 45.0f = Cuántos grados se levantan.
			rotacionAvatar = sin(glfwGetTime() * 10.0f) * 45.0f;
		}
		else {
			rotacionAvatar = 0.0f; // Personaje quieto con extremidades rectas
		}
		// --- 6. CÁLCULO FINAL DE LAS MATRICES DE VISTA ---
		if (tipoCamara == 3) {
			viewMatrix = camera.calculateViewMatrix();
			posCamara = camera.getCameraPosition();
		}
		else {
			// Actualizamos la posición base del avatar por si se movió
			posAvatar = glm::vec3(cuerpoPosX, 1.0f, cuerpoPosZ);

			if (tipoCamara == 0) { // F (Primera Persona)
				posCamara = posAvatar + glm::vec3(0.0f, 1.2f, 0.0f) + forwardAvatar * 1.2f;
				targetCamara = posCamara + forwardAvatar;
			}
			else if (tipoCamara == 1) { // G (Hombro / Tercera Persona)
				posCamara = posAvatar + glm::vec3(0.0f, 1.8f, 0.0f) - forwardAvatar * 5.0f + rightAvatar * 1.0f;
				targetCamara = posAvatar + glm::vec3(0.0f, 1.2f, 0.0f) + forwardAvatar * 5.0f;
			}
			else if (tipoCamara == 2) { // H (Aérea)
				posCamara = posAvatar + glm::vec3(0.0f, 50.0f, -0.1f);
				targetCamara = posAvatar;
			}
			viewMatrix = glm::lookAt(posCamara, targetCamara, up);
		}


		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// --- 8.3 PREPARACIÓN DE SHADERS, ENVÍO DE UNIFORMS Y DIBUJADO DE OBJETOS
		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// Preparación del Buffer y dibujo del fondo
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		arregloSkyboxes[indiceSkyboxActual].DrawSkybox(camera.calculateViewMatrix(), projection);
		// Activación del Shader Principal y recuperación de sus Uniforms
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		// Usamos viewMatrix y posCamara
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3f(uniformEyePosition, posCamara.x, posCamara.y, posCamara.z);

		
		// ===============================================================================
		// --- 8.4: CÁLCULO DE JERARQUÍAS DE LUCES
		// ===============================================================================
		
		// 1. FARO DEL TREN CHICO (Tecla 7 -> Índice 6)
		CalcularPosicionRutaTren(distanciaTrenChico, cx, cz, cRotY);
		// Creamos la matriz del "padre" (el tren) para usarla como ancla de los "hijos" (luz y humo)
		trenChicoMat = glm::mat4(1.0f);
		trenChicoMat = glm::translate(trenChicoMat, glm::vec3(cx, 0.0f, cz));
		trenChicoMat = glm::rotate(trenChicoMat, cRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		posFaroChico = glm::vec3(trenChicoMat * glm::vec4(18.0f, 9.5f, 0.0f, 1.0f));
		dirFaroChico = glm::normalize(glm::vec3(trenChicoMat * glm::vec4(1.0f, -0.2f, 0.0f, 0.0f)));
		// Sobrescribimos el SpotLight 6 con las nuevas coordenadas y direcciones calculadas
		spotLights[6] = SpotLight(1.0f, 1.0f, 1.0f,   // Color Blanco
			1.0f, 1.0f,                               // Intensidades
			posFaroChico.x, posFaroChico.y, posFaroChico.z, // Posición acoplada
			dirFaroChico.x, dirFaroChico.y, dirFaroChico.z, // Dirección de enfoque
			1.0f, 0.022f, 0.0019f,                    // Atenuación (Valores bajos = llega más lejos)
			35.0f);                                   // Ángulo de apertura del cono de luz de 35°
		// 2. FARO DEL TREN LARGO (Tecla 8 -> Índice 7)
		
		CalcularPosicionRutaTrenLargo(distanciaTrenLargo, lx, lz, lRotY);
		// Creamos la matriz del "padre" (el tren) para usarla como ancla de los "hijos" (luz y humo)
		trenLargoMat = glm::mat4(1.0f);
		trenLargoMat = glm::translate(trenLargoMat, glm::vec3(lx, 0.0f, lz));
		trenLargoMat = glm::rotate(trenLargoMat, lRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		posFaroLargo = glm::vec3(trenLargoMat * glm::vec4(18.0f, 9.5f, 0.0f, 1.0f));
		dirFaroLargo = glm::normalize(glm::vec3(trenLargoMat * glm::vec4(1.0f, -0.2f, 0.0f, 0.0f)));
		// Sobrescribimos el SpotLight 7 con las nuevas coordenadas y direcciones calculadas
		spotLights[7] = SpotLight(1.0f, 1.0f, 1.0f,   // Color Blanco
			1.0f, 1.0f,                               // Intensidades
			posFaroLargo.x, posFaroLargo.y, posFaroLargo.z,
			dirFaroLargo.x, dirFaroLargo.y, dirFaroLargo.z,
			1.0f, 0.022f, 0.0019f,                    // Atenuación para llegar mucho más lejos
			35.0f);                                   // Ángulo de apertura de 35°
		matrizHumoChico = trenChicoMat;
		matrizHumoLargo = trenLargoMat;

		// ===============================================================================
		// --- 8.5: ENVÍO DE LUCES AL SHADER
		// ===============================================================================
		// ENVIO de luz DIRECCIONAL (Siempre 1, cambia según el skybox)
		shaderList[0].SetDirectionalLight(&arregloLucesDireccionales[indiceSkyboxActual]);

		// ENVIO de luces PUNTUALES (Siempre 8, pero solo algunas activas según el skybox)
		lucesPuntualesActivas = 0;
		if (indiceSkyboxActual == 0 || indiceSkyboxActual == 5 ||
			indiceSkyboxActual == 6 || indiceSkyboxActual == 7)
		{
			lucesPuntualesActivas = 8;
		}
		shaderList[0].SetPointLights(pointLights, lucesPuntualesActivas);

		// ENVIO de luces SPOT (Variable, según teclas 1 a 8)
		bool* estadosSpot = mainWindow.getStatusLucesSpot();
		SpotLight lucesSpotActivas[8];			// ATemp solo para las luces que estén encendidas
		contadorSpotActivas = 0;	// Lleva la cuenta real de cuántas enviaremos
		for (i = 0; i < 8; i++) {			// Recoremos catalogo de las 8 SpotLights
			if (estadosSpot[i]) {
				lucesSpotActivas[contadorSpotActivas] = spotLights[i];
				contadorSpotActivas++;
			}
		}
		shaderList[0].SetSpotLights(lucesSpotActivas, contadorSpotActivas);
		// Limpieza de offsets de textura y color
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));



		// ===============================================================================
		// --- 8.6: RENDERIZADO DE MODELOS OPACOS
		// ===============================================================================

		// === LUMINARIAS (Desagrupadas por índices del arreglo) ===
		// Poste 1 (Coordenada: -86, 0, -85) - Modelo LOL_12
		model = glm::mat4(1.0);
		model = glm::translate(model, posicionesPostes[0]);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_12.RenderModel();
		// Poste 2 (Coordenada: 84, 0, -85) - Modelo hw_poste
		model = glm::mat4(1.0f);
		model = glm::translate(model, posicionesPostes[1]);
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 3.5f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_poste.RenderModel();
		// Poste 3 (Coordenada: 84, 0, 84) - Modelo Lampara_Ruben_M
		model = glm::mat4(1.0);
		model = glm::translate(model, posicionesPostes[2]);
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara_Ruben_M.RenderModel();
		// Poste 4 (Coordenada: -86, 0, 84) - Modelo lamparaTony
		model = glm::mat4(1.0);
		model = glm::translate(model, posicionesPostes[3]);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lamparaTony.RenderModel();
		// Poste 5 (Coordenada: -86, 0, 0) - Modelo LOL_12
		model = glm::mat4(1.0);
		model = glm::translate(model, posicionesPostes[4]);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_12.RenderModel();
		// Poste 6 (Coordenada: 84, 0, 0) - Modelo Lampara_Ruben_M
		model = glm::mat4(1.0);
		model = glm::translate(model, posicionesPostes[5]);
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara_Ruben_M.RenderModel();
		// Poste 7 (Coordenada: 0, 0, 75) - Modelo lamparaTony
		model = glm::mat4(1.0);
		model = glm::translate(model, posicionesPostes[6]);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lamparaTony.RenderModel();
		// Poste 8 (Coordenada: 0, 0, -75) - Modelo hw_poste
		model = glm::mat4(1.0f);
		model = glm::translate(model, posicionesPostes[7]);
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 3.5f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_poste.RenderModel();


		// >>>>>>>>>>>>>>>>>>>>>>>>> TEXTURAS Y MODELOS - EVELYN >>>>>>>>>>>>>>>>>>>>>>>>>
		// === ESCENARIO ===
		// PISO BASE
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Piso.RenderModel();
		// VIAS
		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M01.RenderModel();
		// ESTACIÓN (Edificio Principal)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M02.RenderModel();
		// === TREN CHICO ===
		// 1. DIBUJAR CABINA
		CalcularPosicionRutaTren(distanciaTrenChico, posX, posZ, rotY);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, rotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model; // Guardamos para el Poro (El Poro SÍ va montado en la cabina)
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M03.RenderModel();
		// 2. DIBUJAR PORO (Jerarquía Mantenida)
		model = modelaux;
		model = glm::translate(model, glm::vec3(-12.63f, 9.28f, -0.13f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// 3. DIBUJAR ANDEN/VAGON (Jerarquía Rota: Sigue la misma ruta con retraso de 48.5 unidades)
		CalcularPosicionRutaTren(distanciaTrenChico - 48.5f, posX, posZ, rotY);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, rotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();
		// === TREN LARGO ===
		// 1. DIBUJAR CABINA
		CalcularPosicionRutaTrenLargo(distanciaTrenLargo, posX, posZ, rotY);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, rotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M03.RenderModel();
		// 2. DIBUJAR VAGON 1 (49 unidades detrás)
		CalcularPosicionRutaTrenLargo(distanciaTrenLargo - 49.0f, posX, posZ, rotY);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, rotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();
		// 3. DIBUJAR VAGON 2 (106 unidades detrás)
		CalcularPosicionRutaTrenLargo(distanciaTrenLargo - 106.0f, posX, posZ, rotY);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, rotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();
		// 4. DIBUJAR VAGON 3 (162 unidades detrás)
		CalcularPosicionRutaTrenLargo(distanciaTrenLargo - 162.0f, posX, posZ, rotY);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX, 0.0f, posZ));
		model = glm::rotate(model, rotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M04.RenderModel();
		// === ESCENARIO ===
		// Barandal Edificio Principal
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 50.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M14.RenderModel();
		// Barandal Biblioteca
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -50.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M14.RenderModel();
		// === EDIFICIOS STEAMPUNK ===
		// Edificio No 3 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M05.RenderModel();
		// Edificio No 3 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, -120.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M05.RenderModel();
		// Edificio No 5 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M06.RenderModel();
		// Edificio No 5 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, -120.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M06.RenderModel();
		// Edificio No 7 (Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M08.RenderModel();
		// Edificio No 7 (NO Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, -120.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M08.RenderModel();
		// Edificio No 1 y 8 (Espejeado / No Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-100.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-105.0f, 0.0f, -120.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f, 0.0f, -120.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M21.RenderModel();
		// Puesto Ziggs
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-105.0f, 0.0f, -30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M22.RenderModel();
		// === EDIFICIOS No tan steampunk ===
		// Estructura (Al lado del puesto Ziggs)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M12.RenderModel();
		// Biblioteca
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M23.RenderModel();
		// === EDIFICIOS STEAMPUNK MODIFICADOS EN BLENDER ===
		// Edificio No 2 (Espejeado / No Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, 125.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_1.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, -125.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_1.RenderModel();
		// Edificio No 6 (Espejeado / No Espejeado)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, 125.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_3.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 0.0f, -125.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_3.RenderModel();
		// Edificio No 6 Variantes
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 0.0f, 125.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_2.RenderModel(); // M28_2 Espejeado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -125.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M28_4.RenderModel(); // M28_4 NO Espejeado
		// === DECORACION 1 ===
		// Mini fuente
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-120.0f, 0.0f, -34.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M15.RenderModel();
		// Nave estacionada
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(55.0f, 0.0f, -45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M16.RenderModel();
		// Coches 1 (M17)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, 55.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M17.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 55.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M17.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 55.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M17.RenderModel();
		// Botes de basura
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-88.09f, 0.0f, -72.72f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M18.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-87.15f, 0.0f, 19.36f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M18.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(86.06f, 0.0f, -18.43f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M18.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(87.51f, 0.0f, 73.59f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M18.RenderModel();
		// Bancas (1 a 18)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-89.0f, 0.0f, -22.65f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-89.0f, 0.0f, -40.17f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-41.42f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-68.03f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.45f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(13.84f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(67.01f, 0.0f, -80.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(86.96f, 0.0f, -69.64f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(86.96f, 0.0f, -51.81f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		/*
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(88.41f, 0.0f, 22.38f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(88.41f, 0.0f, 40.21f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		*/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-88.05f, 0.0f, 69.43f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-88.05f, 0.0f, 51.91f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(41.35f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(67.95f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-40.54f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-13.93f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-67.09f, 0.0f, 80.71f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M19.RenderModel();
		// === DECORACION ===
		// Librería Aire libre
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-60.0f, 0.0f, -25.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M25.RenderModel();
		// Pinos (M26)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 0.0f, -70.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f, 0.0f, -55.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-75.0f, 0.0f, 70.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-175.0f, 0.0f, -105.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-170.0f, 0.0f, -10.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-35.0f, 0.0f, -55.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(165.0f, 0.0f, 45.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 0.0f, 35.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();
		// Coches 2 (M31)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 55.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-55.0f, 0.0f, 55.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, -62.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -62.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M31.RenderModel();
		// Cerezos (M32)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-60.0f, 0.0f, 45.0f));
		model = glm::rotate(model, 120 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(155.0f, 0.0f, -20.0f));
		model = glm::rotate(model, -75 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, -25.0f));
		model = glm::rotate(model, 120 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-90.0f, 0.0f, 20.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, 0.0f, 75.0f));
		model = glm::rotate(model, 105 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-80.0f, 0.0f, -75.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, 0.0f, 120.0f));
		model = glm::rotate(model, 105 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M32.RenderModel();
		// Señalizacion (M33_1)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(110.0f, 0.0f, 15.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M33_1.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(130.0f, 0.0f, 15.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M33_1.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-130.0f, 0.0f, -16.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M33_1.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-110.0f, 0.0f, -16.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M33_1.RenderModel();
		// Señalizacion (M33_2)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-140.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M33_2.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(140.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M33_2.RenderModel();
		// Carteles
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-50.0f, 0.0f, -75.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M34.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 25.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M34.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35.0f, 0.0f, -30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M34.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(27.0f, 0.0f, -83.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M34.RenderModel();
		// ====================================================================================
		// === MODELO M35 (INTERCAMBIO PARA COCINADO DE LUCES) ===
		// ====================================================================================
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-200.0f, 0.0f, -20.0f));
		model = glm::rotate(model, 312 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		// Usamos la misma condición de las luces puntuales (0=Amanecer, 5=Atardecer, 6 y 7=Noche)
		if (indiceSkyboxActual == 0 || indiceSkyboxActual == 5 ||
			indiceSkyboxActual == 6 || indiceSkyboxActual == 7)
		{
			// Luces ON: Renderizamos la versión con el cocinado de luz (M35_2)
			M35_2.RenderModel();
		}
		else
		{
			// Luces OFF (Día pleno): Renderizamos la versión apagada/normal (M35_1)
			M35_1.RenderModel();
		}

		// === RUNATERRA NPC's ===
		// Ziggs (LOL_00)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-111.0, 0.0f, -34.19f));
		model = glm::rotate(model, 137 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_00.RenderModel();
		// Poros (LOL_01)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-89.01f, 0.96f, -41.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-49.56f, 0.91f, -33.35f));
		model = glm::rotate(model, 13 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(85.9f, 2.37f, -18.68f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// === Libro que esta leyendo el poro===
		// 1. Libro Base (M36_1) - Estático
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-51.8f, 1.5f, -33.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M36_1.RenderModel();
		// 2. Página Animada (M36_2)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-51.8f, 1.5f, -33.0f));
		model = glm::rotate(model, rotacionPagina * toRadians, glm::vec3(-1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M36_2.RenderModel();
		// === PLANO DE LITERATURA ANIMADO ===
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-49.56f, 5.0f, litPosZ));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(litEscalaX, litEscalaY, litEscalaZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		texturaLiteratura.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		// === POROS AHRI (MULTITUD EN CONCIERTO) ===
		// Poro 1 (Índice 0)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-135.0f, alturasPoros[0], -49.0f));
		model = glm::rotate(model, -25 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poro 2 (Índice 1)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-132.0f, alturasPoros[1], -50.0f));
		model = glm::rotate(model, -15 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poro 3 (Índice 2)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-134.0f, alturasPoros[2], -52.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poro 4 (Índice 3)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-132.0f, alturasPoros[3], -54.0f));
		model = glm::rotate(model, 15 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();
		// Poro 5 (Índice 4)
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-135.0f, alturasPoros[4], -55.0f));
		model = glm::rotate(model, 25 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_01.RenderModel();

		// Anivia (LOL_02)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-26.0f, 22.0f, -42.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_02.RenderModel();
		// Pinguim (LOL_04)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-90.0f, 0.0f, -76.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-7.73f, 0.0f, -82.43f));
		model = glm::rotate(model, -211 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-142.85f, 0.0f, -100.48f));
		model = glm::rotate(model, -33 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-105.0f, 0.0f, -40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-105.0f, 0.0f, -45.0f));
		model = glm::rotate(model, 18 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-105.0f, 0.0f, -50.0f));
		model = glm::rotate(model, -13 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_04.RenderModel();
		// Escurridizo (LOL_05)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-120.0f, 0.0f, -29.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_05.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-92.0f, 0.0f, -112.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_05.RenderModel();
		// Xayah (LOL_11)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-65.0f, 0.0f, -75.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_11.RenderModel();
		// Annie (LOL_13)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-70.0f, 0.0f, -40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_13.RenderModel();
		// Tristana (LOL_14)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-125.0f, 0.0f, -75.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_14.RenderModel();
		// Montura Sejuani (LOL_15)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-125.0f, 0.0f, -90.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_15.RenderModel();
		// === DECORACION 2 RUNATERRA ===
		// Torreta (LOL_03)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-138.0f, 0.0f, -70.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_03.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-100.0f, 0.0f, -70.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_03.RenderModel();
		// Ward (LOL_06)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-89.0f, 0.0f, -47.0f));
		model = glm::rotate(model, 61 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_06.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-140.0f, 0.0f, -19.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_06.RenderModel();
		// Maestría (LOL_07)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-104.42f, 9.65f, -34.16f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_07.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-107.71f, 9.65f, -30.91f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_07.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-104.42f, 9.65f, -27.65f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_07.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-101.35f, 9.65f, -30.91f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_07.RenderModel();
		// Martillo Jayce (LOL_08)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-114.0f, 0.0f, -25.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_08.RenderModel();
		// Estatua Ahri (LOL_09)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-140.0f, 1.15f, -52.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_09.RenderModel();
		// Base Ahri (LOL_10)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-140.0f, 0.0f, -52.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_10.RenderModel();
		// Mazo Poppy (LOL_16)
		model = glm::mat4(1.0);
		model = glm::translate(glm::mat4(1.0), glm::vec3(-114.0f, 0.0f, -25.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LOL_16.RenderModel();


		// >>>>>>>>>>>>>>>>>>>>>>> TEXTURAS Y MODELOS - JUAN PABLO >>>>>>>>>>>>>>>>>>>>>>>
		// === INSTANCIA HW - CUERPO (EL PADRE) ===
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(cuerpoPosX, 1.0f, cuerpoPosZ));
		model = glm::rotate(model, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, cuerpoRotY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelCuerpo = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hw_cuerpo.RenderModel();
		// === INSTANCIA HW - CABEZA (HIJO DEL CUERPO) ===
		model = modelCuerpo; // Partimos de la base del cuerpo
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_cabeza.RenderModel();
		// === INSTANCIA HW - ESPADA (HIJO DEL CUERPO) ===
		model = modelCuerpo; // Empezamos desde la posici n del cuerpo
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -0.65f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_espada.RenderModel();
		// === PIERNA DERECHA (Avanza 15 ) ===
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(0.18f, -0.6f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Tu rotaci n base
		model = glm::rotate(model, rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // +35 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_PiernaDerecha.RenderModel();
		// === PIERNA IZQUIERDA (Retrocede -15 ) ===
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-0.18f, -0.6f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // -35 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_PiernaIzquierda.RenderModel();
		// === BRAZO DERECHO (Retrocede con pierna derecha) ===
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(0.37f, 0.2f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // -35 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_BrazoDerecho.RenderModel();
		// === BRAZO IZQUIERDO (Avanza con pierna derecha) ===
		model = modelCuerpo;
		model = glm::translate(model, glm::vec3(-0.37f, 0.2f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotacionAvatar * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // +35 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_BrazoIzquierdo.RenderModel();
		// === INSTANCIAS DE HW ===
		// Banca
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(120.0f, 0.0f, -25.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_banca.RenderModel();
		// === Librerias 1-19  ===
		// INSTANCIA: Libreria 1
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -25.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 2
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -31.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 3
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -37.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 4
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -43.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 5
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -49.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 6
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -54.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 7
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(139.0f, 0.0f, -60.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 8
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -31.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  8
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -25.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  9
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -31.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  10
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -37.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  11
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -43.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  12
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -49.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  13
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -54.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria  14
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(105.0f, 0.0f, -60.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 15
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(109.0f, 0.0f, -20.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 16
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(115.0f, 0.0f, -20.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 17
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(121.0f, 0.0f, -20.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 18
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(128.0f, 0.0f, -20.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// INSTANCIA: Libreria 19
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(134.0f, 0.0f, -20.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_libros.RenderModel();
		// === NPC HW ===
		//Hornet		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(130.0f, 0.0f, -25.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f)); // Escala inicial
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hornet.RenderModel();
		//Cornifer
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(110.0f, 0.0f, -30.0f));
		model = glm::rotate(model, 110 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cornifer.RenderModel();
		//Sherma
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(135.0f, -0.5f, -54.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // Escala inicial
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_sherma.RenderModel();
		//Marrissa
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(110.0f, -0.5f, -54.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f)); // Escala inicial
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hw_marissa.RenderModel();




		// >>>>>>>>>>>>>>>>>>>>>>>>> TEXTURAS Y MODELOS - TONY >>>>>>>>>>>>>>>>>>>>>>>>>>>
		// ---------------------- Elementos del escenario de Miku --------------------------------
	   // -- NODO PADRE: Stage ----------------------------------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-120.0f, 0.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Stage.RenderModel();
		modelaux = model;
		// -- HIJOS: personajes y props ---------------------------------------------
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Miku.RenderModel();
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Rin.RenderModel();
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Len.RenderModel();
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luka.RenderModel();
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Piano.RenderModel();
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Instruments.RenderModel();
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Magnet.RenderModel();
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Leeks.RenderModel();
		model = modelaux;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Chairs.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(20.0f, 0.0f, 25.0f));  // ajustar
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Scene.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(20.0f, 0.0f, 40.0f));  // ajustar
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Scene.RenderModel();
		// -- HIJOS: Lámparas -----------
		// Lámpara 1 – hijo de Stage, posicionada a la izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-25.0f, 0.0f, 3.0f));  // ajusta a tu escenario
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lamparaTony.RenderModel();
		// Lámpara 2 – hijo de Stage, posicionada a la derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(25.0f, 0.0f, 3.0f));   // ajusta a tu escenario
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lamparaTony.RenderModel();
		// ------------------------- Reloj animacion Keyframse -----------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.3f, 19.0f, -27.5f));   // posición del reloj
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GearBase.RenderModel();
		// Captura el transform del padre ANTES de añadir rotaciones de hijos
		modelaux = model;
		// -- HIJO: Gear1 ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, 0.0f));
		model = glm::rotate(model, rotGear1 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Gear1.RenderModel();
		// -- HIJO: Gear2 ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.0f, -0.5f, 0.0f));
		model = glm::rotate(model, rotGear2 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Gear2.RenderModel();
		// -- HIJO: Gear3 ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f));
		model = glm::rotate(model, rotGear3 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Gear3.RenderModel();
		// -- HIJO: GearHora ---
		model = modelaux;
		model = glm::rotate(model, rotGearHor * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GearHora.RenderModel();
		// -- HIJO: GearMinuto ---
		model = modelaux;
		model = glm::rotate(model, rotGearMin * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GearMinuto.RenderModel();
		// ----------------------  Reloj animacion Basica -------------------------------------
		model = glm::mat4(1.0f);
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 16.0f, -24.0f));   // posición del reloj
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GearBase.RenderModel();
		modelaux = model; // Captura la base del segundo reloj
		// -- HIJO: Gear1 (Reloj 2) --
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.2f, 0.0f));
		model = glm::rotate(model, autoGearBackRotation * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Gear1.RenderModel();
		// -- HIJO: Gear2 ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.0f, -0.5f, 0.0f));
		model = glm::rotate(model, -autoGearBackRotation * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Gear2.RenderModel();
		// -- HIJO: Gear3 ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f));
		model = glm::rotate(model, autoGearBackRotation * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Gear3.RenderModel();
		// -- HIJO: GearHora (Reloj 2) --
		model = modelaux;
		model = glm::rotate(model, rotAutoGearHora * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GearHora.RenderModel();
		// -- HIJO: GearMinuto (Reloj 2) --
		model = modelaux;
		model = glm::rotate(model, rotAutoGearMinuto * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GearMinuto.RenderModel();
		// -------------------  CuckooBase ------------------------------------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(20.0f, -3.0f, -15.0f)); // posición en la escena
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CuckooBase.RenderModel();
		modelaux = model;
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 7.0f, 0.0f));
		model = glm::rotate(model, ck_rotPendulo * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuckoo1.RenderModel();
		// -- HIJO: Cuckoo2 – pájaro, inclinación en X (picoteo) ---
		model = modelaux;
		model = glm::rotate(model, ck_rotPajaro * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuckoo2.RenderModel();
		// -- HIJO: Cuckoo3 – cadena izquierda, traslación en Y ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, ck_trasCad3, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuckoo3.RenderModel();
		// -- HIJO: Cuckoo4 – cadena derecha, traslación en Y (opuesta) ---
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, ck_trasCad4, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuckoo4.RenderModel();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -15.0f)); // posición en la escena
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Teto.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Library3.RenderModel();
		//elementos literarios escenario 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(25.0f, 0.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Globo.RenderModel();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(55.0f, 0.0f, -65.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Library.RenderModel();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(25.0f, 0.0f, 40.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Library2.RenderModel();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(40.0f, 0.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Library4.RenderModel();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(43.0f, 0.0f, 27.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LibraryE.RenderModel();

		

		// >>>>>>>>>>>>>>>>>>>>>>>>> TEXTURAS Y MODELOS - RUBEN >>>>>>>>>>>>>>>>>>>>>>>>>>
		//DJ Spit - Bordes escenario
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(114.5f, 0.0f, 49.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		DJSpit_Paredes_M.RenderModel();
		//DJ Spit - Escenario principal
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(114.5f, 0.0f, 49.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		DJSpit_Escenario_M.RenderModel();
		//DJ Spit - Basura
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(114.5f, 0.0f, 49.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		DJSpit_Basura_M.RenderModel();
		//DJ Spit - Lata
		model = glm::mat4(1.0);//119.5f, 0.0f, 46.5f     model = glm::translate(model, glm::vec3(-12.6f, lataPosY, -3.0f + lataPosZ));
		model = glm::translate(model, glm::vec3(121.5f, lataPosY, 49.0f + lataPosZ));
		model = glm::rotate(model, rotLata * toRadians, glm::vec3(1.0f, 0.5f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DJSpit_Lata_M.RenderModel();
		///////////////////////////Para la animación de la esfera///////////////////////////////
		model = glm::mat4(1.0f);
		//model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
		model = glm::translate(model, glm::vec3(-250.0f + movEsferaX, 10.0f, 95.0f + movEsferaZ));
		model = glm::rotate(model, rotEsferaZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, rotEsferaY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		DJSpit_Oficina_M.RenderModel();
		////////////////////////////////////////////////////////////////////////////////////////
		//Pino caído
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(195.0f, 0.0f, 95.0f));
		//model = glm::rotate(model, -80 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, -rotArbol * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		M26.RenderModel();

		// ===============================================================================
		// --- 8.7: UBICAR AL OYENTE Y ALTERNAR NIVELES DE VOLUMEN
		// ===============================================================================
		if (SoundEngine) {
			// Obtenemos posición y dirección de tu clase Camera
			camPos = camera.getCameraPosition();
			camDir = camera.getCameraDirection();
			SoundEngine->setListenerPosition(
				vec3df(camPos.x, camPos.y, camPos.z), // Posición
				vec3df(camDir.x, camDir.y, camDir.z)  // Hacia dónde mira
			);
			// 2. Calculamos las distancias geométricas sobre las variables existentes
			distanciaHollow = glm::distance(camPos, posHollow);
			distanciaZiggs = glm::distance(camPos, posZiggs);
			distanciaMiku = glm::distance(camPos, posMiku);
			distanciaSmiling = glm::distance(camPos, posSmiling);
			distanciaArbol = glm::distance(camPos, posArbol);
			// 3. Máquina de estados de proximidad para el volumen
			if (distanciaHollow < umbralSonido || distanciaZiggs < umbralSonido || distanciaMiku < umbralSonido || distanciaSmiling < umbralSonido) {
				// Jugador cerca de un emisor 3D: Atenuamos la música ambiental 2D
				if (ruidoAmbiente) {
					ruidoAmbiente->setVolume(0.05f);
				}
				if (musicaFondo) {
					musicaFondo->setVolume(0.03f);
				}
			}
			else {
				// Jugador en zona neutral: La música 2D vuelve a su nivel normal
				if (ruidoAmbiente) {
					ruidoAmbiente->setVolume(0.09f);
				}
				if (musicaFondo) {
					musicaFondo->setVolume(0.07f);
				}
			}
			if (distanciaArbol < 60.0f && movEsferaX > 375.0f) {
				// Jugador observando de cerca la esfera a punto de chocar con el árbol: Atenuamos la música ambiental 2D
				if (ruidoAmbiente) {
					ruidoAmbiente->setVolume(0.05f);
				}
				if (musicaFondo) {
					musicaFondo->setVolume(0.03f);
				}
			}
		}




		// ====================================================================================
		// FASE 4: ELEMENTOS CON TRANSPARENCIA (BLENDING) Y TEXTURAS ANIMADAS
		// ====================================================================================
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
		// === HUMO DE LOCOMOTORAS===
		glm::vec3 colorHumo = glm::vec3(2.0f, 2.0f, 2.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorHumo));
		texturaHumo.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		// 1. HUMO TREN CHICO
		model = matrizHumoChico;
		model = glm::translate(model, glm::vec3(15.75f, 16.0f + offsetYHumo, 0.0f));
		model = glm::scale(model, glm::vec3(escalaHumoAnimada, escalaHumoAnimada, escalaHumoAnimada));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[3]->RenderMesh();
		// 2. HUMO TREN LARGO
		model = matrizHumoLargo;
		model = glm::translate(model, glm::vec3(15.75f, 16.0f + offsetYHumo, 0.0f));
		model = glm::scale(model, glm::vec3(escalaHumoAnimada, escalaHumoAnimada, escalaHumoAnimada));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[3]->RenderMesh();
		// LIMPIEZA
		glm::vec3 colorOriginal = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(colorOriginal));


		// === POROS CORAZONES ===
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-134.0f, 7.0f + offsetYHumo, -52.0f));
		model = glm::scale(model, glm::vec3(escalaHumoAnimada, escalaHumoAnimada, escalaHumoAnimada));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		texturaPoro.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		

		glDisable(GL_BLEND);
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}

