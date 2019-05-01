#include "ofApp.h"

void ofApp::setupPiCam() {
    if(true){//!vidGrabber.isReady()){
        vidGrabber.close();

        settings.sensorWidth = camWidth; //default 1280
        settings.sensorHeight = camHeight; //default 720
        settings.framerate = framerate;

        settings.enableTexture = true; //default true
     
        settings.enablePixels = true;
        settings.sensorMode = 7;
        settings.ISO = 0;
        settings.brightness = 50;
        settings.contrast = 0;
        settings.sharpness = 0;
        settings.saturation = 0;
        settings.autoISO = false;
        settings.autoShutter = false;
        
        vidGrabber.setup(settings);

        resize = 1;
    }
    else{
        ofLog() << "vidGrabber is already open";
    }

}

void ofApp::setDefaultFrame(){
    ofImage img;
    img.allocate(camWidth, camHeight, OF_IMAGE_COLOR_ALPHA);
    img.setColor(ofColor::red);
    img.update();
    default_frame = img.getPixels();

}

//--------------------------------------------------------------
void ofApp::setup(){

    ofLog() << "setup";
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetLogLevel("ofThread", OF_LOG_ERROR);
	//ofSetVerticalSync(false);
	ofEnableAlphaBlending();
		
	consoleListener.setup(this);
	
    framerate = 25;
    ofSetFrameRate(framerate);
    
    camWidth = 640;  // try to grab at this size.
    camHeight = 480;
    //resizeW = camWidth;
    //resizeH = camHeight;

    setupPiCam();
    setupMidi();

    ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetLogLevel("ofThread", OF_LOG_ERROR);

    detours = {detour0, detour1, detour2, detour3};
    current_detour = 0;
    is_playing = false;
    is_recording = false;
    record_loop = false;
    detour_position = 0;
    detour_speed = 1;
    detour_start = 0;
    detour_end = 0;
    memory_full = false;
    sample_resolution = 0;
    sample_speed = 1;
    mix_position = 0.0;
    mix_selection = 0.0;

    showInfo = false;

    mix_x2 = 0.5;
    mix_x3 = 0.0;
    mix_x4 = 0.0;

    is_delay = false;
    delay_size = 10;

    normPosition = 0;
    normStart = 0;
    normEnd = 0;

    write_index = 0;

    setDefaultFrame();

    in_texture.allocate(camWidth, camHeight, GL_RGBA);
    detour_texture.allocate(camWidth, camHeight, GL_RGBA);
    
    //ofSetFullscreen(1);
    fbo.allocate(ofGetWidth(), ofGetHeight());

    shader.load("mixShader");
    fbo.begin();
        //ofClear(0, 250, 250, 250);
        ofClear(0, 0, 0, 0);
    fbo.end();

/*
#ifndef TARGET_RASPBERRY_PI
    
    vidGrabber.setDeviceID(0);
    vidGrabber.setDesiredFrameRate(framerate);
    vidGrabber.initGrabber(camWidth, camHeight);
    make run

#endif
*/
}


//--------------------------------------------------------------
void ofApp::update(){


    ofBackground(100, 100, 100);
    //readMidiInput();
    //ofLog() << "vid grabber isReady is " << vidGrabber.isReady() << "vidGrabber isFrameNew is " << vidGrabber.isFrameNew();
    //vidGrabber.draw();
    if(vidGrabber.isFrameNew()){
        in_frame = getFrameFromInput();
        detour_frame = getFrameFromDetour();
        mixFrames(in_frame, detour_frame);         

        if(is_recording){
            fbo.readToPixels(out_frame);
            addFrameToDetour(out_frame); }  
    }   
}

//--------------------------------------------------------------
void ofApp::draw(){

    fbo.draw(0, 0, ofGetWidth(), ofGetHeight());

    if(showInfo){printState();}
    //
}

//--------------------------------------------------------------

