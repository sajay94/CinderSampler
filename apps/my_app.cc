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


AudioAligner::AudioAligner() {
    auto ctx = audio::Context::master();

    //AudioUnit demo
    reverb = au::GenericUnit(kAudioUnitType_Effect, kAudioUnitSubType_MatrixReverb);
    fileToPlay.setFile(app::loadAsset("out2.wav"));
    fileToPlay.connectTo(reverb).connectTo(tap).connectTo(mixer).connectTo(output);
    output.start();

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

    drawAudioPlayer();
    start.draw();
    end.draw();

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
    if( event.getCode() == KeyEvent::KEY_SPACE ) {
        if( mBufferPlayerNode->isEnabled() )
            mBufferPlayerNode->stop();
        else
            mBufferPlayerNode->setLoopBegin(start.currentPosition);
            mBufferPlayerNode->start(mBufferPlayerNode->getLoopBeginTime());
    } else if( event.getCode() == KeyEvent::KEY_w )
        this->exportFile();
}

void AudioAligner::fileDrop(FileDropEvent event) {
    fs::path filePath = event.getFile( 0 );
    getWindow()->setTitle( filePath.filename().string() );

    mSourceFile.reset();
    mSourceFile = audio::load( loadFile( filePath ) );

    // BufferPlayerNode can also load a buffer directly from the SourceFile.
    // This is safe to call on a background thread, which would alleviate blocking the UI loop.
    mBufferPlayerNode->loadBuffer( mSourceFile );

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
void AudioAligner::exportFile() {
    audio::BufferRef audioBuffer = mSourceFile->loadBuffer();

    try {
        const fs::path fileName = "/Users/family/Cinder/Projects/final-project-sajay94/assets/out2.wav";
        audio::TargetFileRef target = audio::TargetFile::create( fileName, mSourceFile->getSampleRate(), mSourceFile->getNumChannels() ); // INT_16
        //	audio::TargetFileRef target = audio::TargetFile::create( fileName, mSourceFile->getSampleRate(), mSourceFile->getNumChannels(), audio::SampleType::FLOAT_32 );

        target->write( audioBuffer.get() );

        }
    catch( audio::AudioFileExc &exc ) {
        throw exc;
    }
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

}  // namespace myapp
