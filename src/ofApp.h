#pragma once

#include "ofMain.h"
#ifdef TARGET_RASPBERRY_PI
#include "ofxOMXVideoGrabber.h"
#include "ofxMidi.h"
#include "TerminalListener.h"

#endif

class ofApp : public ofBaseApp, public KeyListener, public ofxMidiListener{

public:

    void setup();
    void update();
    void draw();
    void exit();
    
    void onCharacterReceived(KeyListenerEventData& e);
    void keyPressed(int key);
    
    void setupPiCam();
    void setupMidi();

#ifdef TARGET_RASPBERRY_PI
    ofxOMXVideoGrabber vidGrabber;
#else
    //ofVideoGrabber vidGrabber;
#endif
    	ofxOMXCameraSettings settings;
    TerminalListener consoleListener;

    int camWidth;
    int camHeight;
    
    
    vector<ofPixels> frames;
    bool isRecording;
    float frameIndex;
    
    float framerate;
    int resizeW, resizeH;
    float resize;
    
// midi stuff 

	void newMidiMessage(ofxMidiMessage& eventArgs);
	void readMidiInput(ofxMidiMessage& message);
	ofxMidiIn midiIn;
	std::vector<ofxMidiMessage> midiMessages;
	std::size_t maxMessages = 10; //< max number of messages to keep track of

// detour methods
    ofPixels getFrameFromInput();
    ofPixels getFrameFromDetour();
    void setDefaultFrame();
    void mixFrames(ofPixels in_frame, ofPixels detour_frame);
    void addFrameToDetour(ofPixels in_frame);
    void printState();

    void setStart(float value);
    void setEnd(float value);
    void setPosition(float value);
    void setSpeed(float value);

    int getEndFrame();
    int subsetMod(int amount);
    float subsetModFloat(float amount);
    string getMixSelectionName();

// detour variables

    ofFbo fbo;
    ofShader shader;

    vector< vector<ofPixels> > detours;
    vector<ofPixels> detour0;
    vector<ofPixels> detour1;
    vector<ofPixels> detour2;
    vector<ofPixels> detour3;

    float normStart;
    float normEnd;
    float normPosition;
    int write_index;
    int counter;
    float detour_position_part;

    ofPixels in_frame;
    ofPixels detour_frame;
    ofPixels out_frame;
    ofPixels default_frame;
    ofTexture in_texture;
    ofTexture detour_texture;
    
    bool showInfo;
    int current_detour;
    bool is_playing;
    bool is_recording;
    bool record_loop;
    int detour_position;
    float detour_speed;
    int detour_start;
    int detour_end;
    bool memory_full;
    float mix_position;
    float mix_selection;
    int sample_resolution;
    float sample_speed;

    float mix_x2, mix_x3, mix_x4;

    bool is_delay;
    int delay_size;

};
