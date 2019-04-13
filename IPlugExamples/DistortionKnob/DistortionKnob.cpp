#include "DistortionKnob.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 3;

enum EParams
{
  kThreshold = 0,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  kThresholdX = 100,
  kThresholdY = 50,
  kKnobFrames = 60
};

DistortionKnob::DistortionKnob(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mThreshold(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kThreshold)->InitDouble("Threshold", 100.0, 0.01, 100.0, 0.01, "%");
  GetParam(kThreshold)->SetShape(2.5);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kThresholdX, kThresholdY, kThreshold, &knob));

  AttachGraphics(pGraphics);

  CreatePresets();
}

DistortionKnob::~DistortionKnob() {}

void DistortionKnob::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  int const channelCount = 2;

  for (int i = 0; i < channelCount; i++) {
	  double* input = inputs[i];
	  double* output = outputs[i];

	  for (int s = 0; s < nFrames; ++s, ++input, ++output) {
		  if (*input >= 0) {
			  *output = fmin(*input, mThreshold);
		  } else {
			  *output = fmax(*input, -mThreshold);
		  }
		  *output /= mThreshold;
	  }
  }
}

void DistortionKnob::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void DistortionKnob::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kThreshold:
      mThreshold = GetParam(kThreshold)->Value() / 100.;
      break;

    default:
      break;
  }
}

void DistortionKnob::CreatePresets() {
	MakePreset("Clean", 100.0);
	MakePreset("Subtle", 80.0);
	MakePreset("Extreme", 0.01);
}
