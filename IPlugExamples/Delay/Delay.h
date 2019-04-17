#ifndef __DELAY__
#define __DELAY__

#include "IPlug_include_in_plug_hdr.h"

class Delay : public IPlug
{
public:
  Delay(IPlugInstanceInfo instanceInfo);
  ~Delay();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  int DelayPtrCalculation(int * ptr);

private:
  double mFeedback;
  //int mSampleRate; // How to access if changed from 44.1kHz? 
  // -> Unnecessary, GetSampleRate() is inherited from IPlugBase
  int ptrL;
  int ptrR;
  double mBufferL[32768]; // 2^15 is 32768
  double mBufferR[32768]; // 2^15
  void CreatePresets();
  bool firstIteration; // Replace with logic

};

#endif
