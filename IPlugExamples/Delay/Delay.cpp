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
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mWetLevel(1.), ptrL(0), ptrR(0),
	mBufferSize(32767), firstIteration(true)
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
		  // Writing data to left and right channel buffers for delay effect
		  if (i == 0) {
			  if (ptrL > mBufferSize) {
				  ptrL = 0;
				  firstIteration = false;
			  }
			  mBufferL[ptrL] = *input;
			  // Delay effect
			  if (!firstIteration) {
				  if (mBufferL[DelayPtrCalc(&ptrL)]) {
					  *output = *input + (mWetLevel * mBufferL[DelayPtrCalc(&ptrL)]);
				  } else {
					  *output = *input;
				  }
			  } else {
				  // Do not create delay effect if no data exists in the buffer
				  *output = *input;
			  }
			  ++ptrL;
		  } else {
			  if (ptrR > mBufferSize) {
				  ptrR = 0;
				  firstIteration = false;
			  }
			  mBufferR[ptrR] = *input;
			  if (!firstIteration) {
				  if (mBufferR[DelayPtrCalc(&ptrR)]) {
					  *output = *input + (mWetLevel * mBufferR[DelayPtrCalc(&ptrR)]);
				  } else {
					  *output = *input;
				  }
			  }
			  else {
				  *output = *input;
			  }
			  ptrR++;
		  }
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
	if ((*ptr - 16384) < 0) {
		return (32768 + (*ptr - 16384));
	} else {
		return (*ptr - 16384);
	}
}
