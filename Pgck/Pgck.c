/****************************************************************************/
/*  PGCK : Psp Game Creator Kit base sur NGCK de Lapintade                  */
/*  Porte par Antoine Letouzey                                              */
/*  antoine.letouzey@gmail.com   ---   https://antoine.sxbn.org             */
/****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <zlib.h>

#include "psp_main.h"
#include "Pgck.h"
#include "Pson.h"


//donne a l'avance le nombre de sprite qui sont charges
#define nombreSprite 16


//----------------------------------------------------------------
// declaration prealable de certaine fonctions
//----------------------------------------------------------------
//void SetGlobalColor(SDL_Surface*, SDL_Color);
extern void InitialiseGame(void);
extern void GameLoop(void);

//----------------------------------------------------------------
// VARIABLES GLOBALES
//----------------------------------------------------------------


//declaration du fond et de l'ecran
SDL_Surface* SpriteFond    = NULL;
SDL_Surface* screen = NULL;

//declaration du tableau contenant les sprites
SDL_Surface* tabSprite[nombreSprite];

short int continuer = 1;
int shot = 0;
// vitesse de rafraichissement 1 => 60Hz
float vitesse=2.0;
int pretAquitter = 0;


// utile pour savoir si on doit relancer l'affichage ou attendre un peu
long time1, time2; 

// liste de booleens qui servent a connaitre l'etat d'une touche
touche tabTouche[12];

// couleur du texte
SDL_Color CoulTexte;

//position du joystic
//couple posJoy;



//------------------------------------------
//                FONCTIONS
//------------------------------------------







//---------------------------------------------------------------------------------------------
// chargement des images des sprites 
//--------------------------------------------------------------------------------------------- 

SDL_Surface *Load_and_Display(const char *filename)
 {
   SDL_Surface *Surftemp1, *Surftemp2;     /*  Surfaces temporaires  */

   Surftemp1 = IMG_Load(filename); /* Charge l'image */
   Surftemp2 = SDL_DisplayFormat(Surftemp1);
   SDL_FreeSurface(Surftemp1);
     /* Renvoie l'image chargee*/
   return Surftemp2;
 }



//------------------------------------------
// donne le temps en milisecondes
//------------------------------------------
long PTime()
{
 return SDL_GetTicks();
}

//------------------------------------------
// fixe la vitesse du jeu
//------------------------------------------
void PVitesse(float v)
{
 vitesse = v;
}

//------------------------------------------
// change l'image de fond
//------------------------------------------
void PChangeFond(char* fic)
{
 printf("cahrgement de l'image\n");
 fflush(stdout);
 if (SpriteFond != NULL) SDL_FreeSurface(SpriteFond);
 SpriteFond = IMG_Load(fic);
 if (SpriteFond == NULL) printf("argggg non image pas charge mayday mayday\n");
 fflush(stdout);
}

void PBloqueFond()
{
 SpriteFond = screen;
}




//------------------------------------
// quitte le jeu
//------------------------------------
void Quit()
 {
  continuer = 0;
 }


// -------------------------------------------------------------------
// affiche une image  en (x,y)
// -------------------------------------------------------------------

void PPrintImage(int x, int y, int numSprite)
{
  SDL_Rect rect;
  rect.x = x; rect.y = y;
  SDL_BlitSurface(tabSprite[numSprite], NULL, screen, &rect);
}



//--------------------------------------------------------------------
// affiche un nombre (recursif)
//--------------------------------------------------------------------
void PPrintNb(int x, int y, int v)
 {
  if (v<0) v = 0;
  int unite = v%10;
  SDL_Rect rect;
  rect.x = x; rect.y = y;
  SDL_BlitSurface(tabSprite[unite+3], NULL, screen, &rect);
  if(v/10) PPrintNb(x-15, y, v /10);
 }

