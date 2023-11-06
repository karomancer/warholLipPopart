#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetWindowShape(ofGetScreenWidth(), ofGetScreenHeight());
    
    ofSetVerticalSync(true);
    ofSetFrameRate(120);
    cam.setup(480, 360);
    
    facialFeatures.setup();
    
    ofBackground(255);
    
    gui.setup("andywarhol", "settings.json");
    gui.add(showGrabber.set("Show webcam footage", false));
    gui.add(redThreshold.set("Red threshold", 120, 0, 255));
    gui.add(greenThreshold.set("Green threshold", 120, 0, 255));
}

void ofApp::update() {
    cam.update();
    if(cam.isFrameNew()) {
        ofPixels& camPix = cam.getPixels();
        
        facialFeatures.update(cam);
        
        ofRectangle smile = facialFeatures.getSmileRectangle();
        
        mouthPix.allocate(smile.getWidth(), smile.getHeight(), OF_IMAGE_COLOR);
        
        for (int y = smile.y; y < smile.y + smile.getHeight(); y++) {
            int _x = 0;
            for (int x = smile.x; x < smile.x + smile.getWidth(); x++) {
                ofColor color = camPix.getColor(x,y);
                
                if (color.r < redThreshold && color.g < greenThreshold) {
                    color.setSaturation(255);
                } else {
                    color = ofColor::white;
                }
                
                mouthPix.setColor(x - smile.x, y - smile.y, color);
            }
        }
    }
}

void ofApp::draw() {
    int rectWidth = ofGetWidth() / 2;
    int rectHeight = ofGetHeight() / 2;
    
    /** DRAW MOUTH SQUARES **/
    // Top Left (yellow)
    ofSetColor(ofColor::yellow);
    ofFill();
    ofDrawRectangle(0, 0, rectWidth, rectHeight);
    // Top Right (magenta)
    ofSetColor(ofColor(230, 0, 134));
    ofFill();
    ofDrawRectangle(rectWidth, 0, rectWidth, rectHeight);
    // Bottom Left (purps)
    ofSetColor(ofColor(138, 51, 137));
    ofFill();
    ofDrawRectangle(0, rectHeight, rectWidth, rectHeight);
    // Bottom right (lime)
    ofSetColor(ofColor(159, 192, 18));
    ofFill();
    ofDrawRectangle(rectWidth, rectHeight, rectWidth, rectHeight);
    
    float mouthPixelX = rectWidth / mouthPix.getWidth() + 0.8f;
    float mouthPixelY = rectHeight / mouthPix.getHeight() + 0.8f;
    
    for (int y = 0; y < mouthPix.getHeight(); y++) {
        for (int x = 0; x < mouthPix.getWidth(); x++) {
            if (x * mouthPixelX > rectWidth || y * mouthPixelY > rectHeight) {
                break;
            }
            
            ofColor color = mouthPix.getColor(x,y);
            if (color != ofColor::white) {
                float radius = ofMap(color.r, 0, 255, min(mouthPixelX, mouthPixelY), 0.f);
                
                // top left, red on yellow
                ofSetColor(ofColor::red);
                ofFill();
                ofDrawCircle(x * mouthPixelX, y * mouthPixelY, radius);
                
                // top right, green on magenta
                ofSetColor(ofColor(0, 135, 121));
                ofFill();
                ofDrawCircle(rectWidth + x * mouthPixelX, y * mouthPixelY, radius);
                
                // bottom left, green on magenta
                ofSetColor(ofColor(233, 175, 0));
                ofFill();
                ofDrawCircle(x * mouthPixelX, rectHeight + y * mouthPixelY, radius);
                
                // bottom right, blue on lime
                ofSetColor(ofColor(15, 18, 207));
                ofFill();
                ofDrawCircle(rectWidth + x * mouthPixelX, rectHeight + y * mouthPixelY, radius);
            }
        }
    }
    if (showGrabber) {
        ofSetColor(ofColor(255));
        
        float camX = rectWidth - cam.getWidth() /2;
        float camY = rectHeight - cam.getHeight() /2;
        cam.draw(camX, camY);
        
        ofNoFill();
        ofRectangle smile = facialFeatures.getSmileRectangle();
        ofDrawRectangle(camX + smile.x, camY + smile.y, smile.getWidth(), smile.getHeight());
    }
    gui.draw();
}
