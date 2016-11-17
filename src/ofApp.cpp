//To do:
//Use memorization to keep all random combinations
//Use XmlSettings to save memorization into table
//Generate matrix up, down, left, right of screen
//Random search algorithm
//Snapshot

#include "ofApp.h"

using namespace std;
using namespace boost::multiprecision;

//--------------------------------------------------------------
void ofApp::setup(){

    //Set background colour
    ofBackground(0, 0, 0);

    //Intialize width and height of screen
    w = ofGetWidth();
    h = ofGetHeight();

    //Wait for each frame when faster than 60 fps to
    //avoid screen tearing
    ofSetVerticalSync(true);

    //Set fullscreen toggle value
    mFullscreen = false;
    tglHelp = false;
    getMesh = false;
    tglD = false;

    //Load and set font for title
    title.load("AvantGarde-Book.ttf", 32, true, false, true);

    //---------------------------------------------------
    //CAMERA AND 3DVIEW
    // this sets the camera's distance from the object
    cam.setDistance(100);
    cam.setPosition(ofVec3f(0, 0, 800));
    cam.lookAt(ofVec3f(0, 0, 0));

    bUseEasyCam = true;
    ofSetCircleResolution(40);
    bShowHelp = false;
    // Setting light position
    light.setPosition(0, 0, 2000);

    //---------------------------------------------------
    //MESH GENERATION
    //Set count of sub meshes to be generated
    numOfX = w / 10;
    numOfY = h / 10;
    //Calculate space between sub meshes
    spacingX = w  / numOfX;
    spacingY = h / numOfY;
    //Initialize starting position for tiling in x and y
    startingX = spacingX / 2;
    startingY = spacingY / 2;

    //Set starting values of mesh generation
    fraNr = 324;
    fn = 345;

    snapSPath = "";

    //Declare 10 colours which relates to value 0..9
    black = ofColor(0, 0, 0);
    red = ofColor(255, 0, 0);
    orange = ofColor(255, 128, 0);
    yellow = ofColor(255, 255, 0);
    green = ofColor(0, 255, 0);
    turqoise = ofColor(0, 255, 255);
    blue = ofColor(0, 0, 255);
    lilac = ofColor(128, 0, 255);
    violet = ofColor(255, 0, 255);
    white = ofColor(255, 255, 255);

    //---------------------------------------------------
    //GUI - Initialize and set gui widgets

    //Set gui font
    font.load("fonts/AvantGarde-Book.ttf", 24);

    //Create gui layout
    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
    gui->setWidth(200);

    //Add widgets
    gui->addToggle("FULLSCREEN", false);
    gui->addTextInput("FIBONACCI", "ENTER HERE");
    gui->addTextInput("FRACTION", "ENTER HERE");
    gui->addButton("GENERATE");
    gui->addButton("TAKE SNAPSHOT");
    gui->addToggle("TOGGLE Z-VALUE", false);
    gui->addToggle("ORTHO/PERSP", false);
    gui->addToggle("HELP", false);
    //Add footer will add a gui hide option
    gui->addFooter();

    // finally register a few callbacks to listen for specific component events //
    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->onTextInputEvent(this, &ofApp::onTextInputEvent);
}

