#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{ //CONSTRUCTOR
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  scl = 0.5f;
  sx = sy = 1.0f; //al principi no deformem, multipliquem X i Y per 1
  xAnt = yAnt = 0;
  tx = ty = tz = 0.0f; //al principi la caseta no es mou
  angle1 = angle2 = 0.0f;

}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();
  
  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffers();
}

void MyGLWidget::modelTransform()
{

/*
2.
  L'enunciat diu que hem de rotar els vèrtex després de la translació, pero ho hem de posar al revés (és a dir), primer la rotació i després la translació ja que 
    glm::mat4 TG (1.0f);     →  TG = I
    TG = glm::rotate(...);   →  TG = I · R       = R
    TG = glm::translate(...);→  TG = R · T

    Al shader es fa TG * vèrtex = R · T · vèrtex. Les matrius s’apliquen de dreta a esquerra: el vèrtex es multiplica primer per T (es mou) i després per R (gira). 
    Regla pràctica: llegeix les línies de glm de baix a dalt. L’última que escrius és la primera que s’aplica.
  
  glm::mat4 TG (1.0f); //amb aquesta línia creem la matriu identitat (que és la que no fa res) SEMPRE COMENCEM PER AQUÍ
  TG = glm::rotate (TG, float(M_PI/4), glm::vec3 (0,0,1)); //afegeix la rotació a la matriu
  TG = glm::translate (TG, glm::vec3(tx,ty,tz)); //afegeix una translació a la matriu
  //float(M_PI/4) ->  és la rotacio, equival a 45 graus pero glm sempre treballa en radians
  //glm::vec3(0,0,1): l'enunciat diu que respecte  l'eix. Estem en 2D, al pla XY. Girar dins aquest pla és girar al voltant de l’eix Z, el que surt de la pantalla cap a tu.
  
  3.
  glm::mat4 TG (1.0f);
  TG = glm::translate (TG, glm::vec3 (tx, ty, tz));            // s'aplica 2n
  TG = glm::rotate (TG, float(M_PI/4), glm::vec3 (0, 0, 1));   // s'aplica 1r

  TG = I
  TG = I · T        = T
  TG = T · R

  glUniformMatrix4fv(transloc, 1, GL_FALSE, &TG[0][0]); //envia la matriu al shader, és per a una matriu de 4x4 de floats
  */
 
 /*-tranLoc: a quina adreça (el TG del shader)
 -1: quantes matrius envies
 -GL_FALSE: que no la transposi
 -&TG[0][O] on comencen els 16 números de la matriu*/
 
  //4.
  /*glm::mat4 TG (1.0f);
  TG = glm::translate (TG, glm::vec3 (tx, ty, tz));            // s'aplica tercer (moure)
  TG = glm::rotate (TG, float(M_PI/4), glm::vec3 (0, 0, 1));   // s'aplica segon (rotar)
  TG = glm::scale(TG, glm::vec3 (scl,scl,1.0)); //s'aplica primer (escalar)
 glUniformMatrix4fv(transloc, 1, GL_FALSE, &TG[0][0]); //envia la matriu al shader, és per a una matriu de 4x4 de floats
 */

 //5.
  glm::mat4 TG (1.0f);
  TG = glm::translate (TG, glm::vec3 (tx, ty, tz));
  TG = glm::rotate (TG, angle1, glm::vec3 (0, 0, 1));
  TG = glm::scale (TG, glm::vec3 (scl * sx, scl * sy, 1.0));   // ← CANVIAT
  glUniformMatrix4fv (transloc, 1, GL_FALSE, &TG[0][0]);


}

void MyGLWidget::modelTransform2 ()
{
 /* glm::mat4 TG (1.0f); //matriu identitat (no fa res)
  TG = glm::rotate(TG,angle2, glm::vec3(0,0,1));
  glUniformMatrix4fv(transloc, 1, GL_FALSE, &TG[0][0]);//l'envia al shader
  */

  /*Rotate només sap girar al voltant de l'origen per tant, portem el triangle
  a l'origen, el girem i el tornem al seu lloc
  El centre del triangle és la mitjana dels seus 3 vèrtexs
  
  x = (0.6 + 0.95 + 0.775) / 3 = 2.325 / 3 = 0.775
  y = (-0.9 + -0.9 + -0.55) / 3 = -2.35 / 3 ≈ -0.783
  */
 glm::vec3 centre2(0.775, -0.783, 0.0); //centre del triangle

 glm::mat4 TG (1.0f);
 TG = glm::translate (TG,centre2); //3r: torna al seu lloc
 TG = glm::rotate (TG, angle2, glm::vec3(0, 0, 1)); //2n: gira a l'origen
 TG = glm::translate (TG, -centre2); //1r: escalem
 //En notació de teoria, això és TG = T(c) · R · T(-c). girar respecte a un punt qualsevol
 glUniformMatrix4fv (transloc,1,GL_FALSE, &TG[0][0]);
}

