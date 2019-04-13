#ifndef __DISTORTIONKNOB__
#define __DISTORTIONKNOB__

#include "IPlug_include_in_plug_hdr.h"

class DistortionKnob : public IPlug
{
public:
  DistortionKnob(IPlugInstanceInfo instanceInfo);
  ~DistortionKnob();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mThreshold;
  void CreatePresets();
};

#endif
