/******************************************************************************/
/*                                                                            */
/*     monome par Antoine Letouzey  ~ Guyver2                                 */
/*  antoine.letouzey@gmail.com   ---   https://antoine.sxbn.org               */
/*                                                                            */
/******************************************************************************/


// TODO sauvegarde, effets lumineux et portage
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Pgck/Pgck.h"
#include "Pgck/Pson.h"

#include "struct.h"

#define COL 12

enum {ETAT_PAUSE, ETAT_PLAY, ETAT_TRANS};

int etat = ETAT_PAUSE;

int tab[16][COL];

couple sel;

void InitialiseGame();

//---------- PAUSE


void initPause()
{
 int i = 0;
 for(i=0;i<16;i++)
  {
   int j = 0;
   for(j=0; j<COL; j++) tab[i][j] = 0;
  }
  sel.x = 0;
  sel.y = 0;
/*  tempo = 60;*/
  etat = ETAT_PLAY;
}

//#################### LECTURE
long timer[16];
long timerCourant;
long timerCircle;
long timerSquare;
int tempo[16] = {60, 60, 60, 60, 60, 60, 60, 60,60, 60, 60, 60, 60, 60, 60, 60};// en bpm
int trace[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int idtab = 0;



void affLecture() {
 int i = 0;
 // affichage
 for(i=0;i<8;i++)
  {
   int j = 0;
   for(j=0; j<COL; j++)
    {
     if (tab[i+idtab*8][j]) PPrintImage(8 + j*33, 6 + i*32, 1);
    }
  }
 for (i=0; i<8; i++) {
  int dec = 32 * ((double) (timerCourant - timer[i+idtab*8]) / ((double)60000/(double)tempo[i+idtab*8]));
  PPrintImage(trace[i+idtab*8]*33-65+dec, 6+i*32, 13);
 }
 PPrintImage(sel.x*33-14, sel.y*32-15, 2); // select

 PPrintImage(446, 32*idtab, 14);
 //PPrintImage(0,0,0);
 for (i=0; i<8; i++) PPrintNb(428, i*32 + 9, tempo[i+idtab*8]);
 PPrintImage(405, 13+sel.y*32, 15); // select
 
}



void initLecture(){
timerCourant = PTime();
int i;
for(i=0; i<16; i++) timer[i] = timerCourant;
}


void lecture(){
 if (PGet(Cross)) tab[sel.y+idtab*8][sel.x] = !tab[sel.y+idtab*8][sel.x];
 PBloqueTouche(Cross);
 if (PGet(Triangle)) // reinitialisation
  { // vidage du tableau
  int binf = 0;
  int bsup = 0;
  if(PGet(R)) {
	binf = 0;
	bsup = 16;
  }
  else {
  binf = idtab*8;
  bsup = binf+8;
  }
   int i = 0;
   // raz des tempo / timers / traces
   timerCourant = PTime();
   for(i=binf;i<bsup;i++)
   {
    int j = 0;
    for(j=0; j<COL; j++) tab[i][j] = 0;
    tempo[i] = 60;
    timer[i] = timerCourant;
    trace[i] = 0;
   }
  }
 PBloqueTouche(Triangle);
 //------------------------
 //----- augmentation du tempo
if(PGet(Circle)) {
 if(timerCircle == 0) {
  if(PGet(R)) tempo[sel.y+idtab*8]++;
  else {
   int i=0;
   for(i=0; i<8; i++) tempo[i+idtab*8]++;
  }
  timerCircle = PTime();
 }
 else if(PTime() - timerCircle > 500) {
  if(PGet(R)) tempo[sel.y+idtab*8]++;
  else {
   int i=0;
   for(i=0; i<8; i++) tempo[i+idtab*8]++;
  }
 }
}
else timerCircle = 0;
 // diminution du tempo
if(PGet(Square)) {
 if(timerSquare == 0) {
  if(PGet(R)) tempo[sel.y+idtab*8]--;
  else {
   int i=0;
   for(i=0; i<8; i++) tempo[i+idtab*8]--;
  }
  timerSquare = PTime();
 }
 else if(PTime() - timerSquare > 500) {
  if(PGet(R)) tempo[sel.y+idtab*8]--;
  else {
   int i=0;
   for(i=0; i<8; i++) tempo[i+idtab*8]--;
  }
 }
}
else timerSquare = 0;
int i;
for (i=0; i<16; i++){
 if(tempo[i]>600) tempo[i] = 600;
 if(tempo[i]<6) tempo[i] = 6;
}
//----------------------------------
//-------  syncrho des tempo
if (PGet(R)) {
 if(PGet(Up) && sel.y > 0) {
 trace[sel.y+idtab*8] = trace[sel.y-1+idtab*8];
 tempo[sel.y+idtab*8] = tempo[sel.y-1+idtab*8];
 timer[sel.y+idtab*8] = timer[sel.y-1+idtab*8];
 }

 if(PGet(Down) && sel.y < 7) {
 trace[sel.y+idtab*8] = trace[sel.y+1+idtab*8];
 tempo[sel.y+idtab*8] = tempo[sel.y+1+idtab*8];
 timer[sel.y+idtab*8] = timer[sel.y+1+idtab*8];
 }
 // synchro inter-grilles
if(PGet(Up) && sel.y == 0){
 trace[sel.y+idtab*8] = trace[7+(!idtab)*8];
 tempo[sel.y+idtab*8] = tempo[7+(!idtab)*8];
 timer[sel.y+idtab*8] = timer[7+(!idtab)*8];
 }
 if(PGet(Down) && sel.y == 7){
 trace[sel.y+idtab*8] = trace[(!idtab)*8];
 tempo[sel.y+idtab*8] = tempo[(!idtab)*8];
 timer[sel.y+idtab*8] = timer[(!idtab)*8];
 }

 
}
else { // deplacement
 if(PGet(Up)) sel.y--;
 if(PGet(Down)) sel.y++;
}
if(PGet(Left)) sel.x--;
if(PGet(Right)) sel.x++;
if(sel.y > 7) sel.y = 0;
if(sel.y < 0) sel.y = 7;
if(sel.x > COL-1) sel.x = 0;
if(sel.x < 0) sel.x = COL-1;
PBloqueTouche(Left);
PBloqueTouche(Right);
PBloqueTouche(Up);
PBloqueTouche(Down);

//-------------------------------
// deplacement de la trace
timerCourant = PTime();
for(i=0; i<16; i++) {
 if(timerCourant-timer[i] > 60000/tempo[i]) // ici on fait avancer la barre
  {
   timer[i] = timerCourant;
   trace[i]++;
   if (trace[i] > COL-1) trace[i] = 0;
   // on joue les sons correspondants
   if (tab[i][trace[i]]){
      //printf("son %d\n", i);
      int c = i;
      c%=8;
      PPlaySound(c, i);
     }
   }
 }

//----------------------
// changement de grille
if (PGet(L)) {
 etat = ETAT_TRANS;
 InitialiseGame();
 PBloqueTouche(L);
 return;
}
 affLecture();
}


//#######################   TRANSITION
int trans;
int sens;

void initTransition(){
 idtab = !idtab;
 trans = 0;
 sens = idtab - (!idtab);
}

void transition(){
int i = 0;
 // affichage de la partie haute
 int dec = 0;
 if (sens == 1) dec = trans;
 else dec = 272-trans;
 for(i=0;i<8;i++)
  {
   int j = 0;
   for(j=0; j<COL; j++)
    {
     if (tab[i][j]) PPrintImage(8 + j*33 , 6 + i*32 - dec, 1);
    }
  }
 for (i=0; i<8; i++) {
  int dec2 = 32 * ((double) (timerCourant - timer[i]) / ((double)60000/(double)tempo[i]));
  PPrintImage(trace[i]*33-65+dec2, 6+i*32 - dec, 13);
 }
 
 for (i=0; i<8; i++) PPrintNb(442, i*32 - 2 - dec, tempo[i]);


 // affichage de la partie basse
 if (sens == 1) dec = 272-trans;
 else dec = trans;
 for(i=0;i<8;i++)
  {
   int j = 0;
   for(j=0; j<COL; j++)
    {
     if (tab[i+8][j]) PPrintImage(8 + j*33, 6 + i*32 + dec, 1);
    }
  }
 for (i=0; i<8; i++) {
  int dec2 = 32 * ((double) (timerCourant - timer[i+8]) / ((double)60000/(double)tempo[i+8]));
  PPrintImage(trace[i+8]*33-65+dec2, 6+i*32 + dec, 13);
 }
 for (i=0; i<8; i++) PPrintNb(442, i*32 - 2 + dec, tempo[i+8]);
 

 PPrintImage(477, 0+10*idtab, 14);
 trans += 10;
 if (trans>272) etat = ETAT_PLAY;

 //-------------------------------
// deplacement de la trace
timerCourant = PTime();
for(i=0; i<16; i++) {
 if(timerCourant-timer[i] > 60000/tempo[i]) // ici on fait avancer la barre
  {
   timer[i] = timerCourant;
   trace[i]++;
   if (trace[i] > COL-1) trace[i] = 0;
   // on joue les sons correspondants
   if (tab[i][trace[i]]){
      //printf("son %d\n", i);
      int c = i;
      c%=8;
      PPlaySound(c, i);
     }
   }
 }

 PPrintImage(0,0, 0); // masque
 PPrintImage(446, 32*idtab, 14);

}







//###############################  BOUCLE  #################################
void InitialiseGame()
{
 switch (etat) {
  case ETAT_PAUSE: initPause(); break;
  case ETAT_PLAY: initLecture(); break;
  case ETAT_TRANS: initTransition(); break;
  default : break;
 }
}

void GameLoop()
{
 switch (etat) {
  case ETAT_PLAY: lecture(); break;
  case ETAT_TRANS: transition(); break;
  default : break;
 }

}

