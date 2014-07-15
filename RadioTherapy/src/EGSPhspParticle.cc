/*==================================================================

      PLOTPHSP UTILITIES LIBRARY

      Juan Diego Soler Pulido
      email: juandiego.soler@ciemat.es
      Madrid, Spain
      December 2006
      Daniel P. A.
      05/2008
      Pedro Arce 
      11/2009
 =================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <cstring>

#include "EGSPhspParticle.hh"

EGSPhspParticle::EGSPhspParticle(FILE* , std::string ) 
{
  theFilePointer = 5*sizeof(char)+2*sizeof(int)+3*sizeof(float)+3;
}


EGSPhspParticle::~EGSPhspParticle()
{
}


//-------------------------------------------------------------------
int EGSPhspParticle::RefreshParticle(FILE* file, long number)
{
  float aux;
  int reset_phsp;
  int a1;
  long step;
  
  step = sizeof(unsigned int) + 6*sizeof(float);
  theParticleNumber = number;
  (void) fseek(file,theFilePointer,SEEK_SET);
  a1 = fread(&theLatch,sizeof(unsigned int),1,file);
  a1 &= fread(&theEnergy,sizeof(float),1,file);
  a1 &= fread(&thePosX,sizeof(float),1,file);
  thePosX *= 10.;
  a1 &= fread(&thePosY,sizeof(float),1,file);
  thePosY *= 10.;
  a1 &= fread(&theDirU,sizeof(float),1,file);
  a1 &= fread(&theDirV,sizeof(float),1,file);
  a1 &= fread(&theWeight,sizeof(float),1,file);
  
  if(a1){
    reset_phsp = false;
    theEnergy = fabs(theEnergy);
    if(!CheckIntegrity(theParticleNumber)){
      printf("Phase space problem on record %d!\n",int(theParticleNumber));
      return true;
    }
    // calculate theDirW
    aux = theDirU*theDirU+theDirV*theDirV;
    if (aux<=1.0)
      theDirW = (float) sqrt((float)(1.0-aux));
    else{
      aux= (float)sqrt((float)aux);
      theDirU/=aux;
      theDirV/=aux;
      theDirW=0.0f;
    }
    if(theDirW < 0.0f)
      theDirW *= -1.0f;
    // obtain npass from theLatch
    
    if ((theLatch & 0x80000000) != 0){
      // printf("Multiple crosser\n");
      theNPass=1;
      theLatch &= 0x7FFFFFFF;
    } else
      theNPass=0;
    
    // obtain q from theLatch
    if((theLatch & 0x40000000) != 0){
      theCharge = -1;
      theLatch &= 0xBFFFFFFF;
    } else if ((theLatch & 0x20000000) != 0){
      theCharge = +1;
      theLatch &= 0xDFFFFFFF;
    } else
      theCharge = 0;
    
    theFilePointer += step;
    
    int MAX_PRINT = 10;
    if(theParticleNumber <= MAX_PRINT){
      std::cout << "# " <<theParticleNumber;
      std::cout << " Q " << theCharge;
      std::cout << " E " << theEnergy;
      std::cout << " x " << thePosX;
      std::cout << " y " << thePosY << std::endl;
      std::cout << " u " << theDirU;
      std::cout << " v " << theDirV;
      std::cout << " w " << theDirW;
      std::cout << " W " << theWeight;
      std::cout << " latch " << theLatch;
      std::cout << " NPass " << theNPass <<  std::endl;
    }
  }else{
    std::cerr << "!!! EGSPhspParticle::RefreshParticle: unsuccesful fread at record " << theFilePointer/step << std::endl;
    std::cerr << "End of file" << std::endl;
    
    //     if(fseek(file,(long)0,SEEK_SET) != 0){
    //       printf("In phsp_rw: phsp_read_particle() file could not be reset\n");
    //       exit(-1);
    //     }
    //     eof = eof + 1; //increase the number of restarts
    reset_phsp = true;
    //     theFilePointer += sizeof(long int)+6*sizeof(float);
    //    return reset_phsp;
  }
  
  return reset_phsp;
}


/*void EGSPhspParticle::write_EGSPhspParticle(FILE* outphspfile, long number){
	//char muse;

		long int aux_theLatch;
		if(number == 1){
			theFilePointer_out = 5*sizeof(char)+2*sizeof(int)+3*sizeof(float)+3;
			(void) fseek(outphspfile,theFilePointer_out,SEEK_SET);
		}

		//  (void) fseek(outphspfile,theFilePointer_out,SEEK_SET);

		if (q == -1)
			aux_theLatch |= 0x40000000; // set bit 30
		else if (q == 0)
			aux_theLatch &= 0x9FFFFFFF; // clear bit 30,29
		else {
			aux_theLatch &= 0xBFFFFFFF; 
			aux_theLatch |= 0x20000000; // set 29, clr 30
		}

		if (npass == 0)
			aux_theLatch &= 0x7FFFFFFF;  // clear bit 31
		else
			aux_theLatch |= 0x80000000;  // set bit 31

		fwrite(&aux_theLatch,sizeof(long int),1,outphspfile);
		fwrite(&e,sizeof(float),1,outphspfile);
		fwrite(&x,sizeof(float),1,outphspfile);
		fwrite(&y,sizeof(float),1,outphspfile);
		fwrite(&u,sizeof(float),1,outphspfile);
		fwrite(&v,sizeof(float),1,outphspfile);
		fwrite(&wt,sizeof(float),1,outphspfile);
		theFilePointer_out += sizeof(long int) + 6*sizeof(float);
}

*/

int EGSPhspParticle::CheckIntegrity(int i)
{
  char str[50];

  sprintf(str, "%e", thePosX);
  if (strcmp(str,"nan")==0) 
    {
      printf("x coord in record number %d\n",i);
      return(false);
    }
  sprintf(str,"%e",thePosY);
  if (std::strcmp(str,"nan")==0) 
    {
      printf("y coord in record number %d\n",i);
      return(false);
    }
  sprintf(str,"%e",theDirU);
  if (std::strcmp(str,"nan")==0) 
    {
      printf("u direction in record number %d\n",i);
      return(false);
    }
  sprintf(str,"%e",theDirV);
  if (std::strcmp(str,"nan")==0) 
    {
      printf("v direction in record number %d\n",i);
      return(false);
    }
  sprintf(str,"%e",double(theLatch));
  if (std::strcmp(str,"nan")==0) 
    {
      printf("theLatch variable in record number %d\n",i);
      return(false);
    }
  sprintf(str,"%e",theDirW);
  if (std::strcmp(str,"nan")==0) 
    {
      printf("weight in record number %d\n",i);
      return(false);
    }
  sprintf(str,"%e",theEnergy);
  if (std::strcmp(str,"nan")==0) 
    {
      printf("energy in record number %d\n",i);
      return(false);
    }

  return true;
}