//--------------------------------------------------------------
void ofApp::update(){
    //GUI update
    gui->update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    // this uses depth information for occlusion
    // rather than always drawing things on top of each other
    ofEnableDepthTest();
    light.enable();
    //Start scene drawn by camera
    if (bUseEasyCam == true){
        cam.begin();
        ofScale(1,-1,-1);
    }
    else {
        orthoCam.begin();


    }
    //Generate a mesh when set to true
    if(getMesh == true) genMesh();
    //End area which will be drawn by the camera
    if (bUseEasyCam == true) cam.end();
    else orthoCam.end();

    //Disable
    ofDisableDepthTest();

    ofDisableLighting();
    //Set Color for title background
    ofSetColor(0);
    //Draw rectangle
    ofDrawRectangle(0, 0, w / 1.5, h / 17);
    //set font colour
    ofSetColor(255);
    //Draw title
    title.drawString("AFTER 1.618...", 10, 40);

    for(int i = 0; i < 10; i++){
        //Free section from global offset
        ofPushMatrix();
        ofTranslate(300, 40);
        //Set different color for each number
        if(i == 0){
            //Black needs a rectangle behind
            color = ofColor(black);
            ofSetColor(white);
            ofDrawRectangle(-3,-35, 30, 40);
        }
        if(i == 1) color = ofColor(red);
        if(i == 2) color = ofColor(yellow);
        if(i == 3) color = ofColor(orange);
        if(i == 4) color = ofColor(green);
        if(i == 5) color = ofColor(turqoise);
        if(i == 6) color = ofColor(blue);
        if(i == 7) color = ofColor(lilac);
        if(i == 8) color = ofColor(violet);
        if(i == 9) color = ofColor(white);

        //Set color for each number
        ofSetColor(color);
        //While drawing number behind title
        title.drawString(ofToString(i), i * 40, 0);
        //Leave independant area
        ofPopMatrix();
    }

    if(tglHelp == true){
        //Draw navigation segment
        ofSetCircleResolution(64);
        //Take entire viewport as
        ofRectangle vp = ofGetCurrentViewport();
        float r = MIN(vp.width, vp.height) * 0.5f;
        float x = vp.width * 0.5f;
        float y = vp.height * 0.5f;
        ofPushStyle();

        ofSetLineWidth(3);
        ofSetColor(255, 255, 0);
        ofNoFill();
        glDepthMask(false);
        ofDrawCircle(x, y, r);
        glDepthMask(true);
        ofPopStyle();

        string msg = string("Using mouse inputs to navigate (press 'c' to toggle): ") + (cam.getMouseInputEnabled() ? "YES" : "NO");
        msg += "\n\nLEFT MOUSE BUTTON DRAG:\nStart dragging INSIDE the yellow circle -> camera XY rotation .\nStart dragging OUTSIDE the yellow circle -> camera Z rotation (roll).\n\n";
        msg += "LEFT MOUSE BUTTON DRAG + TRANSLATION KEY (" + ofToString(cam.getTranslationKey()) + ") PRESSED\n";
        msg += "OR MIDDLE MOUSE BUTTON (if available):\n";
        msg += "move over XY axes (truck and boom).\n\n";
        msg += "LEFT MOUSE BUTTON DOUBLE CLICK: Reset camera position.\n";
        msg += "\nRIGHT MOUSE BUTTON:\n";
        msg += "move over Z axis (dolly)";
        msg += "\n\nGENERATE VISUALS:\n1.Enter a position of the Fibonacci sequence\n2.Enter a position of a decimals fraction\n3.Press GENERATE button to create the related visuals";
        msg += "\n\nThe entered numbers will create the value in the middle of screen.\nAll values around this center will be generated\nautomatically regarding to the size of set visuals.";
        msg += "\n\nCAMERA:\nPress ORTHO/PERSP to toggle between 2 views\n-> orthographic and perspective view.";
        msg += "\n\nZ-VALUE:\nPress button to activate scale of visuals in Z axis\nregarding to the actual value (also visible through color).";
        msg += "\n\nSNAPSHOT:\nPress SNAPSHOT button to create an image of the screen as is.\nThe picture will be saved in the data folder with previous entered\nnumbers as file name.";

        msg += "\n\nfps: " + ofToString(ofGetFrameRate(), 2);
        ofDrawBitmapStringHighlight(msg, 10, 80);
    }
    //Draw GUI widgets
    gui->draw();
}

