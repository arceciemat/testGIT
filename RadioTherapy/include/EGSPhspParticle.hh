#ifndef __EGSPhspParticle_H_
#define __EGSPhspParticle_H_

#include <string>

#define SEGMENT_BEG_TOKEN '$'
#define SEGMENT_END_TOKEN ':'
#define OK     0
#define FAIL  -1
#define MAX_NPART 5 // 1 photons
                    // 2 electrons
                    // 3 positrons
                    // 4 neutrons
                    // 5 protons

class EGSPhspParticle {
public:
  EGSPhspParticle(FILE* file, std::string mode);
  ~EGSPhspParticle();
  long theParticleNumber;
  unsigned int theLatch;
  float theCharge;
  float theEnergy;
  float thePosX, thePosY, thePosZ;
  float theDirU, theDirV, theDirW;
  float theWeight;
  int theNPass;
  // char muse;
  long theFilePointer;
  //  int eof;
  //  vector<long> theLongs;
  //  vector<float> theFloats;
  
  //  int rc_x, rc_y, rc_z, rc_u, rc_v, rc_w, rc_wght, rc_flts, rc_lngs;
  int CheckIntegrity(int i);
  int RefreshParticle(FILE* file, long number);
  //  void write_EGSPhspParticle(FILE* outphspfile, long number);
  float theXmin, theXmax, theYmin, theYmax;
  float theDeltaX, theTotalX, theDeltaY, theTotalY;

private:
};

#endif
