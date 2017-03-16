#ifndef DETECTORSTATE_H
#define DETECTORSTATE_H

#include <QObject>
#include <map>
#include "Ctracker.h"
#include "recorder.h"



class DetectorState : public QObject
{
    Q_OBJECT
public:
    explicit DetectorState(QObject *parent = 0, Recorder *rec = 0);
    void finishRecording();
    void resetState();

    CTracker tracker;

    int posCounter;
    int negAndNoMotionCounter;
    std::atomic<unsigned int> numberOfPlanes;
    int MIN_POS_REQUIRED;
    bool wasPlane;

    enum DetectionResult {
       UNKNOWN,
       AIRPLANE,
       BIRD,
       ALL_NEGATIVE,
       MIN_POSITIVE_NOT_REACHED

    };


    struct Result {
        QString message;
        bool willSaveVideo;

    };



private:


    Recorder *recorder;
    std::map<DetectionResult, Result> map_result;

signals:
    void sendOutputText(QString output_text);
    void foundDetectionResult(DetectorState::DetectionResult);

public slots:

private slots:
    void handleResult(DetectorState::DetectionResult result);
};
Q_DECLARE_METATYPE(DetectorState::DetectionResult)
#endif // DETECTORSTATE_H