ofPixels ofApp::getFrameFromInput(){

    ofPixels this_frame;
    int ww = vidGrabber.getWidth() ;
    int hh = vidGrabber.getHeight() ;
    int rw = (float)ww * resize;
    int rh = (float)hh * resize;
    
    
#ifdef TARGET_RASPBERRY_PI
    
    this_frame = vidGrabber.getPixels();
    //pixels.setFromPixels(vidGrabber.getPixels().getData(), ww, hh, 3);
    
#else
    
    this_frame.setFromPixels(vidGrabber.getPixels().getData(), ww, hh, 3);
    
#endif
    
    this_frame.resize(rw, rh);

    return this_frame;
}

ofPixels ofApp::getFrameFromDetour(){

    if(detours[current_detour].size() == 0){ return default_frame; }


    ofPixels this_frame = detours[current_detour][detour_position];

    if(is_playing){
        detour_position_part = subsetModFloat(detour_position_part + detour_speed);
        detour_position = (int)floor(detour_position_part);
    }
    if(is_delay){
        int current_size = detours[current_detour].size();
        detour_position = current_size - 1;
        this_frame = detours[current_detour][detour_position];
        if(current_size > delay_size){
            detours[current_detour].erase(detours[current_detour].begin()+delay_size,detours[current_detour].begin()+current_size);
        }
        detours[current_detour].insert(detours[current_detour].begin(),in_frame);
    }

    return this_frame;
}



void ofApp::mixFrames(ofPixels in_frame, ofPixels detour_frame){

    int ww = in_frame.getWidth();
    int hh = in_frame.getHeight();

    in_texture.loadData(in_frame.getData(), ww, hh, GL_RGBA);

    detour_texture.loadData(detour_frame.getData(), ww, hh, GL_RGBA);


     fbo.begin();
        shader.begin();
            shader.setUniform1f("u_time", ofGetElapsedTimef());
            shader.setUniform2f("u_resolution", ofGetWidth(), ofGetHeight());
            shader.setUniform1f("u_x0", mix_position);
            shader.setUniform1f("u_x1", mix_selection);
            shader.setUniform1f("u_x2", mix_x2);
            shader.setUniform1f("u_x3", mix_x3);
            shader.setUniform1f("u_x4", mix_x4);
            shader.setUniformTexture("u_tex0", in_texture, in_texture.getTextureData().textureID);
            shader.setUniformTexture("u_tex1", detour_texture, detour_texture.getTextureData().textureID);
            in_texture.draw(0,0, ofGetWidth(), ofGetHeight());
        shader.end();
     fbo.end();  
    
}

void ofApp::addFrameToDetour(ofPixels in_frame){
    if(record_loop | memory_full ){
        detours[current_detour][write_index] = in_frame;
        write_index = subsetMod(write_index + 1);
    }
    else{
        detours[current_detour].push_back(in_frame);
    }
}


