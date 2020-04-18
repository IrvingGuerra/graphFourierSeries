#include "gfx.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdlib.h> 


#include <thread> 
#include <math.h>

#include <iostream>
using namespace std;

#define WindowsX 800
#define WindowsY 600
#define X WindowsX/2
#define Y WindowsY/2
#define PI 3.14159265

int puerto = 5000;
double sfX[6284];
double sfY[6284];

void setRed(){
   gfx_color(255,0,0);
}
void setGreen(){
   gfx_color(0,255,0);
}
void setBlue(){
   gfx_color(0,0,255);
}
void setBlack(){
   gfx_color(0,0,0);
}
void setWhite(){
   gfx_color(255,255,255);
}

void setWindow(){
   gfx_open(WindowsX, WindowsY, "Series de Fourier");
   gfx_clear_color(255,255,255);
   gfx_clear();
   setBlack();
   int x,y;
   for(y=0;y<WindowsY;y++)
      gfx_point(X,y);
   for(x=0;x<WindowsX;x++)
      gfx_point(x,Y);
   gfx_flush();
}

void printFn(double x[], double y[], int size){
   //Graficamos primer periodo
   for(int i = 0 ; i < size; i++){
      gfx_point((x[i]*50)+X - (2*PI*50),(y[i]*50)+Y);
      i++;
   } 
   //Graficamos segundo periodo
   for(int i = 0 ; i < size; i++){
      gfx_point((x[i]*50)+X,(y[i]*50)+Y);
      i++;
   } 
   //Graficamos tercer periodo
   for(int i = 0 ; i < size; i++){
      gfx_point((x[i]*50)+X + (2*PI*50),(y[i]*50)+Y);
      i++;
   } 
}

void hiloPinta(int s, sockaddr_in msg_to_client_addr, int clilen){
   int res;
   while(1){
      res = 2;
      std::cout << "[ SEND ] Pidiendo Fourier parte X" << std::endl;
      sendto(s, (char *)&res, sizeof(int), 0, (struct sockaddr *)&msg_to_client_addr, clilen);
      recvfrom(s, (char *) sfX, sizeof(sfX), 0, (struct sockaddr *)&msg_to_client_addr, (socklen_t *)&clilen);
      std::cout << "[ OK ] Se recibieron X" << std::endl;
      res = 3;
      std::cout << "[ SEND ] Pidiendo Fourier parte Y" << std::endl;
      sendto(s, (char *)&res, sizeof(int), 0, (struct sockaddr *)&msg_to_client_addr, clilen);
      recvfrom(s, (char *) sfY, sizeof(sfY), 0, (struct sockaddr *)&msg_to_client_addr, (socklen_t *)&clilen);
      std::cout << "[ OK ] Se recibieron Y" << std::endl;
      std::cout << "[ PAINTING ] Dibujando Fourier" << std::endl;
      setRed();
      printFn(sfX, sfY, sizeof(sfX)/sizeof(*sfX));
      gfx_flush();
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
   }
}

void hiloBorra(int s, sockaddr_in msg_to_client_addr, int clilen){
   while(1){
      //Solo borraremos despues de 5 segundos
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      setWhite();
      printFn(sfX, sfY, sizeof(sfX)/sizeof(*sfX));
      gfx_flush();
   }
}

int main(void){
   setWindow();
   double fnX[629];
   double fnY[629];
   int s, res, clilen;
   struct sockaddr_in server_addr, msg_to_client_addr;
   s = socket(AF_INET, SOCK_DGRAM, 0);

   bzero((char *)&server_addr, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = INADDR_ANY;
   server_addr.sin_port = htons(puerto);
   ::bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr));
   clilen = sizeof(msg_to_client_addr);
   
   std::cout << "[ OK ] Esperando fx parte X" << std::endl;
   recvfrom(s, (char *) fnX, sizeof(fnX), 0, (struct sockaddr *)&msg_to_client_addr, (socklen_t *)&clilen);
   res = 1;
   std::cout << "[ SEND ] Confirmando fx parte X" << std::endl;
   sendto(s, (char *)&res, sizeof(int), 0, (struct sockaddr *)&msg_to_client_addr, clilen);
   std::cout << "[ OK ] Esperando fx parte Y" << std::endl;
   recvfrom(s, (char *) fnY, sizeof(fnY), 0, (struct sockaddr *)&msg_to_client_addr, (socklen_t *)&clilen);
   std::cout << "[ PAINTING ] Dibujando fx" << std::endl;
   setBlue();
   printFn(fnX, fnY, sizeof(fnX)/sizeof(*fnX));

   while(1) {
      thread pintar;
      thread borrar;
      pintar = std::thread(hiloPinta, s, msg_to_client_addr, clilen);
      borrar = std::thread(hiloBorra, s, msg_to_client_addr, clilen);
      pintar.join();
      borrar.join();
   }
   return 0;
}



