#include "Delay.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 3;

enum EParams
{
  kWetLevel = 0,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  kWetLevelX = 79,
  kWetLevelY = 62,
  kKnobFrames = 128
};

Delay::Delay(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mWetLevel(1.), mPtr(0),
	mBufferSize(32768), firstIteration(true)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kWetLevel)->InitDouble("WetLevel", 50.0, 0.0, 100.0, 0.01, "%");
  GetParam(kWetLevel)->SetShape(0.1);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kWetLevelX, kWetLevelY, kWetLevel, &knob));

  AttachGraphics(pGraphics);

  CreatePresets();
}

Delay::~Delay() {}

void Delay::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked.

  int const channelCount = 2;

  for (int i = 0; i < channelCount; i++) {
    double* input = inputs[i];
    double* output = outputs[i];
	
	  for (int s = 0; s < nFrames; ++s, ++input, ++output) {
		// Reset pointer index to 0 upon reaching end (Circular Buffer)
	    if (mPtr >= mBufferSize) {
		  mPtr = 0;
		  firstIteration = false;
		}
		// Writing data to audio buffer for delay effect
		mBuffer[mPtr] = *input;
		// Delay effect
		if (!firstIteration) {
		  *output = *input + (mWetLevel * mBuffer[DelayPtrCalc(&mPtr)]);
		} else {
		  // Do not create delay effect if no data exists in the buffer
		  *output = *input;
		}
		++mPtr;
	  }
  }
}

void Delay::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void Delay::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kWetLevel:
      mWetLevel = (GetParam(kWetLevel)->Value() / 100.);
      break;

    default:
      break;
  }
}

void Delay::CreatePresets() {
	MakePreset("Default", 50.0);
	MakePreset("Subtle", 20.0);
	MakePreset("Extreme", 99.9);
}

int Delay::DelayPtrCalc(int * ptr) {
  if ((*ptr - (mBufferSize / 2)) < 0) {
    return (mBufferSize + (*ptr - (mBufferSize / 2)));
  } else {
    return (*ptr - (mBufferSize / 2));
  }
}