void PPrintScore(int x, int y, unsigned long v)
 {
  if (v<0) v = 0;
  int unite = v%10;
  SDL_Rect rect;
  rect.x = x; rect.y = y;
  SDL_BlitSurface(tabSprite[unite], NULL, screen, &rect);
  if(v/10) PPrintNb(x-15, y, v /10);
 }

void PPrintCode(int x, int y, char* code)
{
 if ((*code) == 0) return;
 char c = code[strlen(code)-1];
 int unite = c - '0';
 code[strlen(code)-1] = '\0';
 SDL_Rect rect;
 rect.x = x; rect.y = y;
 SDL_BlitSurface(tabSprite[unite], NULL, screen, &rect);
 PPrintCode(x-15, y, code);
 }

void PPrintFond()
 {
  SDL_Rect rect;
  rect.x = 0; rect.y = 0;
  SDL_BlitSurface(SpriteFond, NULL, screen, &rect);
 }


/*

// -------------------------------------------------------------------
// pour ecrire une chaine de caractere
//le couple (x,y) donne les coordonees en pixel du coin superieur gauche de la 1ere lettre de la chaine
//ch est un tableau de char contenant la chaine
//taille est la taille de la police
// -------------------------------------------------------------------

void PPrintMot(int x, int y, char *ch)
{
    // Ecriture du texte dans la SDL_Surface "texte" en mode Blended (optimal)
    SDL_Surface *texte = TTF_RenderUTF8_Blended(police, ch, CoulTexte);
    SDL_Rect rect;
    rect.x = x; rect.y = y;
    SDL_BlitSurface(texte, NULL, screen, &rect);
    SDL_FreeSurface(texte);
}
*/

//------------------------------------------------------------------------
// modifie la couleur du texte a partir de 3 composantes (RVB)
//------------------------------------------------------------------------
void PSetCouleurTexte(int R, int V, int B)
{
 CoulTexte.r=R; 
 CoulTexte.g=V;
 CoulTexte.b=B;
}


// -------------------------------------------------------------------
// vide la fenetre en remplissant le fond
// -------------------------------------------------------------------
void EffaceEcran2()
{
 SDL_Rect rect;
 rect.x = rect.y = 0;
 SDL_BlitSurface(SpriteFond, NULL, screen, &rect);
}


// -------------------------------------------------------------------------------------------
// Permet de bloquer une touche ou bouton de la souris  -> desactive le fait de garder appuyee
// -------------------------------------------------------------------------------------------
void PBloqueTouche(int t)
{
 if (tabTouche[t].etat) tabTouche[t].libre = 0;
}
// -------------------------------------------------------------------
// Bloque toutes les commandes d'un coup
// -------------------------------------------------------------------
void PEffaceTouches()
{
 int i;
 for(i=Circle; i<R; i++) if (tabTouche[i].etat) tabTouche[i].libre = 0;
}

// -------------------------------------------------------------------
// Mise a jour des "valeurs" des touches.
// -------------------------------------------------------------------

