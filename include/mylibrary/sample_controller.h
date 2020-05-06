#ifndef SAMPLE_CONTROLLER_H_
#define SAMPLE_CONTROLLER_H_

#include <cinder/audio/audio.h>
#include "cinder/audio/cocoa/ContextAudioUnit.h"
#include "../../../../samples/_audio/BufferPlayer/include/Resources.h"
#include "../../../../samples/_audio/common/AudioDrawUtils.h"
#include "../../../../blocks/Cinder-UI/src/UI.h"




namespace myapp {
    using namespace ci;
    using namespace std;
    using namespace audio;
    using namespace reza::ui;
    using namespace audio::cocoa;
    /**
     * This class holds an individual sample. The main app pulls the Waveform and Buffer from an instance
     * of SampleController. This class will also handles all effects on the sample, which will will be controlled
     * by the UI in the my_app
     */
    class SampleController {
        public:
            SampleController();
            // Declares the BufferPlayer and WaveformPlot of the sample located at asset
            void setup(const fs::path asset);
            // Will update the effects based on changes mad to sliders and dials in UI
            void update();
            void draw();

            float getVolume();
            void setVolume(float value);
            float getPan();
            void setPan(float value);
            float getLowPass();
            void setLowPass(int freq);
            float getHighPass();
            void setHighPass(int freq);
            void writeToFile(const fs::path asset);
            BufferPlayerNodeRef getSampleBufferPlayer();
            BufferRecorderNodeRef getBufferRecorderNode();
            SourceFileRef getSampleSourceFile();
            WaveformPlot getSampleWaveformPlot();


        private:

            BufferPlayerNodeRef mBufferPlayerNode;
            SourceFileRef mSourceFile;
            WaveformPlot mWaveformPlot;

            BufferRecorderNodeRef mRecorderNode;

            // This object handles all effects so that settings can differ between samples
            audio::GainNodeRef          mGain;
            audio::Pan2dNodeRef         mPan;
            audio::DelayNodeRef			mDelay;
            audio::FilterLowPassNodeRef		mLowPass;
            audio::FilterHighPassNodeRef	mHighPass;

            // Using cinder::cocoa to create effects --> less control
            shared_ptr<EffectAudioUnitNode> delay;
            shared_ptr<EffectAudioUnitNode> lowpass;

    };

}
#endif  // SAMPLE_CONTROLLER_H_