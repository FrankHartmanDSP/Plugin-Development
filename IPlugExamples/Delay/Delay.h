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
  int DelayPtrCalc(int * ptr);

private:
  double mWetLevel;
  //int mSampleRate; // How to access if changed from 44.1kHz? 
  // -> Unnecessary, GetSampleRate() is inherited from IPlugBase
  int mPtr;
  double mBuffer[32768]; // 2^15 is 32768
  int mBufferSize;
  void CreatePresets();
  bool firstIteration;

};

#endif
