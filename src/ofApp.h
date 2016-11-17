#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofx2DCam.h"
#include <stdio.h>                      
#include <iostream>
#include <cstdlib>
#include "boost/multiprecision/cpp_int.hpp"
#include <boost/algorithm/string.hpp>

#define numX 128
#define numY 80

#define get_buffer(a) buffer[(a)%2]
#define BIG boost::multiprecision::cpp_int

class ofApp : public ofBaseApp{
    public:
        void setup();
        void update();
        void draw();

        void genMesh();
        void genFib();
        void genFra();
        void onButtonEvent(ofxDatGuiButtonEvent e);
        void onTextInputEvent(ofxDatGuiTextInputEvent e);
        void refreshWindow();
        void toggleFullscreen();
        void toggleHelp();
        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y);
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);

    //Set 2D/3D camera and related functions
    bool bShowHelp;
    ofEasyCam cam;
    int prevClickTime;
    ofx2DCam orthoCam;
    bool bUseEasyCam;

    //Set text
    ofTrueTypeFont title;

    //All about creating 3d cubes in response to initialized vector values
    int w, h, d, numOfX, numOfY, spacingX, spacingY, startingX, startingY;
    int locX, locY;
    const char* argv[];
    unsigned int n, fn, initFra, count, initFibInt, countX;
    BIG initFib, value, fraNr, rest;
    string fibBuf;
    string fibBuf2;
    string snapSPath;

    ofColor black;
    ofColor red;
    ofColor orange;
    ofColor yellow;
    ofColor green;
    ofColor turqoise;
    ofColor blue;
    ofColor lilac;
    ofColor violet;
    ofColor white;
    ofColor color;

    vector <BIG> fibSeq;
    vector <int> fraPos;
    int meshVals[numX][numY];
    BIG buffer[2];

    //GUI
    bool mFullscreen, getMesh, tglHelp, tglD;
    ofxDatGui* gui;
    ofxDatGuiFolder* folder;
    ofTrueTypeFont font;
    int rowHeight;

    ofLight light;
};