void ofApp::keyPressed(int key){
    switch(key){
        case 'r':
            is_recording = !is_recording;
            break;
        case 'i':
            showInfo = !showInfo;
            break;
        case 't':
            record_loop = !record_loop;
            break;
        case 'a':
            is_delay = !is_delay;
            break;
        case 'z':
            delay_size --;
            if(delay_size < 0){delay_size = 0;}
            break;
        case 'x':
            delay_size ++;
            break;
        case 'w':
            is_playing = !is_playing;
            break;
        case 'c':
            detours[current_detour].clear();
            break;
        case 'k':
            mix_position -= 0.2;
            if(mix_position < 0){mix_position = 0;}
            break;
        case 'l':
            mix_position += 0.2;
            if(mix_position > 1){mix_position = 1;}
            break;
        case '[':
            detour_speed -= 0.2;
            if (detour_speed < -3){detour_speed = -3;}
            break;
        case ']':
            detour_speed += 0.2;
            if (detour_speed > 3){detour_speed = 3;}
            break;
        case '=':
            detour_speed *= -1;
            break;
        case 'v':
            normStart -= 0.1;
            if (normStart < 0){normStart = 0;}
            setStart(normStart);
            break;
        case 'b':
            normStart += 0.1;
            if (normStart > 1){normStart = 1;}
            setStart(normStart);
            break;
        case 'n':
            normEnd -= 0.1;
            if (normEnd < 0){normEnd = 0;}
            setEnd(normEnd);
            break;
        case 'm':
            normEnd += 0.1;
            if (normEnd > 1){normEnd = 1;}
            setEnd(normEnd);
            break;
        case ',':
            normPosition -= 0.1;
            if (normPosition < 0){normPosition = 0;}
            setPosition(normPosition);
            break;
        case '.':
            normPosition += 0.1;
            if (normPosition > 1){normPosition = 1;}
            setPosition(normPosition);
            break;
        case 'o':
            detour_position = subsetMod(detour_position - 1);
            detour_position_part = (float)detour_position;
            break;
        /*case 'p':
            detour_position = subsetMod(detour_position + 1);
            detour_position_part = (float)detour_position;
            break;
        case 'p':
            vidGrabber.setup(settings);
            //setupPiCam();
            break;
        case ' ':
            vidGrabber.close();
            break; */
        case '<':
            setupMidi();
            break;
        case 'q':
            ofExit();
            break;
        case '0':
            current_detour = 0;
            detour_start = 0;
            detour_end = 0;
            detour_position_part = 0;
            detour_position = 0;
            break;
        case '1':
            current_detour = 1;
            detour_start = 0;
            detour_end = 0;
            detour_position_part = 0;
            detour_position = 0;
            break;
        case '2':
            current_detour = 2;
            detour_start = 0;
            detour_end = 0;
            detour_position_part = 0;
            detour_position = 0;
            break;
        case '3':
            current_detour = 3;
            detour_start = 0;
            detour_end = 0;
            detour_position_part = 0;
            detour_position = 0;
            break;
    }
  
}

void ofApp::setStart(float value){
    detour_start = 0 + (int)floor(value*((float)getEndFrame()));
    if(detour_position < detour_start){
        detour_position = detour_start;
        detour_position_part = (float)detour_position;
        }
}

void ofApp::setEnd(float value){
    detour_end = detour_start + 1 + (int)floor(value*((float)detours[current_detour].size() - detour_start -1));
    if(detour_position > detour_end){
        detour_position = detour_end;
        detour_position_part = (float)detour_position;
        }
}

void ofApp::setPosition(float value){
    detour_position = detour_start + (int)floor(value*(float)getEndFrame());
}

void ofApp::setSpeed(float value){
    detour_speed = -5.0 + 10.0*value;
}


void ofApp::readMidiInput(ofxMidiMessage& message){
    
    //for(unsigned int i = 0; i < midiMessages.size(); ++i){
                
        //ofxMidiMessage &message = midiMessages[i];
        if(message.status == MIDI_NOTE_ON){
            switch(message.pitch){
                case 36:
                    keyPressed((int)'0');
                    break;
                case 37:
                    keyPressed((int)'1');
                    break;
                case 38:
                    keyPressed((int)'2');
                    break;
                case 39:
                    keyPressed((int)'i');
                    break;
                case 40:
                    keyPressed((int)'r');
                    break;
                case 41:
                    keyPressed((int)'t');
                    break;
                case 42:
                    keyPressed((int)'c');
                    break;
                case 43:
                    ofLog() << "pressed once";
                    //keyPressed((int)'p');
                    break;        
            }
        }
        else if(message.status == MIDI_CONTROL_CHANGE){
            float normValue;
            normValue = (float)message.value / (float)127;
            switch(message.control){
                case 1:
                    if(is_playing){ setSpeed(normValue);  }
                    else { setPosition(normValue); }
                    break;
                case 2:
                    mix_position = normValue;
                    break;
                case 3:
                    setStart(normValue);
                    break; 
                case 4:
                    setEnd(normValue);
                    break;
                case 5:
                    mix_selection = normValue;
                    break;
                case 6:
                    mix_x2 = normValue;
                    break;
                case 7:
                    mix_x3 = normValue;
                    break;
                case 8:
                    mix_x4 = normValue;
                    break; 
            }         
        }
        
        //midiMessages.erase(midiMessages.begin());    
    //}
    //midiMessages.clear()
    
}

