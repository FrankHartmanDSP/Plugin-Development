#include "Delay.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 3;

enum EParams
{
  kFeedback = 0,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  kFeedbackX = 79,
  kFeedbackY = 62,
  kKnobFrames = 128
};

Delay::Delay(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mFeedback(1.), ptrL(0), ptrR(0), firstIteration(true)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kFeedback)->InitDouble("Feedback", 50.0, 0.0, 99.9, 0.01, "%");
  GetParam(kFeedback)->SetShape(0.1);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedbackX, kFeedbackY, kFeedback, &knob));

  AttachGraphics(pGraphics);

  CreatePresets();
}

Delay::~Delay() {}

void Delay::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  int const channelCount = 2;

  for (int i = 0; i < channelCount; i++) {
	  double* input = inputs[i];
	  double* output = outputs[i];

	  for (int s = 0; s < nFrames; ++s, ++input, ++output) {
		  // Writing data to left and right channel buffers for delay effect
		  
		  if (i == 0) {
			  if (ptrL > 32767) {
				  ptrL = 0;
				  firstIteration = false;
			  }
			  mBufferL[ptrL] = *input;
			  // Delay effect
			  if (!firstIteration) {
				  if (mBufferL[DelayPtrCalculation(&ptrL)]) {
					  *output = *input + mBufferL[DelayPtrCalculation(&ptrL)];
				  }
				  else {
					  *output = *input;
				  }

			  }
			  else {
				  *output = *input;
			  }
		  } else {
			  /**
			  if (ptrR > 32767) {
				  ptrR = 0;
				  firstIteration = false;
			  }
			  mBufferR[ptrR] = *input;
			  if (!firstIteration) {
				  if (mBufferR[DelayPtrCalculation(&ptrR)]) { //unhandled read exception
					  *output = *input + mBufferR[DelayPtrCalculation(&ptrR)];
				  }
				  else {
					  *output = *input;
				  }

			  }
			  else {
				  *output = *input;
			  }
			  */
		  }
		  

		  
		  
		  /**
		  if (*input >= 0) {
			  *output = fmin(*input, mFeedback);
		  } else {
			  *output = fmax(*input, -mFeedback);
		  }
		  *output /= mFeedback;
		  */

		  ptrL++;
		  ptrR++;
		  //*output = *input;
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
    case kFeedback:
      mFeedback = 1 - (GetParam(kFeedback)->Value() / 100.);
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

int Delay::DelayPtrCalculation(int * ptr) {
	if ((*ptr - 16384) < 0) {
		return (32768 + (*ptr - 16384));
	} else {
		return (*ptr - 16384);
	}
}