//--------------------------------------------------------------
void ofApp::genFib(){

    //Ring buffer to store previous two fibonacci number, index it with [i%2]
    //Use defined function get_buffer(i)
    BIG buffer[2]={ 1, 1 };

    //Count loop is used if seeked fibonacci number is gt 2
    if(fn > 2){
      for(unsigned int i = 3; i < fn; i++){
        get_buffer(i) = get_buffer(i-1) + get_buffer(i-2);
        //Add fibonacci number to array
        fibSeq.push_back(get_buffer(i));
        //Only hold the numbers needed for display -> erase the rest
        //needed to reach the range dedicated by entered number
        if(i < fn - numY) fibSeq.erase(fibSeq.begin());
      }
    }
    //Print out Fibonacci number of entered position in sequence
    cout << "Fibonacci[" << fn << "] is " << get_buffer(fn-1) << std::endl;
}

//--------------------------------------------------------------
void ofApp::genFra(){

    //Fill fraPos for each fibSeq entry
    for(unsigned int row = 0; row < fibSeq.size() - 1; row++){
        //The first number always will be one
        initFib = fibSeq[row + 1] / fibSeq[row];
        //Get the rest between fibonacci partners
        rest = fibSeq[row + 1] - fibSeq[row];
        //Iterate through each fraction till it reveals
        //all number necessary for display
        for (unsigned int col = 1; col < fraNr; col++){
            if(col < fraNr - numX){
                //Multiply rest by 10 to get next
                //fraction number through division with first
                //Fibonacci number of the couple by row
                initFib = (rest * 10) / fibSeq[row];
                //Convert number into a string
                fibBuf = ofToString(initFib);
                //Select only first number
                fibBuf2 = fibBuf[0];
                //Convert it back to an integer
                initFib = ofToInt(fibBuf2);
                //Generate next rest
                rest = (rest * 10) - (fibSeq[row] * initFib);
            }
            else if(col >= fraNr - numX && col < fraNr){
                //Do the same thing but enter fraction number into fraPos vector
                initFib = (rest * 10) / fibSeq[row];
                //Convert number into a string
                fibBuf = ofToString(initFib);
                fibBuf2 = fibBuf[0];
                //This is still a BIG number
                initFib = ofToInt(fibBuf2);
                //Convert integer into normal integer
                initFibInt = ofToInt(fibBuf2);
                //To get it added to the array for visualization
                fraPos.push_back(initFibInt);
                //Calculate next rest
                rest = (rest * 10) - (fibSeq[row] * initFib);
            }
        }
    }
    //Enter all fraction numbers from 1 dimensional array
    //into a 2D array, which builds basis for visualization
    for(int x = 0; x < numX; x++){
        for(int y = 0; y < numY; y++){
            countX = x + (y * numX);
            meshVals[x][y] = fraPos[countX];
        }
    }
}

//--------------------------------------------------------------
void ofApp::genMesh(){

    //Generate 2D/3D representation of
    for(int x = 0; x < numX; x++){
        for(int y = 0; y < numY; y++){
            //Set position of tiles
            locX = x * spacingX + spacingX / 2;
            locY = y * spacingY + spacingY / 2;
            //Get number from array
            value = meshVals[x][y];
            //Set colour of cube regarding to 0...9
            //and z dimension of cubes
            if(tglD == true){
                if(value == 0){
                    color = ofColor(black);
                    d = -2;
                }
                if(value == 1){
                    color = ofColor(red);
                    d = -4;
                }
                if(value == 2){
                    color = ofColor(yellow);
                    d = -6;
                }
                if(value == 3){
                    color = ofColor(orange);
                    d = -8;
                }
                if(value == 4){
                    color = ofColor(green);
                    d = -10;
                }
                if(value == 5){
                    color = ofColor(turqoise);
                    d = -12;
                }
                if(value == 6){
                    color = ofColor(blue);
                    d = -14;
                }
                if(value == 7){
                    color = ofColor(lilac);
                    d = -16;
                }
                if(value == 8){
                    color = ofColor(violet);
                    d = -18;
                }
                if(value == 9){
                    color = ofColor(white);
                    d = -20;
                }
            }
            //In case the 3D effect is switch off in GUI
            //do the same thing like above, but use only 2 as Z scale
            if(tglD == false){
                d = -2;
                if(value == 0) color = ofColor(black);
                if(value == 1) color = ofColor(red);
                if(value == 2) color = ofColor(yellow);
                if(value == 3) color = ofColor(orange);
                if(value == 4) color = ofColor(green);
                if(value == 5) color = ofColor(turqoise);
                if(value == 6) color = ofColor(blue);
                if(value == 7) color = ofColor(lilac);
                if(value == 8) color = ofColor(violet);
                if(value == 9) color = ofColor(white);
            }
            //Set colour defined by if loops above
            ofSetColor(color);
            ofPushMatrix();
            //Using matrix to zero z position of cube in offset
            ofTranslate(-w/2, -h/2, d / 2);
            //Activate shape filling
            ofFill();
            //Draw box
            ofDrawBox(locX, locY, 0, 10, 10, d);
            ofPopMatrix();
        }
    }
}