void ofApp::printState(){
    stringstream info;
	info << "APP FPS: " << ofGetFrameRate() << "\n";
	info << "current_detour: " << current_detour << "\n";
    info << "is_playing: " << is_playing << "\n";
    info << "is_recording: " << is_recording << "\n";
    info << "record_loop: " << record_loop << "\n";
    info << "detour_position: " << detour_position << "\n";
    info << "detour_size: " << detours[current_detour].size() << "\n";
    info << "detour_speed: " << detour_speed << "\n";
    info << "detour_start: " << detour_start << "\n";
    info << "detour_end: " << detour_end << "\n";
    info << "memory_full: " << memory_full << "\n";
    info << "mix_position: " << mix_position << "\n";
    info << "mix_selection: " << getMixSelectionName() << "\n";
    //info << "sample_resolution: " << sample_resolution << "\n";
    //info << "sample_speed: " << sample_speed << "\n";
    info << "is_delay: " << is_delay << "\n";
    info << "delay_size: " << delay_size << "\n";

	//info <<	filterCollection.filterList << "\n";
    ofDrawBitmapStringHighlight(info.str(), 50, 50, ofColor::black, ofColor::yellow);
}

string ofApp::getMixSelectionName(){
    if(0.0 <= mix_selection && mix_selection < 0.2 ){return "mixVeritcalWipe";}
    else if(0.2 <= mix_selection && mix_selection < 0.4 ){return "mixLuma0";}
    else if(0.4 <= mix_selection && mix_selection < 0.6 ){return "mixLuma1";}
    else if(0.6 <= mix_selection && mix_selection < 0.8 ){return "mixBlend";}
    else if(0.8 <= mix_selection && mix_selection <= 1.0 ){return "mixBlendAdd";}
    return "nothing";
}

int ofApp::getEndFrame(){
    int end = detours[current_detour].size() - 1;
    if(detour_end > 0){end = detour_end;}
    return end;
}

int ofApp::subsetMod(int amount){
    int newAmount = ( (amount - detour_start) % (getEndFrame() + 1 - detour_start) );
    if (newAmount < 0){ newAmount += getEndFrame() + 1; }
    return newAmount + detour_start;
}

float ofApp::subsetModFloat(float amount){
    // x mod y for a float is x - y*floor(x/y)
    float x = amount - (float)detour_start;
    float y = (float)getEndFrame() - (float)detour_start;
    return x - y*floor(x/y) + (float)detour_start;
}

void ofApp::setupMidi(){
    if( !midiIn.isOpen()){
            	// print input ports to console
	    midiIn.listInPorts();

	    midiIn.openPort(1);
	    // don't ignore sysex, timing, & active sense messages,
	    //midiIn.ignoreTypes(false, false, false);
	
	    // add ofApp as a listener
	    midiIn.addListener(this);
	    // print received messages to the console
	    //midiIn.setVerbose(true);
    }
    else{
        ofLog() << "is already open !";
}

}

void ofApp::newMidiMessage(ofxMidiMessage& msg) {

    readMidiInput(msg);
	// add the latest message to the message queue
	//midiMessages.push_back(msg);

	// remove any old messages if we have too many
	//while(midiMessages.size() > maxMessages) {
	//	midiMessages.erase(midiMessages.begin());
	//}
}

void ofApp::exit() {
   midiIn.closePort();
   midiIn.removeListener(this);
}


void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
    keyPressed((int)e.character);
}
