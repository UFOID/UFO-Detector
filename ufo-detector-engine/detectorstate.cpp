#include "detectorstate.h"


DetectorState::DetectorState(QObject *parent, Recorder *rec) : QObject(parent), tracker(0.2,0.5,60.0,15,15)
{
    recorder = rec;
    resetState();

    map_result[UNKNOWN] = {"At least one object found positive: saved video",true};
    map_result[AIRPLANE] = {"Object was airplane: removed video",false};
    map_result[BIRD] = {"Object was bird: removed video",false};
    map_result[MIN_POSITIVE_NOT_REACHED] = {"Minimum required positive detections not met: removed video",false};
    map_result[ALL_NEGATIVE] = {"All found objects negative: removed video",false};
    int id = qRegisterMetaType<DetectorState::DetectionResult>();
    connect(this,SIGNAL(foundDetectionResult(DetectorState::DetectionResult)),this,SLOT(handleResult(DetectorState::DetectionResult)));

}

void DetectorState::finishRecording()
{

    if(!tracker.removedTrackWithPositive)
    {
        // All detected objects had more negative than positive detections
        emit foundDetectionResult(ALL_NEGATIVE);
    }
    else
    {
        if(posCounter>=MIN_POS_REQUIRED)
        {
            if (wasPlane)
            {
                // All objects where aircraft
                emit foundDetectionResult(AIRPLANE);
            }
            else if(tracker.wasBird)
            {
                // All objects where a bird
                emit foundDetectionResult(BIRD);
            }
            else
            {
                // At least one object was unkown
                emit foundDetectionResult(UNKNOWN);
            }

        }
        else
        {
            // Minimum positive required not reached
            emit foundDetectionResult(MIN_POSITIVE_NOT_REACHED);
        }
    }

}

void DetectorState::resetState()
{
    tracker.removedTrackWithPositive=false;
    tracker.wasBird=false;
    negAndNoMotionCounter=0;
    posCounter=0;    
    wasPlane= false;
    numberOfPlanes = 0;
}

void DetectorState::handleResult(DetectorState::DetectionResult result)
{
    recorder->stopRecording(map_result[result].willSaveVideo);
    emit sendOutputText("Finished recording - " + map_result[result].message);

}