void MyGLWidget::paintGL ()
{
// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
// useu els paràmetres que considereu (els que hi ha són els de per defecte)
//  glViewport (0, 0, ample, alt);
  
  glClear (GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer

  modelTransform(); //NOU: envia la TG
  // Activem l'Array a pintar 
  glBindVertexArray(VAO1);
  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, 9);

  modelTransform2();
  glBindVertexArray(VAO2);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  
  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#else
  ample = w;
  alt = h;
#endif
}

void MyGLWidget::keyPressEvent(QKeyEvent *event)
{
  makeCurrent(); //vol dir: activa OpenGL per a aquesta finestra. a paintGL Qt ho fa per tu
  switch(event->key()){  // el switch fa una cosa diferent depenent de la tecla premuda // event->key() diu quina tecla s'ha premut 
      case Qt::Key_S: // es el nom que Qt dona a la tecla S: voler una escala més gran
        scl += 0.1;
        //glUniform1f(varLoc,scl); //donem valor al uniform, cosa que cal fer cada cop que es vulgui canviar el valor de scl
        break;
      case Qt::Key_D: //volem una escala més petita
        scl -= 0.1;
        //glUniform1f(varLoc, scl);
        break;
      case Qt::Key_Left: tx -= 0.1; break; //esquerra: X disminueix
      case Qt::Key_Right: tx += 0.1; break; //dreta: X augmenta
      case Qt::Key_Up: ty += 0.1; break; //amunt: Y augmenta
      case Qt::Key_Down: ty -= 0.1; break; //abaix: Y disminueix
      case Qt::Key_X: sx += 0.1; break; //més ample
      case Qt::Key_C: sx -= 0.1; break; //més estret
      case Qt::Key_Y: sy += 0.1; break; //més alt
      case Qt::Key_U: sy -= 0.1; break; //més baix
      case Qt::Key_P: 
        angle1 += M_PI/6; //obj1: +30 graus (sentit antihorari)
        angle2 -= M_PI/6; //obj2: -30 graus (sentit horari)
        break;
      default:
        event->ignore(); //si es clica una altra tecla s'ignora
        break;
  }
  update(); //estem dient que torni a pintar la figura després dels canvis plantejats. 
            //Qt crida a paintGL (sense això el valor canvia però la pantalla no s'actualitza)
}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
  xAnt = event->x(); //on és el ratolí ara, en X
  yAnt = event->y(); //on és el ratolí ara, en Y
}

void MyGLWidget::mouseMoveEvent (QMouseEvent *event)
{
  makeCurrent();
  int dx = event->x() - xAnt; //(1) quant s'ha mogut en X
  int dy = yAnt - event->y(); //(2) quant s'ha mogut en Y (ha de fer-se al revés)

  sx += dx*0.01; //(3) el ratolí es mou en píxels, en un moviment normal en fas fàcilment 100. 
  sy += dy*0.01; //si sumessis 100 directament a sx la caseta es faria gegant de cop. multiplicant per 0.01, 100 píxels sumen 1.0

  xAnt = event->x(); //la posició actual passa a ser l'anterior
  yAnt = event->y();

  update(); //tornaa pintar perquè es vegi el canvi





}

