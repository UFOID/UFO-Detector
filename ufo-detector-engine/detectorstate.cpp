#include "detectorstate.h"

DetectorState::DetectorState(QObject *parent, Recorder *rec) : QObject(parent), tracker(0.2,0.5,60.0,15,15)
{
    recorder = rec;
    resetState();

    map_result[UNKNOWN] = {"At least one object found positive: saved video",true};
    map_result[AIRPLANE] = {"Object was airplane: removed video",true};
    map_result[BIRD] = {"Object was bird: removed video",false};
    map_result[MIN_POSITIVE_NOT_REACHED] = {"Minimum required positive detections not met: removed video",false};
    map_result[ALL_NEGATIVE] = {"All found objects negative: removed video",false};

}

void DetectorState::finishRecording()
{
    if(tracker.wasBird)
    {
        auto output_text = tr("One object identified as a bird");

    }
    if(!tracker.removedTrackWithPositive)
    { //+++all detected objects had more negative than positive detections or was a bird
        handleResult(ALL_NEGATIVE);
    }
    else
    {//+++one object had more positive than negative detections
        if(posCounter>=MIN_POS_REQUIRED)
        {
            if (wasPlane)
            {
                handleResult(AIRPLANE);
            }
            else
            {
                handleResult(UNKNOWN);
            }

        }
        else
        {
            handleResult(MIN_POSITIVE_NOT_REACHED);
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

void DetectorState::handleResult(DetectionResult result)
{
    recorder->stopRecording(map_result[result].willSaveVideo);
    emit sendOutputText("Finished recording - " + map_result[result].message);

}
