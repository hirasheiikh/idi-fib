// MyGLWidget.cpp
#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget() {
}

void MyGLWidget::initializeGL ()
{
    BL2GLWidget::initializeGL(); //1r: tot el de l'esquelet (carrega el shader, crea la caseta)
    projectTransform(); //2n: ara que el shader existeix, li enviem la matriu
}
void MyGLWidget::carregaShaders ()
{
    BL2GLWidget::carregaShaders(); //carreguem el shader com sempre, només que ara es troba a BL2GLWidget
    projLoc = glGetUniformLocation(program->programId(), "proj"); // pregunta on es troba "proj"
}
void MyGLWidget::projectTransform ()
{
    //glm::perspective (FOV en radians, ra window, znear, zfar)
    glm::mat4 Proj = glm::perspective(float(M_PI)/2.0f, 1.0f, 0.4f, 3.0f); //calcula la matriu
    /*
    FOV(Field Of View): quant obre l'ull, en vertical. És un angle en radians. M_PI/2 són 90 graus
        -FOV gran és com un gran angular: veus molt i tot es veu petit
        -FOV petit és com un zoom: veus poc i tot es veu gran
    ra(relació d'aspecte): ample / alt de la finestra. La nostra és quadrada, així que ra = 1. 
        Si no coincideix amb la finestra, la imatge es deforma.
    zNear: el que és més aprop d'aquesta distància no es veu. Aquí és 0.4
    zFar: el que és més lluny d'aquesta distància no es veu. Aquí és 3.
    */
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]); //enviem la matriu al shader
}


