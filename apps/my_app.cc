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
    end.setUp(mBufferPlayerNode->getNumFrames(), mBufferPlayerNode->getNumFrames());

    // connect and enable the Context
    mBufferPlayerNode >> ctx->getOutput();
    ctx->enable();
}

void AudioAligner::setup() {
    cinder::gl::enableDepthWrite();
    cinder::gl::enableDepthRead();
    // Cinder-UI --> fix mouse calibration

    mUi = SuperCanvas::create( "Output Controls:" );

    mUi->addSliderf( "Volume", &volume );
    mUi->addSliderf( "Pan", &pan, -1.0, 1.0, Sliderf::Format().crossFader() );
    mUi->autoSizeToFitSubviews();
    mUi->setOrigin(vec2(0, 400));

}

void AudioAligner::draw() {

    //mUi->load( getSaveLoadPath() );
    gl::clear();
    gl::enableAlphaBlending();


    if (isSampleSelected) {
        auto current = samples.at(currentSample);
        drawAudioPlayer(kSamplePosition, current.getSampleWaveformPlot(), current.getSampleBufferPlayer());
    }
    drawAudioPlayer(kAudioPosition, mWaveformPlot, mBufferPlayerNode);
    start.draw(), end.draw();
}

void AudioAligner::drawAudioPlayer(const Rectf& size, WaveformPlot plot, BufferPlayerNodeRef player) {
    gl::setMatricesWindow( getWindowSize());
    gl::color( ColorA( 0, 1, 0, 0.7f ) );
    gl::drawStrokedRect(size);
    plot.load( player->getBuffer(), size );
    plot.draw(size.getX1(), size.getY1());

    // draw the current play position
    float readPos = (.875) * (float)getWindowWidth() * player->getReadPosition() / player->getNumFrames();
    gl::color( ColorA( 0, 1, 0, 0.7f ) );
    gl::drawSolidRect( Rectf( readPos - 2, 0, readPos + 2, 100 ) );

}

void AudioAligner::keyDown(KeyEvent event) {
    if( event.getCode() == KeyEvent::KEY_SPACE ) {
        if (!isSampleSelected) {
            if( mBufferPlayerNode->isEnabled() )
                mBufferPlayerNode->stop();
            else {
                mBufferPlayerNode->setLoopBegin(start.currentPosition);
                mBufferPlayerNode->start(mBufferPlayerNode->getLoopBeginTime());
            }
        } else {
            auto current = samples.at(currentSample);
            if( current.getSampleBufferPlayer()->isEnabled() )
                current.getSampleBufferPlayer()->stop();
            else {
                current.getSampleBufferPlayer()->start(current.getSampleBufferPlayer()->getLoopBeginTime());
            }
        }
    } else if( event.getCode() == KeyEvent::KEY_s ) {
        setUpSample();
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

void AudioAligner::setUpSample() {

    audio::BufferRef sampleBuffer = mBufferPlayerNode->getBuffer();
    sampleBuffer->copyOffset(*sampleBuffer, end.currentPosition - start.currentPosition, 0, start.currentPosition);

    currentSample++;
    fs::path newSamplePath = "../assets/out" + to_string(currentSample) + ".wav";

    try {
        audio::TargetFileRef target = audio::TargetFile::create( newSamplePath,
                mSourceFile->getSampleRate(), mSourceFile->getNumChannels() );
        target->write( sampleBuffer.get(), end.currentPosition - start.currentPosition);
        isSampleSelected = true;

        }
    catch( audio::AudioFileExc &exc ) {
        throw exc;
    }

    // creates new SampleController using the the newly created sample
    auto newSample = SampleController();
    samples.push_back(newSample);
    samples.at(currentSample).setup(newSamplePath);
}



void AudioAligner::mouseDown(MouseEvent event) {
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
        size_t frame = event.getX() * mBufferPlayerNode->getNumFrames() / (kWindowToWidgetRatio * getWindowWidth());
        start.setPosition(frame);
        mBufferPlayerNode->setLoopBegin(start.currentPosition);
        moveStart = false;
    } else if (moveEnd) {
        size_t frame = event.getX() * mBufferPlayerNode->getNumFrames() / (kWindowToWidgetRatio * getWindowWidth());
        end.setPosition(frame);
        mBufferPlayerNode->setLoopEnd(end.currentPosition);
        moveEnd = false;
    }
}

// Used to quickly load mUi default from json
//fs::path AudioAligner::getSaveLoadPath()
//{
//    fs::path path = getAssetPath( "" );
//    path += "/" + mUi->getName() + ".json";
//    return path;
//}



}  // namespace myapp