void MyGLWidget::creaBuffers ()
{
  //---------OBJECTE 1: CASETA-----------
  glm::vec3 Vertices[9];  // x vèrtexs amb X, Y i Z

  //MEITAT INFERIOR
  Vertices[0] = glm::vec3(-0.5, -0.5, 0.0); //esq
  Vertices[1] = glm::vec3(0.5, -0.5, 0.0); //dre
  Vertices[2] = glm::vec3(0.5, 0.5, 0.0); //adalt

  //MEITAT SUPERIOR
  Vertices[3] = glm::vec3(-0.5, -0.5, 0.0); //abaix
  Vertices[4] = glm::vec3(-0.5, 0.5, 0.0); //esq
  Vertices[5] = glm::vec3(0.5, 0.5, 0.0); //dre

  //TRIANGLE SUPERIOR (caseta)
  Vertices[6] = glm::vec3(0.0, 1.0, 0.0); //adalt
  Vertices[7] = glm::vec3(-0.5, 0.5, 0.0); //esq
  Vertices[8] = glm::vec3(0.5, 0.5, 0.0); //dre
  

  glm::vec3 Colors[9]; // x vèrtexs (en aquest cas, 9, amb X, Y i Z)

  //quadrat (6 vèrtexs): vermell,verd,blau...
  Colors[0] = glm::vec3(1.0, 0.0, 0.0); //vermell
  Colors[1] = glm::vec3(0.0, 1.0, 0.0); //verd
  Colors[2] = glm::vec3(0.0, 0.0, 0.0); //blau
  Colors[3] = glm::vec3(1.0, 0.0, 0.0); //vermell
  Colors[4] = glm::vec3(0.0, 1.0, 0.0); //verd
  Colors[5] = glm::vec3(0.0, 0.0, 0.0); //blau

  //sostre (3 vèrtexs)
  Colors[6] = glm::vec3(1.0, 1.0, 0.0); //groc
  Colors[7] = glm::vec3(1.0, 0.0, 1.0); //magenta
  Colors[8] = glm::vec3(0.0, 1.0, 1.0); //cian

  // Creació del Vertex Array Object (VAO) que usarem per pintar
  glGenVertexArrays(1, &VAO1);
  glBindVertexArray(VAO1);

  // Creació del buffer amb les dades dels vèrtexs
  GLuint VBO1;
  glGenBuffers(1, &VBO1);
  glBindBuffer(GL_ARRAY_BUFFER, VBO1);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex (només el 0 en aquest cas)	
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);


  //Creació del buffer amb els colors dels vèrtexs
  GLuint VBO2;
  glGenBuffers(1, &VBO2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO2);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);


  // Desactivem el VAO
  glBindVertexArray(0);

  // ------Objecte 2: UN TRIANGLE--------
  glm::vec3 Vertices2[3];
  Vertices2[0] = glm::vec3(0.6, -0.9, 0.0); //abaix esquerra
  Vertices2[1] = glm::vec3(0.95, -0.9, 0.0); //abaix dreta
  Vertices2[2] = glm::vec3(0.775, -0.55, 0.0); //a dalt

  glm::vec3 Colors2[3];
  Colors2[0] = glm::vec3(1.0, 0.0, 0.0); //vermell
  Colors2[1] = glm::vec3(0.0, 1.0, 0.0); //verd
  Colors2[2] = glm::vec3(0.0, 0.0, 1.0); //blau

  glGenVertexArrays(1, &VAO2);
  glBindVertexArray(VAO2);

  GLuint VBO3;
  glGenBuffers(1, &VBO3);
  glBindBuffer(GL_ARRAY_BUFFER, VBO3);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2), Vertices2, GL_STATIC_DRAW);
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0); 
  glEnableVertexAttribArray(vertexLoc);

  GLuint VBO4;
  glGenBuffers(1, &VBO4);
  glBindBuffer(GL_ARRAY_BUFFER, VBO4);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors2), Colors2, GL_STATIC_DRAW);
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray(0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/basicShader.frag");
  vs.compileSourceFile("shaders/basicShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();


  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  //com vertex és un int fem servir glGetAttribLLocation
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");

  //Obtenim identificador per l'atribut "color" del vertex shader, no posem uniform perquè color és un in
  colorLoc = glGetAttribLocation (program->programId(), "color");
  
  //obtenim l'adreça de l'uniform "val" del vertex shader
  //aquesta línia busca dins el shader la variable que es diu "val" i guarda l'adreça a varLoc
  //com val és un uniform fem servir glGetUniformLocation
  //varLoc = glGetUniformLocation(program->programId(), "val");

  //li donem el valor inicial (scl = 0.5). aquí estem dient: a l'adreça varLoc posa-hi el valor de scl (1f = 1 float)
  //glUniform1f(varLoc, scl);
  //obtenim l'adreça de l'uniform "TG" del vertex shader
  //aquesta línia busca dins el shader la variable que es diu "TG" i guarda l'adreça a transLoc
  //com TG és un uniform fem servir glGetUniformLocation
  transloc = glGetUniformLocation(program->programId(), "TG"); 
}
