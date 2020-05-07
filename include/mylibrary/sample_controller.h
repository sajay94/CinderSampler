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

            //Getters and setter to modify SampleController based on UI Sliders
            float getVolume();
            void setVolume(float value);
            float getPan();
            void setPan(float value);
            float getLowPass();
            void setLowPass(int freq);
            float getHighPass();
            void setHighPass(int freq);

            //Getters and setters to adjust automation for gain and pan
            float getParamVolume();
            void setParamVolume(float value);
            float getParamPan();
            void setParamPan(float value);
            void setRampVolume();
            void setRampPan();

            void writeToFile(const fs::path asset);
            BufferPlayerNodeRef getSampleBufferPlayer();

        private:

            BufferPlayerNodeRef mBufferPlayerNode;
            SourceFileRef mSourceFile;
            WaveformPlot mWaveformPlot;

            BufferRecorderNodeRef mRecorderNode;

            // This object handles all effects so that settings can differ between samples
            audio::GainNodeRef          mGain;
            audio::Pan2dNodeRef         mPan;
            audio::FilterLowPassNodeRef		mLowPass;
            audio::FilterHighPassNodeRef	mHighPass;

            float volume = 1.0;
            float pan = 0.5;
            int low = 1600;
            int high = 0;
            float delay = 0;
            float finalVolume = 1;
            float finalPan = 0.5;

    };

}
#endif  // SAMPLE_CONTROLLER_H_