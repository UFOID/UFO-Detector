/**
 * UFO Detector | www.UFOID.net
 *
 * Copyright (C) 2016 UFOID
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Ctracker.h"
using namespace cv;
using namespace std;

size_t CTrack::NextTrackID=0;
// ---------------------------------------------------------------------------
// Track constructor.
// The track begins from initial point (pt)
// ---------------------------------------------------------------------------
CTrack::CTrack(Point2f pt, float dt, float Accel_noise_mag)
{
    track_id=NextTrackID;
    if(track_id==100){
        NextTrackID=0;
    }
    else NextTrackID++;
    // Every track have its own Kalman filter,
    // it user for next point position prediction.
    KF = new TKalmanFilter(pt,dt,Accel_noise_mag);
    // Here stored points coordinates, used for next position prediction.
    prediction=pt;
    skipped_frames=0;
    negCounter=0;
    posCounter=0;
    birdCounter=0;
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CTrack::~CTrack()
{
    // Free resources.
    delete KF;
}

void CTrack::getCount(int index)
{
    cout << index << " pos: " << posCounter <<" neg: " << negCounter << endl;
}

// ---------------------------------------------------------------------------
// Tracker. Manage tracks. Create, remove, update.
// ---------------------------------------------------------------------------
CTracker::CTracker(float _dt, float _Accel_noise_mag, double _dist_thres, int _maximum_allowed_skipped_frames,int _max_trace_length)
{
    dt=_dt;
    Accel_noise_mag=_Accel_noise_mag;
    dist_thres=_dist_thres;
    maximum_allowed_skipped_frames=_maximum_allowed_skipped_frames;
    max_trace_length=_max_trace_length;
    removedTrackWithPositive=false;
    wasBird=false;
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CTracker::Update(vector<Point2d>& detections)
{
    // -----------------------------------
    // If there is no tracks yet, then every point begins its own track.
    // -----------------------------------
    if(tracks.size()==0)
    {
        // If no tracks yet
        for(int i=0; i < (int)detections.size(); i++)
        {
            CTrack* tr=new CTrack(detections[i],dt,Accel_noise_mag);
            tracks.push_back(tr);
            //wasBird=false;
        }
    }

    // -----------------------------------
    // Tracks already exist in any event
    // -----------------------------------
    int N=tracks.size();
    int M=detections.size();

    // ������� ���������� �� N-���� ����� �� M-���� �������.
    vector< vector<double> > Cost(N,vector<double>(M));
    vector<int> assignment; // ����������

    // -----------------------------------
    // ����� ��� ����, �������� ������� ����������
    // -----------------------------------
    double dist;
    for(int i=0; i < (int)tracks.size(); i++)
    {
        // Point2d prediction=tracks[i]->prediction;
        // cout << prediction << endl;
        for(int j=0; j < (int)detections.size(); j++)
        {
            Point2d diff=(tracks[i]->prediction-detections[j]);
            dist=sqrtf(diff.x*diff.x+diff.y*diff.y);
            Cost[i][j]=dist;
        }
    }
    // -----------------------------------
    // Solving assignment problem (tracks and predictions of Kalman filter)
    // -----------------------------------
    AssignmentProblemSolver APS;
    APS.Solve(Cost,assignment,AssignmentProblemSolver::optimal);

    // -----------------------------------
    // clean assignment from pairs with large distance
    // -----------------------------------
    // Not assigned tracks
    vector<int> not_assigned_tracks;

    for(int i=0; i < (int)assignment.size();i++)
    {
        if(assignment[i]!=-1)
        {
            if(Cost[i][assignment[i]]>dist_thres)
            {
                assignment[i]=-1;
                // Mark unassigned tracks, and increment skipped frames counter,
                // when skipped frames counter will be larger than threshold, track will be deleted.
                not_assigned_tracks.push_back(i);
            }
        }
        else
        {
            // If track have no assigned detect, then increment skipped frames counter.
            tracks[i]->skipped_frames++;
        }

    }

    // -----------------------------------
    // If track didn't get detects long time, remove it.
    // If one of the removed track has more positive than negative detection set bool to true
    // If one of the removed track was counted as a bird twice or more set bool to true
    // -----------------------------------
    for(int i=0; i < (int)tracks.size(); i++)
    {
        if((int)tracks[i]->skipped_frames > maximum_allowed_skipped_frames)
        {
            if(tracks[i]->posCounter>tracks[i]->negCounter){
                if(tracks[i]->birdCounter<2){
                    removedTrackWithPositive=true;
                }
                else wasBird=true;
            }
            //cout << "Pos: " << tracks[i]->posCounter << " Neg: " << tracks[i]->negCounter << endl;
            delete tracks[i];
            tracks.erase(tracks.begin()+i);
            assignment.erase(assignment.begin()+i);
            i--;

        }
    }
    // -----------------------------------
    // Search for unassigned detects
    // -----------------------------------
    vector<int> not_assigned_detections;
    vector<int>::iterator it;
    for(int i=0; i < (int)detections.size(); i++)
    {
        it=find(assignment.begin(), assignment.end(), i);
        if(it==assignment.end())
        {
            not_assigned_detections.push_back(i);
        }
    }

    // -----------------------------------
    // and start new tracks for them.
    // -----------------------------------
    if(not_assigned_detections.size()!=0)
    {
        for(int i=0; i < (int)not_assigned_detections.size(); i++)
        {
            CTrack* tr=new CTrack(detections[not_assigned_detections[i]],dt,Accel_noise_mag);
            tracks.push_back(tr);
        }
    }

    // Update Kalman Filters state

    for(int i=0; i < (int)assignment.size(); i++)
    {
        // If track updated less than one time, than filter state is not correct.

        tracks[i]->KF->GetPrediction();

        if(assignment[i]!=-1) // If we have assigned detect, then update using its coordinates,
        {
            tracks[i]->skipped_frames=0;
            tracks[i]->prediction=tracks[i]->KF->Update(detections[assignment[i]],1);
        }else				  // if not continue using predictions
        {
            tracks[i]->prediction=tracks[i]->KF->Update(Point2f(0,0),0);
        }

        if((int)tracks[i]->trace.size() > max_trace_length)
        {
            tracks[i]->trace.erase(tracks[i]->trace.begin(),tracks[i]->trace.end()-max_trace_length);
        }

        tracks[i]->trace.push_back(tracks[i]->prediction);
        tracks[i]->KF->LastResult=tracks[i]->prediction;
    }

}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CTracker::~CTracker(void)
{
    for(int i=0; i < (int)tracks.size(); i++)
    {
    delete tracks[i];
    }
    tracks.clear();
}

// -----------------------------------
// Update tracking of there was no movement detected
// -----------------------------------
void CTracker::updateEmpty(){
    for(int i=0; i < (int)tracks.size(); i++)
    {
        tracks[i]->skipped_frames++;
        if((int)tracks[i]->skipped_frames > maximum_allowed_skipped_frames)
        {

            if(tracks[i]->posCounter>tracks[i]->negCounter){
                if(tracks[i]->birdCounter<2){
                    removedTrackWithPositive=true;
                }
                else wasBird=true;
            }
            else if (tracks[i]->birdCounter>1){
                wasBird=true;
            }
            //cout << "Pos: " << tracks[i]->posCounter << " Neg: " << tracks[i]->negCounter << endl;
            delete tracks[i];
            tracks.erase(tracks.begin()+i);
            //assignment.erase(assignment.begin()+i);
            i--;

        }
    }


}
