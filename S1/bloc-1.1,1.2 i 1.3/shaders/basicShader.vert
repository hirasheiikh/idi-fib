#version 330 core

in vec3 vertex; //diferent per a cada vèrtex
in vec3 color;

out vec3 fcolor;
//uniform float val; //per multiplicar les coordenades
                   //és el mateix per a tots els vèrtex
uniform mat4 TG;   //és la matriu de transformació, TG = Transformació Geomètrica

void main()  {
    fcolor = color;
    gl_Position = TG*vec4 (vertex, 1.0);
    //vertex*val multiplica les tres coordenades (x,y,z) pel valor
    //si val = 0,5 el triangle surt a la meitat de la mida
    
}