void demanderQuitter(void);
#if 0
void UpdateCommandes()
 {
   SDL_Event event;

  /*  Gestions des messages Systeme  */
   while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
                   //#################  souris  ##########################
                case SDL_MOUSEBUTTONUP:                                //relachement d'un bouton
                         switch( event.button.button )
                                 {
                                 case SDL_BUTTON_LEFT:
                                                      tabTouche[L].etat=0;
                                                      tabTouche[L].libre=1;
                                                       break;
                                 case SDL_BUTTON_RIGHT:
                                                      tabTouche[R].etat=0;
                                                      tabTouche[R].libre=1;
                                                       break;
                                 default:
                                         break;
                                 }
                                 break;
                case SDL_MOUSEBUTTONDOWN:                          //appui sur un bouton
                         switch( event.button.button )
                                 {
                                 case SDL_BUTTON_LEFT:
                                                      tabTouche[L].etat=1;
                                                       break;
                                 case SDL_BUTTON_RIGHT:
                                                      tabTouche[R].etat=1;
                                                       break;
                                 default:
                                         break;
                                 }
                                 break;
                    //#################   clavier   ########################
				case SDL_KEYDOWN:                                  //appui sur une touche
                     switch( event.key.keysym.sym )
                                    {
  			               case SDLK_ESCAPE:
                     		               if (!pretAquitter) demanderQuitter();
				               break;

		   		       case SDLK_SPACE:
                                               tabTouche[Start].etat = 1;
                                               break;
		   		       case SDLK_LEFT:
                                               tabTouche[Left].etat = 1;
                                               break;
		   		       case SDLK_RIGHT:
		   		       		tabTouche[Right].etat = 1;
                                               break;
		   		       case SDLK_DOWN:
		   		       		tabTouche[Down].etat = 1;
                                               break;
		   		       case SDLK_UP:
		   		        	tabTouche[Up].etat = 1;
                                               break;
                       		       case SDLK_z:
                                               tabTouche[Triangle].etat = 1;
                                               break;
                                       case SDLK_d:
                                               tabTouche[Circle].etat = 1;
                                               break;
                                       case SDLK_q:
                                               tabTouche[Square].etat = 1;
                                               break;
                                       case SDLK_s:
                                               tabTouche[Cross].etat = 1;
                                               break;
                                       default:
                                               break;
	                            }
                                   break;

				case SDL_KEYUP:                               //relachement d'une touche
				switch( event.key.keysym.sym )
                                    {
		   		       case SDLK_SPACE:
                                               tabTouche[Start].etat = 0;
                                               tabTouche[Start].libre = 1;
                                               break;
		   		       case SDLK_LEFT:
                                               tabTouche[Left].etat = 0;
                                               tabTouche[Left].libre = 1;
                                               break;
		   		       case SDLK_RIGHT:
		   		       		tabTouche[Right].etat = 0;
                                               tabTouche[Right].libre = 1;
                                               break;
		   		       case SDLK_DOWN:
		   		       		tabTouche[Down].etat = 0;
                                               tabTouche[Down].libre = 1;
                                               break;
		   		       case SDLK_UP:
		   		        	tabTouche[Up].etat = 0;
                                               tabTouche[Up].libre = 1;
                                               break;
                       		       case SDLK_z:
                                               tabTouche[Triangle].etat = 0;
                                               tabTouche[Triangle].libre = 1;
                                               break;
                                       case SDLK_d:
                                               tabTouche[Circle].etat = 0;
                                               tabTouche[Circle].libre = 1;
                                               break;
                                       case SDLK_q:
                                               tabTouche[Square].etat = 0;
                                               tabTouche[Square].libre = 1;
                                               break;
                                       case SDLK_s:
                                               tabTouche[Cross].etat = 0;
                                               tabTouche[Cross].libre = 1;
                                               break;
                                       default:
                                               break;
	                            }
                                   break;
        			case SDL_QUIT:
                                   continuer = 0;
                                   break;
			        default:
				  break;
			}
		}

}
#endif


