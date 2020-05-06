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

AudioAligner::AudioAligner() {
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

void AudioAligner::setup() {
    cinder::gl::enableDepthWrite();
    cinder::gl::enableDepthRead();


    mUi = SuperCanvas::create( "Sequencer" , getWindow());
    mUi->addSlideri("Num Samples", &numSamples, 1, kSequenceLimit);

    mUi->setOrigin(vec2(0, 450));

    mUiControls = SuperCanvas::create( "Controls" , getWindow());
    mUiControls->addSliderf("Volume", &volume);
    mUiControls->addSliderf( "Pan", &pan, 0, 1.0, Sliderf::Format().crossFader() );
    mUiControls->addSpacer();
    mUiControls->addSpacer();
    mUiControls->addSlideri( "LowPass", &low, 0, 1600 );
    mUiControls->addSlideri( "HighPass", &high, 0, 1600 );

    mUiControls->setOrigin(vec2(0, 550));

}

void AudioAligner::draw() {

    //mUi->load( getSaveLoadPath() );
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

void AudioAligner::drawAudioPlayer(const Rectf& size, WaveformPlot plot, BufferPlayerNodeRef player) {
    gl::setMatricesWindow( getWindowSize());
    gl::color( ColorA( 0, 1, 0, 0.7f ) );
    gl::drawStrokedRect(size);
    plot.load( player->getBuffer(), size );
    plot.draw(size.getX1(), size.getY1());

    // draw the current play position
    float readPos = (1.0/kWindowToWidgetRatio)* (float)getWindowWidth() * player->getReadPosition() / player->getNumFrames();
    gl::color( ColorA( 0, 1, 0, 0.7f ) );
    gl::drawSolidRect( Rectf( readPos - 2, 0, readPos + 2, 100 ) );

}
void AudioAligner::drawSequencer() {
    gl::setMatricesWindow( getWindowSize());
    gl::color( ColorA( 0, 1, 0, 0.7f ) );
    gl::drawStrokedRect(kSequencerPosition);
    for (size_t range = 0; range < kSequenceLimit; range++) {
        if (range < numSamples) {
            if (range == currentSample ) {
                gl::color( ColorA( 0, .5, .2, 0.7f ) );
            } else {
                gl::color( ColorA( 0, 0, 1, 0.7f ) );
            }
        } else {
            gl::color( ColorA( 0, 0, 1, 0.2f ) );
        }

        const Rectf kSequencerPosition = Rectf((43.75 * range) + 50, 405, (43.75 * range) + 90, 445);
        gl::drawSolidRect(kSequencerPosition);
    }
}

void AudioAligner::keyDown(KeyEvent event) {
    if( event.getCode() == KeyEvent::KEY_SPACE ) {
        if (currentStage == Clip) {
            if( mBufferPlayerNode->isEnabled() )
                mBufferPlayerNode->stop();
            else {
                mBufferPlayerNode->setLoopBegin(bounds[0].currentPosition);
                mBufferPlayerNode->start(mBufferPlayerNode->getLoopBeginTime());
            }
        } else {
            if( mSampleBufferPlayerNode->isEnabled() )
                mSampleBufferPlayerNode->stop();
            else {
                mSampleBufferPlayerNode->start(mSampleBufferPlayerNode->getLoopBeginTime());
            }
        }
    } else if( event.getCode() == KeyEvent::KEY_c ) {
        if( mBufferPlayerNode->isEnabled() )
            mBufferPlayerNode->stop();
        if (currentStage == Clip) {
            currentSample++;
            setUpSample(mBufferPlayerNode, mSourceFile, 0, "sample.wav");
            setUpBounds(mBufferPlayerNode);
        } else {
            setUpSample(mSampleBufferPlayerNode, mSampleSourceFile, 2, "sample.wav");
            setUpBounds(mSampleBufferPlayerNode);
        }
        currentStage = Sample;
    } else if( event.getCode() == KeyEvent::KEY_s ) {
        if( mBufferPlayerNode->isEnabled() )
            mBufferPlayerNode->stop();
        if ( mSampleBufferPlayerNode->isEnabled() )
            mSampleBufferPlayerNode->stop();
        setUpSequencer();
    } else if( event.getCode() == KeyEvent::KEY_p ) {
        size_t index = 0;
        samples[index].getSampleBufferPlayer()->start();
        index++;
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
                samples[sample].writeToFile(outPath + "2/out2" + to_string(sample) + ".wav");
            }
        }
    }
}

