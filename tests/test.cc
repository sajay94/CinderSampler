// Copyright (c) 2020 CS126SP20. All rights reserved.

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <cinder/Rand.h>
#include <mylibrary/audio_bound.h>
#include <mylibrary/sample_controller.h>

#include <mylibrary/example.h>

using namespace myapp;

TEST_CASE("AudioBound position", "[AudioBound]") {
    auto testBound = AudioBound();
    testBound.setUp(0, 10000);
    REQUIRE(testBound.getCurrentFrame() == 0);
}

TEST_CASE("AudioBound frames", "[AudioBound]") {
    auto testBound = AudioBound();
    testBound.setUp(5000, 10000);
    REQUIRE(testBound.getNumFrames() == 10000);
}

TEST_CASE("AudioBound setter", "[AudioBound]") {
    auto testBound = AudioBound();
    testBound.setUp(5000, 10000);
    REQUIRE(testBound.getCurrentFrame() == 5000);
    testBound.setPosition(0);
    REQUIRE(testBound.getCurrentFrame() == 0);
}

TEST_CASE("SampleController BufferPlayer", "[SampleController]") {
    auto testSample = SampleController();
    testSample.setup("sample.wav");
    int size = testSample.getSampleBufferPlayer()->getNumFrames();
    REQUIRE(size > 0);
}

TEST_CASE("SampleController Gain", "[SampleController]") {
    auto testSample = SampleController();
    testSample.setup("sample.wav");
    int size = testSample.getVolume();
    REQUIRE(size == 1);
    REQUIRE(size == testSample.getParamVolume());
}

TEST_CASE("SampleController Pan", "[SampleController]") {
    auto testSample = SampleController();
    testSample.setup("sample.wav");
    REQUIRE(testSample.getPan() == 0.5f);
}

TEST_CASE("SampleController LowPass", "[SampleController]") {
    auto testSample = SampleController();
    testSample.setup("sample.wav");
    int size = testSample.getLowPass();
    REQUIRE(size == 1600);
}

TEST_CASE("SampleController HighPass", "[SampleController]") {
    auto testSample = SampleController();
    testSample.setup("sample.wav");
    int size = testSample.getHighPass();
    REQUIRE(size == 0);
}

TEST_CASE("SampleController Gain Setter", "[SampleController]") {
    auto testSample = SampleController();
    testSample.setup("sample.wav");
    testSample.setVolume(.95);
    REQUIRE(testSample.getVolume() == .95f);
}

TEST_CASE("SampleController Pan Setter", "[SampleController]") {
    auto testSample = SampleController();
    testSample.setup("sample.wav");
    testSample.setPan(.95);
    REQUIRE(testSample.getPan() == .95f);
}

TEST_CASE("SampleController LowPass Setter", "[SampleController]") {
    auto testSample = SampleController();
    testSample.setup("sample.wav");
    testSample.setLowPass(1000);
    REQUIRE(testSample.getLowPass() == 1000);
}

TEST_CASE("SampleController HighPass Setter", "[SampleController]") {
    auto testSample = SampleController();
    testSample.setup("sample.wav");
    testSample.setHighPass(300);
    REQUIRE(testSample.getHighPass() == 300);
}

TEST_CASE("SampleController Gain Automation", "[SampleController]") {
    auto testSample = SampleController();
    testSample.setup("sample.wav");
    testSample.setParamVolume(.95);
    testSample.setRampVolume();
    REQUIRE(testSample.getParamVolume() == .95f);
}

TEST_CASE("SampleController Pan Automation", "[SampleController]") {
    auto testSample = SampleController();
    testSample.setup("sample.wav");
    testSample.setParamPan(.95);
    testSample.setRampPan();
    REQUIRE(testSample.getParamPan() == .95f);
}