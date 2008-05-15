/****************************************************************************/
/*  PSON : module de sonore de Pgck, developpe par Antoine Letouzey          */
/*  antoine.letouzey@gmail.com   ---   https://antoine.sxbn.org              */
/****************************************************************************/



#ifndef PSON_H
#define PSON_H

//#include <SDL/SDL_mixer.h>

#define NB_SONS 8



void PInitSon();
void PUnInitSon();
void PChangeMusiqueFond(char* path);
int PSetVolume(int vol);
void PPlayStopFond(int flag);
void PPlaySound(int sample, int channel);
void PStopChannel(int channel);
int PGetVolume();


#endif
