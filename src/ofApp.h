#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

// single-person smile strength detector
// not robust against rotation
class FacialFeatureDetector {
protected:
    ofRectangle roi;
    ofImage image;
        
    ofRectangle smile = ofRectangle(0,0,0,0);
    
public:
    ofxCv::ObjectFinder faceFinder, smileFinder, leftEyeFinder, rightEyeFinder;

    void setup() {
        faceFinder.setup("haarcascade_frontalface_default.xml");
        faceFinder.setPreset(ofxCv::ObjectFinder::Accurate);
        faceFinder.setFindBiggestObject(true);
        
        smileFinder.setup("haarcascade_mcs_mouth.xml");
        smileFinder.setPreset(ofxCv::ObjectFinder::Accurate);
        smileFinder.setFindBiggestObject(true);
                
        smileFinder.setMinNeighbors(0);
    }
    template <class T>
    void update(T& img) {
        update(ofxCv::toCv(img));
    }
    void update(const cv::Mat& mat) {
        faceFinder.update(mat);
        if (faceFinder.size()) {
            roi = getBiggestFace();
            float lowerRatio = .35;
            roi.y += roi.height * (1 - lowerRatio);
            roi.height *= lowerRatio;
            cv::Mat faceMat(mat, ofxCv::toCv(roi));        
                                                    
            smileFinder.update(faceMat);
        }
    }
    
    bool intersectsSmile(int x, int y) {
        return !(x < smile.x || x > smile.x + smile.width || y < smile.y || y > smile.y + smile.height);
    }
    
    ofRectangle getBiggestFace() const {
        if (faceFinder.size() > 1) {
            int area = 0;
            ofRectangle biggestFace;
            for (int i = 0; i < faceFinder.size(); i++) {
                ofRectangle face = faceFinder.getObject(i);
                int newArea = face.getWidth() * face.getHeight();
                if (newArea > area) {
                    area = newArea;
                    biggestFace = face;
                }
            }
            return biggestFace;
        }
        
        return faceFinder.getObject(0);
    }
    
    ofRectangle getSmileRectangle() {
        int x = 0, y = 0, width = 0, height = 0, size = smileFinder.size();
        
        // average all smiles detected
        for (int i = 0; i < size; i++) {
            ofRectangle smileObject = smileFinder.getObject(i);
            x += smileObject.getX();
            y += smileObject.getY();
            width += smileObject.getWidth();
            height += smileObject.getHeight();
        }
        ofRectangle smileRect = ofRectangle(x/size + roi.getX(), y/size + roi.getY(), width/size, height/size);
                
        smile = smileRect;
        
        return smile;
    }
};

class ofApp : public ofBaseApp {
    public:
        void setup();
        void updateThresholdImage();
        void update();
        void draw();
        
        ofVideoGrabber cam;
        FacialFeatureDetector facialFeatures;
    
        
    
        ofPixels mouthPix;
        ofParameter<bool> showGrabber;
        ofParameter<float> redThreshold;
        ofParameter<float> greenThreshold;
        
        ofxPanel gui;
};
