#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include "glm/glm.hpp"
#include <QKeyEvent>
#include <QMouseEvent>
#include "glm/gtc/matrix_transform.hpp"


class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();
    
  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ();

    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ();
 
    // resize - Es cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);  

    //keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    

  private:
    void creaBuffers ();
    void pintaEscena ();
    void carregaShaders ();
    void modelTransform(); //aquesta és la funció que construeix la TG i l'envia al shader
    void modelTransform2(); //aquesta és la funció que construeix la TG del triangle

    // attribute locations
    GLuint vertexLoc;
    GLuint colorLoc;
    
    // Program
    QOpenGLShaderProgram *program;

    GLuint VAO1;
    GLuint VAO2;
    GLint ample, alt;

    float scl, sx, sy; // el VALOR que tindrà el uniform (viu al C++) --> número que després canviarem amb les tecles
    int xAnt, yAnt; //posició anterior del ratolí, en píxels
    //GLuint varLoc; //l'"ADREÇA" del uniform dins del shader
    //és l'adreça per arribar al val del shader. En C++ no podem escriure directament dins la GPU així que es necessita l'adreça
    GLint transloc; // adreça del uniform TG
    float tx,ty,tz; //construeix la TG i l'envia al shader
    float angle1, angle2;


  };                
