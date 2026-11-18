// Posem els fitxers de cap¸calera necessaris
// Permet declarar una QApplication
#include <QApplication>
// Permet declarar un QPushButton
#include <QPushButton>
// Programa principal
int main(int argc, char ** argv )
{
// Aplicaci´o en Qt que cont´e la meva interf´ıcie
QApplication a(argc, argv);
// Bot´o amb el text “Hello Qt!” que ser`a
// la meva interf´ıcie
QPushButton hello("Hello Qt!", 0);
// M`etode per determinar la mida del bot´o
hello.resize( 150, 30 );
// Mostro el bot´o
hello.show();
// El meu programa (main) executa el m`etode que
// posa en marxa l’aplicaci´o “a” de Qt (exec) i
// retorna el valor que generi en acabar
return a.exec();
}