void UpdateCommandes()
{
 SceCtrlData pad;
 sceCtrlReadBufferPositive(&pad, 1);
 if (pad.Buttons & PSP_CTRL_CIRCLE) tabTouche[Circle].etat = 1;
 else { tabTouche[Circle].etat = 0; tabTouche[Circle].libre = 1; }
 
 if (pad.Buttons & PSP_CTRL_TRIANGLE) tabTouche[Triangle].etat = 1;
 else { tabTouche[Triangle].etat = 0; tabTouche[Triangle].libre = 1; }

 if (pad.Buttons & PSP_CTRL_SQUARE) tabTouche[Square].etat = 1;
 else { tabTouche[Square].etat = 0; tabTouche[Square].libre = 1; }

 if (pad.Buttons & PSP_CTRL_CROSS) tabTouche[Cross].etat = 1;
 else { tabTouche[Cross].etat = 0; tabTouche[Cross].libre = 1; }

 if (pad.Buttons & PSP_CTRL_START) tabTouche[Start].etat = 1;
 else { tabTouche[Start].etat = 0; tabTouche[Start].libre = 1; }

 if (pad.Buttons & PSP_CTRL_SELECT) tabTouche[Select].etat = 1;
 else { tabTouche[Select].etat = 0; tabTouche[Select].libre = 1; }

 if (pad.Buttons & PSP_CTRL_RIGHT) tabTouche[Right].etat = 1;
 else { tabTouche[Right].etat = 0; tabTouche[Right].libre = 1; }

 if (pad.Buttons & PSP_CTRL_UP) tabTouche[Up].etat = 1;
 else { tabTouche[Up].etat = 0; tabTouche[Up].libre = 1; }

 if (pad.Buttons & PSP_CTRL_LEFT) tabTouche[Left].etat  = 1;
 else { tabTouche[Left].etat = 0; tabTouche[Left].libre = 1; }

 if (pad.Buttons & PSP_CTRL_DOWN) tabTouche[Down].etat  = 1;
 else { tabTouche[Down].etat = 0; tabTouche[Down].libre = 1; }

 if (pad.Buttons & PSP_CTRL_LTRIGGER) tabTouche[L].etat = 1;
 else { tabTouche[L].etat = 0; tabTouche[L].libre = 1; }

 if (pad.Buttons & PSP_CTRL_RTRIGGER) tabTouche[R].etat = 1;
 else { tabTouche[R].etat = 0; tabTouche[R].libre = 1; }
 
 //if ((pad.Buttons & PSP_CTRL_HOME) && !pretAquitter) continuer = 0;

}

//--------------------------------------------------------------------
// Les fonctions suivantes permettent de raporter l'etat d'une touche
//--------------------------------------------------------------------

int PGet(int t)
 {
  return (tabTouche[t].etat && tabTouche[t].libre);
 }

// -------------------------------------------------------------------
couple PGetJoy()
{
 couple posJoy;
 SceCtrlData pad;
 sceCtrlReadBufferPositive(&pad, 1);
 posJoy.x = pad.Lx-128;
 posJoy.y = pad.Ly-128;
 return posJoy;
}



/*
//-----------------------------------------------------------------------
//retourne un couple d'entier contenant les coordonnees de la souris
//-----------------------------------------------------------------------
couple PGetJoy()
{
  couple coord;
  int a,b;
  SDL_GetMouseState(&a,&b);
  coord.x= (((float)a/480.0)*256.0)-128;
  coord.y=(((float)b/272.0)*256.0)-128;
  return coord;
}
*/

void demanderQuitter()
{
 SDL_Surface tmpQuitter;
 pretAquitter = 1;
 tmpQuitter = (*screen);
 SDL_Rect r;
 r.x = 0; r.y = 0;
 SDL_BlitSurface(tabSprite[0], NULL, screen, &r);
 SDL_Flip(screen);
 while(1)
  {
   UpdateCommandes();
   if (PGet(Cross)) { continuer = 0; PBloqueTouche(Cross); break; }
   if (PGet(Triangle))
    {
     PBloqueTouche(Triangle);
     (*screen) = tmpQuitter;
     pretAquitter = 0;
     break;
    }
  }
}