void AudioAligner::fileDrop(FileDropEvent event) {
    try {
        mSourceFile.reset();
        mSourceFile = audio::load( loadFile( event.getFile( 0 ) ) );

        mBufferPlayerNode->loadBuffer( mSourceFile );
        start.setUp(0, mBufferPlayerNode->getNumFrames());
        end.setUp(mBufferPlayerNode->getNumFrames(), mBufferPlayerNode->getNumFrames());
        //mBufferPlayerNode->setLoopEnabled();

        mWaveformPlot.load( mBufferPlayerNode->getBuffer(), getWindowBounds() );
    } catch( Exception &exc ) {
        throw exc;
    }

}

void AudioAligner::setUpSample(BufferPlayerNodeRef player, SourceFileRef source, size_t bound_index, string filename) {

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
void AudioAligner::setUpBounds(BufferPlayerNodeRef player){
    if (player == mBufferPlayerNode) {
        auto sampleCtx = audio::Context::master();
        setUpSampleBufferPlayer(sampleCtx);
        size_t newSampleNumFrames = mSampleBufferPlayerNode->getNumFrames();
        // sample audio_bound setup
        sampleStart.setUp(0, newSampleNumFrames);
        bounds.push_back(sampleStart);
        sampleEnd.setUp(newSampleNumFrames, newSampleNumFrames);
        bounds.push_back(sampleEnd);
    } else {
        auto sampleCtx = &*mSampleBufferPlayerNode->getContext();
        sampleCtx->uninitializeNode(mSampleBufferPlayerNode);
        setUpSampleBufferPlayer(sampleCtx);
        size_t newSampleNumFrames = mSampleBufferPlayerNode->getNumFrames();
        // sample audio_bound reset
        bounds[2].reset(0, newSampleNumFrames);
        bounds[3].reset(newSampleNumFrames, newSampleNumFrames);
    }
}

void AudioAligner::setUpSampleBufferPlayer(Context* sampleCtx) {
    // create a SourceFile and set its output samplerate to match the Context.
    mSampleSourceFile = audio::load( app::loadAsset("sample.wav"), sampleCtx->getSampleRate() );

    // load the entire sound file into a BufferRef, and construct a BufferPlayerNode with this.
    audio::BufferRef buffer = mSampleSourceFile->loadBuffer();
    mSampleBufferPlayerNode = sampleCtx->makeNode( new audio::BufferPlayerNode( buffer ) );

    mSampleBufferPlayerNode >> sampleCtx->getOutput();
    sampleCtx->enable();
}

void AudioAligner::setUpSequencer() {
    for (size_t sample = 0; sample < kSequenceLimit; sample++) {
        auto newSample = SampleController();
        setUpSample(mSampleBufferPlayerNode, mSampleSourceFile, 2, "samples/out" + to_string(sample) + ".wav");
        newSample.setup("samples/out" + to_string(sample) + ".wav");
        newSample.setVolume(volume);
        newSample.setPan(pan);
        samples.push_back(newSample);
    }
    currentStage = Sequence;
}

void AudioAligner::exportSequence() {
    fs::path exportPath = outPath + "export.wav";
    audio::BufferRef exportRef;
    auto ctx = audio::Context::master();
    mExportRecorderNode = ctx->makeNode( new audio::BufferRecorderNode);
    mExportRecorderNode->setNumFrames(mSampleBufferPlayerNode->getNumFrames() * numSamples, true);
    mExportRecorderNode->setAutoEnabled(true);
    try {
        for (size_t index = 0; index < numSamples; index++) {
//        for (size_t index = 0; index < temp->getNumFrames(); index++) {
//            *compositeBuffer = *temp;
//            temp++;
//            &compositeBuffer++;
//        }
            std::cout << mExportRecorderNode->getRecordedCopy()->getNumFrames() << std::endl;
            samples[index].getSampleBufferPlayer() >> mExportRecorderNode;
            //mExportRecorderNode->setEnabled(true);
            //mExportRecorderNode->disable(samples[index].getSampleBufferPlayer()->getLoopEndTime());
        }
    }
    catch( audio::AudioFileExc &exc ) {
        throw exc;
    }
//    mExportRecorderNode->writeToFile(exportPath);
    try {
        audio::TargetFileRef target = audio::TargetFile::create( exportPath,
                                                                 mSampleSourceFile->getSampleRate(), mSampleSourceFile->getNumChannels() );
        std::cout << mExportRecorderNode->getRecordedCopy().get()->getNumFrames() << " " << mExportRecorderNode->getNumFrames() << std::endl;
        target->write(mExportRecorderNode->getRecordedCopy().get(), 512);

    }
    catch( audio::AudioFileExc &exc ) {
        throw exc;
    }
}

void AudioAligner::mouseDown(MouseEvent event) {
    for (AudioBound &bound : bounds) {
        if (abs(event.getX() - bound.getXPosition()) < 5) {
            bound.wasMoved = true;
        }
    }
    if (kSequencerPosition.contains(event.getPos())) {
        int index = (event.getX() - 50) / (700 / kSequenceLimit);
        if (index < numSamples) {
            currentSample = index;
            setControls();
        }
    }
}

void AudioAligner::mouseDrag(MouseEvent event) {
    for (AudioBound &bound : bounds) {
        if (bound.wasMoved) {
            size_t frame = event.getX() * bound.getNumFrames() / (kWindowToWidgetRatio * getWindowWidth());
            bound.setPosition(frame);
        }
    }
    if( mBufferPlayerNode->isEnabled() )
        mBufferPlayerNode->seek( mBufferPlayerNode->getNumFrames() * event.getX() / getWindowWidth() );
}

void AudioAligner::mouseUp(MouseEvent event) {
    for (size_t bound_ind = 0; bound_ind < bounds.size(); bound_ind++) {
        if (bounds[bound_ind].wasMoved) {
            size_t frame = event.getX() * bounds[bound_ind].getNumFrames() / (kWindowToWidgetRatio * getWindowWidth());
            bounds[bound_ind].setPosition(frame);
            if (bound_ind % 2 == 0) {
                if (currentSample < 0) {
                    mBufferPlayerNode->setLoopBegin(bounds[bound_ind].currentPosition);
                } else {
                    mSampleBufferPlayerNode->setLoopBegin(bounds[bound_ind].currentPosition);
                }
            } else {
                if (currentSample < 0) {
                    mBufferPlayerNode->setLoopEnd(bounds[bound_ind].currentPosition);
                } else {
                    mSampleBufferPlayerNode->setLoopEnd(bounds[bound_ind].currentPosition);
                }
            }
            bounds[bound_ind].wasMoved = false;
        }
    }
}

void AudioAligner::update() {
    if (currentStage == Sequence) {
        if (volume != samples[currentSample].getVolume()) {
            samples[currentSample].setVolume(volume);
        } else if (pan != samples[currentSample].getPan()) {
            samples[currentSample].setPan(pan);
        } else if (low != samples[currentSample].getLowPass()) {
            samples[currentSample].setLowPass(low);
        } else if (high != samples[currentSample].getHighPass()) {
            samples[currentSample].setHighPass(high);
        }

    }
}

void AudioAligner::setControls() {
    volume = samples.at(currentSample).getVolume();
    pan = samples.at(currentSample).getPan();
    low = samples.at(currentSample).getLowPass();
    high = samples.at(currentSample).getHighPass();
}

// Used to quickly load mUi default from json
//fs::path AudioAligner::getSaveLoadPath()
//{
//    fs::path path = getAssetPath( "" );
//    path += "/" + mUi->getName() + ".json";
//    return path;
//}



}  // namespace myapp
