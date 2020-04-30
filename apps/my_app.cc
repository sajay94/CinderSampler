// Copyright (c) 2020 [Your Name]. All rights reserved.

#include "my_app.h"

#include <cinder/app/App.h>
#include <cinder/gl/gl.h>
#include <cinder/audio/audio.h>



#include "cinder/audio/Source.h"
#include "cinder/audio/Target.h"

#include "AudioUnit/AudioUnit.h"
#include "cmath"


namespace myapp {



using namespace ci;
using namespace ci::app;
using namespace std;
using namespace au;

const fs::path kOutputFileName = "/Users/family/Cinder/Projects/final-project-sajay94-sampler/assets/crazy-808.wav";
const fs::path kMp3FileName = "/Users/family/Cinder/Projects/final-project-sajay94-sampler/assets/deepbark.mp3";

AudioAligner::AudioAligner() {
    auto ctx = audio::Context::master();

    //AudioUnit demo
//    reverb = au::GenericUnit(kAudioUnitType_Effect, kAudioUnitSubType_MatrixReverb);
//    fileToPlay.setFile(app::loadAsset("out2.wav"));
//    fileToPlay.connectTo(reverb).connectTo(tap).connectTo(mixer).connectTo(output);
//    output.start();

    // create a SourceFile and set its output samplerate to match the Context.
    mSourceFile = audio::load( app::loadAsset("tasty burger.mp3"), ctx->getSampleRate() );

    // load the entire sound file into a BufferRef, and construct a BufferPlayerNode with this.
    audio::BufferRef buffer = mSourceFile->loadBuffer();
    mBufferPlayerNode = ctx->makeNode( new audio::BufferPlayerNode( buffer ) );
    mBufferPlayerNode->setLoopEnabled();


    start.setUp(0, mBufferPlayerNode->getNumFrames());
    end.setUp(mBufferPlayerNode->getNumFrames(), mBufferPlayerNode->getNumFrames());
    // add a Gain to reduce the volume
    mGain = ctx->makeNode( new audio::GainNode( 0.5f ) );

    // connect and enable the Context
    mBufferPlayerNode >> mGain >> ctx->getOutput();
    ctx->enable();
}

void AudioAligner::setup() {
    cinder::gl::enableDepthWrite();
    cinder::gl::enableDepthRead();
}

void AudioAligner::draw() {
    gl::clear();
    gl::enableAlphaBlending();

    if (isSampleSelected) {
        sample.update();
        gl::color( ColorA( 0, 1, 0, 0.7f ) );
        const cinder::PolyLine2 poly = drawSample();
        gl::draw(poly);
    } else {
        drawAudioPlayer();
        start.draw();
        end.draw();
    }
}
void AudioAligner::drawAudioPlayer() {
    gl::setMatricesWindow( getWindowSize());
    const Rectf size(50, 100, 750, 300);
    gl::color( ColorA( 0, 1, 0, 0.7f ) );
    gl::drawStrokedRect(size);
    mWaveformPlot.load( mBufferPlayerNode->getBuffer(), size );
    mWaveformPlot.draw(50, 100);

    // draw the current play position
    float readPos = (.875) * (float)getWindowWidth() * mBufferPlayerNode->getReadPosition() / mBufferPlayerNode->getNumFrames();
    gl::color( ColorA( 0, 1, 0, 0.7f ) );
    gl::drawSolidRect( Rectf( readPos - 2, 0, readPos + 2, 200 ) );


//    float readPos = (.875) * (float)getWindowWidth() * mBufferPlayerNode->getReadPosition() / mBufferPlayerNode->getNumFrames();
//    gl::color( ColorA( 0, 1, 0, 0.7f ) );
//    gl::drawSolidRect( Rectf( readPos - 2, 0, readPos + 2, 200 ) );

//        if (currentState == importDubAudio) {
//            for (size_t elem : timeStamps ) {
//                float readPos = (.875) * (float)getWindowWidth() * elem / mBufferPlayerNode->getNumFrames();
//                gl::color( ColorA( 1, .6, 0, 0.7f ) );
//                gl::drawSolidRect( Rectf( readPos - 2, 0, readPos + 2, 200 ) );
//            }
//        }
    }

void AudioAligner::keyDown(KeyEvent event) {
    if( event.getCode() == KeyEvent::KEY_SPACE && !isSampleSelected) {
        if( mBufferPlayerNode->isEnabled() )
            mBufferPlayerNode->stop();
        else
            mBufferPlayerNode->setLoopBegin(start.currentPosition);
            mBufferPlayerNode->start(mBufferPlayerNode->getLoopBeginTime());
    } else if( event.getCode() == KeyEvent::KEY_w ) {
        setUpSample();
        setSample();
    } else if( event.getCode() == KeyEvent::KEY_p ) {
        //reverb.showUI("Reverb");
        fileToPlay.play(0);
    } else if( event.getCode() == KeyEvent::KEY_UP ) {
        mixer.setInputVolume(.1, 0);
        std::cout << mixer.getInputLevel(0) << std::endl;
    } else if( event.getCode() == KeyEvent::KEY_DOWN ) {
        mixer.setInputVolume(1, 0);
        std::cout << mixer.getInputLevel(0) << std::endl;
    } else if( event.getCode() == KeyEvent::KEY_r ) {
        if (!isDistortionOn) {
            if (isReverbOn) {
                fileToPlay.connectTo(tap).connectTo(mixer, 0).connectTo(output);
            } else {
                fileToPlay.connectTo(reverb).connectTo(tap).connectTo(mixer, 0).connectTo(output);
            }
        } else {
            if (isReverbOn) {
                fileToPlay.connectTo(distortion).connectTo(tap).connectTo(mixer, 0).connectTo(output);
            } else {
                fileToPlay.connectTo(distortion).connectTo(reverb).connectTo(tap).connectTo(mixer, 0).connectTo(output);
            }
        }
        isReverbOn = !isReverbOn;
    } else if( event.getCode() == KeyEvent::KEY_d ) {
        if (!isReverbOn) {
            if (isDistortionOn) {
                fileToPlay.connectTo(tap).connectTo(mixer, 0).connectTo(output);
            } else {
                fileToPlay.connectTo(distortion).connectTo(tap).connectTo(mixer, 0).connectTo(output);
            }
        } else {
            if (isDistortionOn) {
                fileToPlay.connectTo(reverb).connectTo(tap).connectTo(mixer, 0).connectTo(output);
            } else {
                fileToPlay.connectTo(distortion).connectTo(reverb).connectTo(tap).connectTo(mixer, 0).connectTo(output);
            }
        }
        isDistortionOn = !isDistortionOn;
    }
}

void AudioAligner::fileDrop(FileDropEvent event) {
    const fs::path& filePath = event.getFile( 0 );
    getWindow()->setTitle( filePath.filename().string() );

    mSourceFile.reset();
    mSourceFile = audio::load( loadFile( filePath ) );

    // BufferPlayerNode can also load a buffer directly from the SourceFile.
    // This is safe to call on a background thread, which would alleviate blocking the UI loop.
    mBufferPlayerNode->loadBuffer( mSourceFile );
    start.setUp(0, mBufferPlayerNode->getNumFrames());
    end.setUp(mBufferPlayerNode->getNumFrames(), mBufferPlayerNode->getNumFrames());
    //mBufferPlayerNode->setLoopEnabled();

    mWaveformPlot.load( mBufferPlayerNode->getBuffer(), getWindowBounds() );
}

void AudioAligner::resize() {
    // visualize the audio buffer
    ci::Area bounds = getWindowBounds();
    bounds.setY1(bounds.getY1() - bounds.getHeight()/2);
    bounds.setY2(bounds.getY2() - bounds.getHeight()/2);

    if( mBufferPlayerNode )
        mWaveformPlot.load( mBufferPlayerNode->getBuffer(), bounds);
}
void AudioAligner::setUpSample() {

    audio::BufferRef audioBuffer = mBufferPlayerNode->getBuffer();
    audio::BufferRef buf = mBufferPlayerNode->getBuffer();

    audioBuffer->copyOffset(*buf, end.currentPosition - start.currentPosition, 0, start.currentPosition);

    try {
        audio::TargetFileRef target = audio::TargetFile::create( kOutputFileName, mSourceFile->getSampleRate(), mSourceFile->getNumChannels() ); // INT_16
        //	audio::TargetFileRef target = audio::TargetFile::create( fileName, mSourceFile->getSampleRate(), mSourceFile->getNumChannels(), audio::SampleType::FLOAT_32 );

        target->write( audioBuffer.get(), end.currentPosition - start.currentPosition);
        isSampleSelected = true;
        //sample.setup(kMp3FileName);
        }
    catch( audio::AudioFileExc &exc ) {
        throw exc;
    }
}

void AudioAligner::setSample() {
    mixer.setInputBusCount(1);
    reverb = au::GenericUnit(kAudioUnitType_Effect, kAudioUnitSubType_MatrixReverb);
    distortion = au::GenericUnit(kAudioUnitType_Effect, kAudioUnitSubType_Distortion);
    fileToPlay.setFile(kOutputFileName);
    fileToPlay.connectTo(reverb).connectTo(distortion).connectTo(tap).connectTo(mixer, 0).connectTo(output);
    output.start();
    tap.getSamples(tapBuffer);
}

void AudioAligner::mouseDown(MouseEvent event) {
    //mBufferPlayerNode->start();
//    std::string hello("Hello!");
//    speechSynth.say(hello);
    if (abs(event.getX() - start.getXPosition()) < 5) {
        moveStart = true;
    } else if (abs(event.getX() - end.getXPosition()) < 5) {
        moveEnd = true;
    }
}

void AudioAligner::mouseDrag(MouseEvent event) {
    if( mBufferPlayerNode->isEnabled() )
        mBufferPlayerNode->seek( mBufferPlayerNode->getNumFrames() * event.getX() / getWindowWidth() );
}

void AudioAligner::mouseUp(MouseEvent event) {
    if (moveStart) {
        size_t frame = event.getX() * mBufferPlayerNode->getNumFrames() / ((float (8/7)) * getWindowWidth());
        start.setPosition(frame);
        mBufferPlayerNode->setLoopBegin(start.currentPosition);
        moveStart = false;
    } else if (moveEnd) {
        size_t frame = event.getX() * mBufferPlayerNode->getNumFrames() / ((float (8/7)) * getWindowWidth());
        end.setPosition(frame);
        mBufferPlayerNode->setLoopEnd(end.currentPosition);
        moveEnd = false;
    }
}

void AudioAligner::update() {
    if (isSampleSelected) {
        //sample.update();
        tap.getSamples(tapBuffer);
    }
}

PolyLine2 AudioAligner::drawSample() {
    int width = 700;
    int height = 200;
    // Taken from Cinder-AudioUnit samples :
    // https://github.com/admsyn/Cinder-AudioUnit/blob/master/samples/auComplexRouting/src/auComplexRoutingApp.cpp
    PolyLine2f waveform;

    if(!tapBuffer.empty()) {
        waveform.getPoints().reserve(tapBuffer.size());

        const float xStep = width / (float)tapBuffer.size();

        for(int i = 0; i < tapBuffer.size(); i++) {
            float x = i * xStep;
            float y = tapBuffer[i] * (height / 2.f) + (height / 2.f);
            ci::vec2 newPoint(x, y);
            //std::cout << tapBuffer[i] << std::endl;
            waveform.push_back(newPoint);
        }
    }

    return waveform;
}


}  // namespace myapp