//-------------------------------------------------------------------------------------------- 
// Initialisation du moteur
//--------------------------------------------------------------------------------------------
void PGCKInit()
{
  time1 = time2 = SDL_GetTicks();
  // par defaut on active l'affichage de la souris
  // initialisation des sprites
  SpriteFond   = IMG_Load("datas/fond.png");
  int i = 0;
  tabSprite[i++] = IMG_Load("datas/masque.png");//--0
  tabSprite[i++] = IMG_Load("datas/doton.png");
  tabSprite[i++] = IMG_Load("datas/select.png");
  tabSprite[i++] = IMG_Load("datas/chiffres/00.png");//-3
  tabSprite[i++] = IMG_Load("datas/chiffres/01.png");
  tabSprite[i++] = IMG_Load("datas/chiffres/02.png");
  tabSprite[i++] = IMG_Load("datas/chiffres/03.png");
  tabSprite[i++] = IMG_Load("datas/chiffres/04.png");
  tabSprite[i++] = IMG_Load("datas/chiffres/05.png");
  tabSprite[i++] = IMG_Load("datas/chiffres/06.png");
  tabSprite[i++] = IMG_Load("datas/chiffres/07.png");
  tabSprite[i++] = IMG_Load("datas/chiffres/08.png");
  tabSprite[i++] = IMG_Load("datas/chiffres/09.png");//-12
  tabSprite[i++] = IMG_Load("datas/wave.png");
  tabSprite[i++] = IMG_Load("datas/light.png");
  tabSprite[i++] = IMG_Load("datas/zone.png"); //--15
   


  // initialisation de l'etat des touches
  for (i=Circle; i<R; i++) {tabTouche[i].etat = 0; tabTouche[i].libre = 1; }

  //sceCtrlSetSamplingCycle(0);
  //sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

  // initialise la couleur du texte en rouge.
  PSetCouleurTexte(220,10,10);
  // initialisation de la partie son.
  PInitSon();
}


// ---------------------------------------------------------------------------------------------
void PGCKGameLoop()
{
  time1 = SDL_GetTicks();

//   // vitesse=1 => le jeu tourne a 60 fps
//   if ((float)(time1-time2)<16.0*vitesse)
//   {
//     UpdateCommandes();
//     return;
//    }

  time2=time1;
  UpdateCommandes();
   // screenshot
  if (tabTouche[Select].etat && tabTouche[L].etat && tabTouche[R].etat)
           {
            char path[50];
            sprintf(path,"datas/shot--%d.bmp", shot);
            shot++;
            SDL_SaveBMP(screen, path);
           }
  EffaceEcran2();
  GameLoop();
  return;
}


// ---------------------------------------------------------------------------------------------
void PGCKUnInit()
{
  int i;
  // liberation de l'espace alloue pour les images
  //if (SpriteFond != NULL) SDL_FreeSurface(SpriteFond);
  for (i=0;i<nombreSprite;i++) if (tabSprite[i] != NULL) SDL_FreeSurface(tabSprite[i]);
}


//------- prog principal


int PGCKmain(int argc, char *argv[])
{
 //-- initialise le Random
 srand(time(0));
 //TTF_Init();
 /* Initialise SDL */
 if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0 ) {
	printf("Couldn't initialize SDL: %s\n",SDL_GetError());
	exit(1);
 }
 atexit(SDL_Quit);
 screen = SDL_SetVideoMode(480, 272, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

 //---------affichage du logo pendant le chargement des images (ou au moins 4 secondes)
 long t1 = SDL_GetTicks();
 SDL_Surface *logo = IMG_Load("datas/fondcredits.png");
 SDL_Surface *g2 = IMG_Load("datas/guyver2.png");
 SDL_Surface *g = IMG_Load("datas/genevois.png");
 SDL_Rect rect;
 rect.x = 0; rect.y = 0;
 SDL_BlitSurface(logo, NULL, screen, &rect);
 rect.x = 50+rand()%330; rect.y = 20+rand()%210;
 SDL_BlitSurface(g2, NULL, screen, &rect);
 rect.x = 50+rand()%330; rect.y = 20+rand()%210;
 SDL_BlitSurface(g, NULL, screen, &rect);
 SDL_Flip(screen);
 
 PGCKInit();
 while(SDL_GetTicks() - t1 < 2000) ;// 4 secondes au moins
 SDL_FreeSurface(logo);
 SDL_FreeSurface(g);
 SDL_FreeSurface(g2);
 
 
 // chargemnt fini

 InitialiseGame();
 while (continuer) {
          PGCKGameLoop();
          SDL_Flip(screen);
 }
 // fin du jeu liberation de la memoire allouee
 PGCKUnInit();
 //FIN du programme
 return 0;
}