//--------------------------------------------------------------+
void ofApp::onButtonEvent(ofxDatGuiButtonEvent e)
{
    if (e.target->is("FULLSCREEN")) toggleFullscreen();
    cout << "onButtonEvent: " << e.target->getLabel() << " " << e.target->getEnabled() << endl;
    if (e.target->is("HELP")) toggleHelp();
    if (e.target->is("GENERATE")){
        //Clear arrays before generating new sequences
        fibSeq.clear();
        fraPos.clear();
        //Call number generator for Fibonacci numbers and fraction
        genFib();
        genFra();
        //Activate drawing class
        getMesh = true;
    }
    //Switch back and forth between 2D/3D visualization
    if (e.target->is("TOGGLE Z-VALUE")) tglD = !tglD;
    //Get snapshot image with coordinates as file name
    if (e.target->is("TAKE SNAPSHOT")){
        snapSPath = ofToString(fn - numY / 2) + "_" + ofToString(fraNr - numX / 2) + ".png";
        //Save all on the screen
        ofSaveScreen(snapSPath);
    }
    //Switch between orthographic and perspective view
    if (e.target->is("ORTHO/PERSP")) bUseEasyCam = !bUseEasyCam;

     cout << "onButtonEvent: " << e.target->getLabel() << " " << e.target->getEnabled() << endl;
}

//--------------------------------------------------------------
void ofApp::onTextInputEvent(ofxDatGuiTextInputEvent e)
{
    // text input events carry the text of the input field //
    cout << "From Event Object: " << e.text << endl;
    // although you can also retrieve it from the event target //
    cout << "From Event Target: " << e.target->getText() << endl;
    if(e.target->is("FIBONACCI")){
        fn = (ofToInt64(e.target->getText()));
        fn = fn + numY / 2;
        cout<<fn<<endl;
    }
    if(e.target->is("FRACTION")){
        fraNr = (ofToInt64(e.target->getText()));
        fraNr = fraNr + numX / 2;
        cout<<fraNr<<endl;
    }
}

//--------------------------------------------------------------
void ofApp::toggleFullscreen(){
    //Toggle between windowed and fullscreen
    mFullscreen = !mFullscreen;
    refreshWindow();
}

//--------------------------------------------------------------
void ofApp::toggleHelp(){
    //Switch on/off help guide
    tglHelp = !tglHelp;
}

//--------------------------------------------------------------
void ofApp::refreshWindow(){
    ofSetFullscreen(mFullscreen);
    if (!mFullscreen) {
        ofSetWindowShape(1280, 800);
        ofSetWindowPosition(0, 0);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key) {
        case 'C':
        case 'c':
            if(cam.getMouseInputEnabled()) cam.disableMouseInput();
            else cam.enableMouseInput();
            break;
        case 'F':
        case 'f':
            ofToggleFullscreen();
            break;
        case 'G':
        case 'g':
            genFib();
            genFra();
            break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    //If time between clicks small enough the reset of camera
    //is triggered
    if(ofGetElapsedTimeMillis()-prevClickTime<200){
        if (bUseEasyCam == true){
            cam.reset();
            cam.setPosition(ofVec3f(0, 0, 800));
            cam.lookAt(ofVec3f(0, 0, 0));
        }
    }
    prevClickTime = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}


