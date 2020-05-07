// Copyright (c) 2020 [Your Name]. All rights reserved.

#include "my_app.h"

#include <cinder/app/App.h>
#include <cinder/gl/gl.h>
#include <cinder/audio/audio.h>



#include "cinder/audio/Source.h"
#include "cinder/audio/Target.h"
#include "cinder/audio/cocoa/ContextAudioUnit.h"


namespace myapp {



using namespace ci;
using namespace ci::app;
using namespace std;
using namespace reza::ui;
using namespace myapp;

AudioSampler::AudioSampler() {
    auto ctx = audio::Context::master();

    // create a SourceFile and set its output samplerate to match the Context.
    mSourceFile = audio::load( app::loadAsset("tasty burger.mp3"), ctx->getSampleRate() );

    // load the entire sound file into a BufferRef, and construct a BufferPlayerNode with this.
    audio::BufferRef buffer = mSourceFile->loadBuffer();
    mBufferPlayerNode = ctx->makeNode( new audio::BufferPlayerNode( buffer ) );
    mBufferPlayerNode->setLoopEnabled();


    // audio_bound setup
    start.setUp(0, mBufferPlayerNode->getNumFrames());
    bounds.push_back(start);
    end.setUp(mBufferPlayerNode->getNumFrames(), mBufferPlayerNode->getNumFrames());
    bounds.push_back(end);

    // connect and enable the Context
    mBufferPlayerNode >> ctx->getOutput();
    ctx->enable();
}

void AudioSampler::setup() {
    cinder::gl::enableDepthWrite();
    cinder::gl::enableDepthRead();


    mUi = SuperCanvas::create( "Sequencer" , getWindow());
    mUi->addSlideri("Num Samples", &numSamples, 1, kSequenceLimit);
    mUi->setOrigin(vec2(0, kControlsYPosition - 100));

    mUiControls = SuperCanvas::create( "Controls" , getWindow());
    mUiControls->addSliderf("Volume", &volume);
    mUiControls->addSliderf( "Pan", &pan, 0, 1.0, Sliderf::Format().crossFader() );
    mUiControls->addSpacer();
    mUiControls->addSpacer();
    mUiControls->addSlideri( "LowPass", &low, 0, low );
    mUiControls->addSlideri( "HighPass", &high, 0, low );
    mUiControls->setOrigin(vec2(0, kControlsYPosition));

    mUiAutomation = SuperCanvas::create( "Automation" , getWindow());
    mUiAutomation->addSliderf("Volume", &finalVolume);
    mUiAutomation->addSliderf("Pan", &finalPan, 0, 1.0, Sliderf::Format().crossFader() );
    mUiAutomation->setOrigin(vec2(getWindowWidth() / 2, kControlsYPosition));

}

void AudioSampler::draw() {
    gl::clear();
    gl::enableAlphaBlending();

    drawAudioPlayer(kAudioPosition, mWaveformPlot, mBufferPlayerNode);
    bounds[0].draw(), bounds[1].draw();
    if (currentStage != Clip) {
        drawAudioPlayer(kSamplePosition, mSampleWaveformPlot, mSampleBufferPlayerNode );
        bounds[2].draw(), bounds[3].draw();
    }
    drawSequencer();
}

void AudioSampler::drawAudioPlayer(const Rectf& size, WaveformPlot plot, const BufferPlayerNodeRef& player) {
    gl::setMatricesWindow( getWindowSize());
    gl::color( border );
    gl::drawStrokedRect(size);
    plot.load( player->getBuffer(), size );
    plot.draw(size.getX1(), size.getY1());

    // draw the current play position
    float readPos = (1.0/kWindowToWidgetRatio)* (float)getWindowWidth() * player->getReadPosition() / player->getNumFrames();
    gl::color( border );
    gl::drawSolidRect( Rectf( readPos - 2, 0, readPos + 2, 100 ) );

}
void AudioSampler::drawSequencer() {
    gl::setMatricesWindow( getWindowSize());
    gl::color( border);
    gl::drawStrokedRect(kSequencerPosition);
    for (size_t range = 0; range < kSequenceLimit; range++) {
        if (range < numSamples) {
            if (range == currentSample ) {
                gl::color( currentColor);
            } else {
                gl::color( lightBlue);
            }
        } else {
            gl::color( darkBlue);
        }
        int x1 = (kBoxSize * range) + 50, y1 =  kPaddedWidth / 2 + 55;
        const Rectf kSequencerPosition = Rectf(x1, y1, x1 + 40, y1 + 40);
        gl::drawSolidRect(kSequencerPosition);
    }
}

void AudioSampler::keyDown(KeyEvent event) {
    if (currentStage == Clip) {
        if (event.getCode() == KeyEvent::KEY_SPACE) {
            if (mBufferPlayerNode->isEnabled())
                mBufferPlayerNode->stop();
            else {
                mBufferPlayerNode->setLoopBegin(bounds[0].currentPosition);
                mBufferPlayerNode->start(mBufferPlayerNode->getLoopBeginTime());
            }
        } else if( event.getCode() == KeyEvent::KEY_c ) {
            currentSample++;
            setUpSample(mBufferPlayerNode, mSourceFile, 0, "sample.wav");
            setUpBounds(mBufferPlayerNode);
            currentStage = Sample;
        }
    } else if (currentStage == Sample) {
        if (mBufferPlayerNode->isEnabled())
            mBufferPlayerNode->stop();
        if (event.getCode() == KeyEvent::KEY_SPACE) {
            if( mSampleBufferPlayerNode->isEnabled() )
                mSampleBufferPlayerNode->stop();
            else {
                mSampleBufferPlayerNode->start(mSampleBufferPlayerNode->getLoopBeginTime());
            }
        } else if( event.getCode() == KeyEvent::KEY_c ) {
            setUpSample(mSampleBufferPlayerNode, mSampleSourceFile, 2, "sample.wav");
            setUpBounds(mSampleBufferPlayerNode);
        } else if( event.getCode() == KeyEvent::KEY_s ) {
            if ( mSampleBufferPlayerNode->isEnabled() )
                mSampleBufferPlayerNode->stop();
            setUpSequencer();
        }
    } else {
        if( event.getCode() == KeyEvent::KEY_p ) {
            size_t index = 1;
            samples[0].getSampleBufferPlayer()->start();
            updateControls();
            for (size_t sample = 0; sample < numSamples; sample++)
                updateAutomation(sample);
            while (index < numSamples) {
                if (!samples[index-1].getSampleBufferPlayer()->isEnabled()) {
                    samples[index].getSampleBufferPlayer()->start();
                    index++;
                }
            }
        } else if( event.getCode() == KeyEvent::KEY_e ) {
            if (currentStage == Sequence) {
                //exportSequence();
                for (size_t sample = 0; sample < numSamples; sample++) {
                    samples[sample].writeToFile(outPath + "out" + to_string(sample) + ".wav");
                }
            }
        }
    }
}

void AudioSampler::fileDrop(FileDropEvent event) {
    try {
        mSourceFile.reset();
        mSourceFile = audio::load( loadFile( event.getFile( 0 ) ) );
        mBufferPlayerNode->loadBuffer( mSourceFile );
        bounds[0].setUp(0, mBufferPlayerNode->getNumFrames());
        bounds[1].setUp(mBufferPlayerNode->getNumFrames(), mBufferPlayerNode->getNumFrames());
        mWaveformPlot.load( mBufferPlayerNode->getBuffer(), getWindowBounds() );
        currentStage = Clip;
    } catch( Exception &exc ) {
        throw exc;
    }

}

void AudioSampler::setUpSample(const BufferPlayerNodeRef& player, const SourceFileRef& source, size_t bound_index, const string& filename) {

    audio::BufferRef sampleBuffer = player->getBuffer();
    sampleBuffer->copyOffset(*sampleBuffer,
            bounds[bound_index + 1].currentPosition - bounds[bound_index].currentPosition,
            0, bounds[bound_index].currentPosition);
    fs::path newSamplePath = outPath + filename;

    try {
        audio::TargetFileRef target = audio::TargetFile::create( newSamplePath,
                source->getSampleRate(), source->getNumChannels() );
        target->write( sampleBuffer.get(),
                bounds[bound_index + 1].currentPosition - bounds[bound_index].currentPosition);

        }
    catch( audio::AudioFileExc &exc ) {
        throw exc;
    }
}
void AudioSampler::setUpBounds(const BufferPlayerNodeRef& player){
    if (player == mBufferPlayerNode) {
        auto sampleCtx = audio::Context::master();
        setUpSampleBufferPlayer(sampleCtx);
        size_t newSampleNumFrames = mSampleBufferPlayerNode->getNumFrames();

        sampleStart.setUp(0, newSampleNumFrames);
        bounds.push_back(sampleStart);
        sampleEnd.setUp(newSampleNumFrames, newSampleNumFrames);
        bounds.push_back(sampleEnd);
    } else {
        auto sampleCtx = &*mSampleBufferPlayerNode->getContext();
        sampleCtx->uninitializeNode(mSampleBufferPlayerNode);
        setUpSampleBufferPlayer(sampleCtx);
        size_t newSampleNumFrames = mSampleBufferPlayerNode->getNumFrames();

        bounds[2].setUp(0, newSampleNumFrames);
        bounds[3].setUp(newSampleNumFrames, newSampleNumFrames);
    }
}

void AudioSampler::setUpSampleBufferPlayer(Context* sampleCtx) {
    // create a SourceFile and set its output samplerate to match the Context.
    mSampleSourceFile = audio::load( app::loadAsset("sample.wav"), sampleCtx->getSampleRate() );

    // load the entire sound file into a BufferRef, and construct a BufferPlayerNode with this.
    audio::BufferRef buffer = mSampleSourceFile->loadBuffer();
    mSampleBufferPlayerNode = sampleCtx->makeNode( new audio::BufferPlayerNode( buffer ) );

    mSampleBufferPlayerNode >> sampleCtx->getOutput();
    sampleCtx->enable();
}

void AudioSampler::setUpSequencer() {
    for (size_t sample = 0; sample < kSequenceLimit; sample++) {
        auto newSample = SampleController();
        setUpSample(mSampleBufferPlayerNode, mSampleSourceFile, 2, "samples/out" + to_string(sample) + ".wav");
        newSample.setup("samples/out" + to_string(sample) + ".wav");
        newSample.setVolume(volume);
        newSample.setPan(pan);
        samples.push_back(newSample);
    }
    setControls();
    currentStage = Sequence;
}

void AudioSampler::exportSequence() {
    fs::path exportPath = outPath + "export.wav";
    audio::BufferRef exportRef;
    auto ctx = audio::Context::master();
    mExportRecorderNode = ctx->makeNode( new audio::BufferRecorderNode);
    mExportRecorderNode->setNumFrames(mSampleBufferPlayerNode->getNumFrames() * numSamples, true);
    mExportRecorderNode->setAutoEnabled(true);
    try {
        for (size_t index = 0; index < numSamples; index++) {
            samples[index].getSampleBufferPlayer() >> mExportRecorderNode;
            //mExportRecorderNode->setEnabled(true);
            //mExportRecorderNode->disable(samples[index].getSampleBufferPlayer()->getLoopEndTime());
        }
    }
    catch( audio::AudioFileExc &exc ) {
        throw exc;
    }
//    mExportRecorderNode->writeToFile(exportPath);
}

void AudioSampler::mouseDown(MouseEvent event) {
    for (AudioBound &bound : bounds) {
        if (abs(event.getX() - bound.getXPosition()) < 5) {
            bound.wasMoved = true;
        }
    }
    if (kSequencerPosition.contains(event.getPos())) {
        int index = (event.getX() - 50) / (getWindowHeight() / kSequenceLimit);
        if (index < numSamples) {
            updateControls();
            currentSample = index;
            setControls();
        }
    }
}

void AudioSampler::mouseDrag(MouseEvent event) {
    for (AudioBound &bound : bounds) {
        if (bound.wasMoved) {
            size_t frame = event.getX() * bound.getNumFrames() / (kWindowToWidgetRatio * getWindowWidth());
            bound.setPosition(frame);
        }
    }
    if( mBufferPlayerNode->isEnabled() )
        mBufferPlayerNode->seek( mBufferPlayerNode->getNumFrames() * event.getX() / getWindowWidth() );
}

void AudioSampler::mouseUp(MouseEvent event) {
    for (size_t index = 0; index < bounds.size(); index++) {
        if (bounds[index].wasMoved) {
            size_t frame = event.getX() * bounds[index].getNumFrames() / (kWindowToWidgetRatio * getWindowWidth());
            bounds[index].setPosition(frame);
            if (currentStage == Clip) {
                if (index % 2 == 0) {
                    mBufferPlayerNode->setLoopBegin(bounds[index].currentPosition);
                } else {
                    mBufferPlayerNode->setLoopEnd(bounds[index].currentPosition);
                }
            } else if (currentStage == Sample) {
                if (index % 2 == 0) {
                    mSampleBufferPlayerNode->setLoopBegin(bounds[index].currentPosition);
                } else {
                    mSampleBufferPlayerNode->setLoopEnd(bounds[index].currentPosition);
                }
            }
            bounds[index].wasMoved = false;
        }
    }
}

void AudioSampler::setControls() {
    volume = samples.at(currentSample).getVolume();
    pan = samples.at(currentSample).getPan();
    low = samples.at(currentSample).getLowPass();
    high = samples.at(currentSample).getHighPass();
    finalVolume = samples.at(currentSample).getParamVolume();
    finalPan = samples.at(currentSample).getParamPan();
}

void AudioSampler::updateControls() {
    if (volume != samples[currentSample].getVolume()) {
        samples[currentSample].setVolume(volume);
    }
    if (finalVolume != samples[currentSample].getParamVolume() || volume != finalVolume) {
        samples[currentSample].setParamVolume(finalVolume);
    }
    if (pan != samples[currentSample].getPan()) {
        samples[currentSample].setPan(pan);
    }
    if (finalPan != samples[currentSample].getParamPan() || pan != finalPan) {
        samples[currentSample].setParamPan(finalPan);
    }
    if (low != samples[currentSample].getLowPass()) {
        samples[currentSample].setLowPass(low);
    }
    if (high != samples[currentSample].getHighPass()) {
        samples[currentSample].setHighPass(high);
    }
}

void AudioSampler::updateAutomation(size_t index) {
    auto sample = samples[index];
    if (sample.getParamVolume() != sample.getVolume()) {
        sample.setRampVolume();
    }
    if (sample.getParamPan() != sample.getPan()) {
        sample.setRampPan();
    }

}


}  // namespace myapp
