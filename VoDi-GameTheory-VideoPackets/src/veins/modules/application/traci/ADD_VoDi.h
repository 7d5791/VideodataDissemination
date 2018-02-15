//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef nsf_H
#define nsf_H


#include "BaseWaveApplLayer.h"
#include "modules/mobility/traci/TraCIMobility.h"



#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"
#include "veins/modules/mac/ieee80211p/Mac80211pToPhy11pInterface.h"
#include "veins/modules/mac/ieee80211p/WaveAppToMac1609_4Interface.h"
#include "veins/modules/phy/Decider80211p.h"
#include "veins/modules/phy/DeciderResult80211.h"
#include "veins/modules/phy/Decider80211pToPhy80211pInterface.h"
#include "veins/modules/phy/PhyLayer80211p.h"
#include "veins/modules/phy/NistErrorRate.h"
#include "veins/base/phyLayer/ChannelState.h"
#include "veins/modules/phy/SNRThresholdDecider.h"
#include "veins/base/utils/MacToNetwControlInfo.h"
#include "veins/base/phyLayer/BasePhyLayer.h"
#include "veins/base/phyLayer/PhyToMacControlInfo.h"
#include "veins/modules/phy/DeciderResult80211.h"
#include "veins/modules/utility/ConstsPhy.h"
#include "veins/modules/application/traci/beacon.h"
#include "veins/modules/application/traci/Message.h"
#include "veins/modules/application/traci/ahmad.h"


#include "veins/modules/messages/VideoStreamWithTrace.h"
#include "veins/modules/messages/VideoStreamMessage_m.h"




#include <vector>
#include <map>
#include <algorithm>

using Veins::TraCIMobility;
using Veins::AnnotationManager;
using std::vector;
using std::map;

#include <iostream>
#include <vector>
#include <iterator>
using namespace std;

typedef std::vector<WaveShortMessage*> WaveShortMessages;
typedef std::pair<std::string, double> MyPairType;

// action mode macros
#define SERVER_MODE     1
#define CLIENT_MODE     2

using namespace std;

typedef std::vector<WaveShortMessage*> WaveShortMessages;
typedef std::vector<VideoStreamMessage*> VideoStreamMessages;



enum MessageKind {
    FRAME_START = 100, PACKET_TX = 200
};


class ADD_VoDi : public BaseWaveApplLayer
{

public:
    struct VideoStreamData {
           // packet generation
           uint16_t currentSequenceNumber; ///< current (16-bit RTP) sequence number

           // variable for a video trace
           TraceFormat traceFormat;    ///< file format of trace file
           long numFrames;         ///< total number of frames for a video trace
           long numFramesSent; ///< counter for the number of frames sent; reset to zero once the whole frames in the trace file have been sent
           double framePeriod;     ///< frame period for a video trace
           long currentFrame; ///< frame index (starting from 0) to read from the trace (will be wrapped around)
           long frameNumber; ///< frame number (display order) of the current frame
           double frameTime; ///< cumulative display time of the current frame in millisecond
           FrameType frameType;    ///< type of the current frame
           long frameSize;         ///< size of the current frame in byte
           long bytesLeft;         ///< bytes left to transmit in the current frame
           double pktInterval; ///< interval between consecutive packet transmissions in a given frame
           int destinationId;

           // statistics
           long numPktSent;           ///< number of packets sent

           // self messages for timers
           cMessage *frameStartMsg;  ///< start of each frame
           cMessage *packetTxMsg;    ///< start of each packet transmission
       };

    struct CompareSecond
    {
        bool operator()(const MyPairType& left, const MyPairType& right) const
        {
        return left.second < right.second;
        }
    };

    typedef std::vector<VideoStreamData *> VideoStreamVector;
    virtual int numInitStages() const {
            return 4;
        }
    virtual void initialize(int stage);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
    ADD_VoDi();
    virtual ~ADD_VoDi();

protected:
   // TraCIMobility* traci;
    TraCIMobility* mobility;
   TraCICommandInterface* traci;
    TraCICommandInterface::Vehicle* traciVehicle;
    AnnotationManager* annotations;

    vector<WaveShortMessage*> warningMessages;

    uint32_t receivedBeacons;
    uint32_t receivedData;
    uint32_t numberOfNodes;

    double distanceFromSource;
    double distanceThreshold;

    simsignal_t arrivalSignal;
    simsignal_t arrivalSignal1;
    simsignal_t warningMsgCounterSignalRx;
    simsignal_t beaconMsgCounterSignalRx;
      //simsignal_t warningMsgCounterSignalTx;
      //simsignal_t beaconMsgCounterSignalTx;
    simsignal_t distanceMsgSignalRx;
    simsignal_t numberOfNodesSignal;

    simtime_t timeout;  // timeout
    cMessage *timeoutEvent;  // holds pointer to the timeout self-message1

    simtime_t timeout2;  // timeout
    cMessage *timeoutEvent2;  // holds pointer to the timeout self-message2

    double randomWaitingTime;

    simtime_t lastDroveAt;
    bool sentMessage;
    double neighborLifetimeThreshold;
    long indexOfAccidentNode;

    WaveShortMessages neighbors;

    std::map<long,WaveShortMessages>    receivedWarningMessageMap;    // treeId, WSM vector
    long idMsg;

    std::vector<int> indicesWarnigMsgsApp;


    /*Data Dissemination*/

    BeaconList ListBeacon;
    int contador;

    std::list<int> listNst;
    std::list<int> listN0t;

    std::list<simtime_t> listChannelOccupancy;
    std::list<simtime_t> listTimeWindow;

    std::list<std::string> junctionIds;
    std::map<std::string,Coord> junctionMap;
    std::map<std::string,double> junctionDistance;

    //Vodi game
    map<simtime_t, double> vectorKiUi;
    map<int, double> vectorKjUj;
    map<simtime_t, double> vectorMKjUjN;
    map<int, int> counterMessages;

    //forwarding game
    map<long, long double> vectorBeta;
    map<long, long double> vectorUtility;

    //beaconing rate
    map<simtime_t, double> beaconingRate;

protected:
        /*
         * These are variables for the client mode
         */
        // module parameters
        double startupDelay;
        long numTraceFrames; ///< number of frames in the trace file (needed to handle wrap around of frames by the server)
        int gopSize;    ///< GOP pattern: I-to-I frame distance (N)
        int numBFrames; ///< GOP pattern: I-to-P frame distance (M)

        // statistics
        bool warmupFinished;    ///< if true, start statistics gathering
        long numPacketsReceived;    ///< number of packets received
        long numPacketsLost;    ///< number of packets lost
        long numFramesReceived; ///< number of frames received and correctly decoded
        long numFramesDiscarded; ///< number of frames discarded due to packet loss or failed dependency

        long numSentPacket;
        long numSentFrame;
        long ReceivedPacketCount;

        // variables for packet and frame loss handling
        uint16_t prevSequenceNumber; ///< (16-bit RTP) sequence number of the most recently received packet
        long prevIFrameNumber; ///< frame number of the most recently received I frame
        long prevPFrameNumber; ///< frame number of the most recently received P frame
        long currentFrameNumber; ///< frame number (display order) of the frame under processing
        long currentEncodingNumber; ///< encoding number (transmission order) of the frame under processing
        FrameType currentFrameType; /// type (I, IDR, P, or B) of the frame under processing
        bool currentFrameDiscard; ///< if true, all the remaining packets of the current frame are to be discarded
        bool currentFrameFinished; ///< if true, the frame has been successfully received and decoded

        VideoStreamVector streamVector;

        int m_count;
        int minCount;
        double minDistance;
        double maxTime;
        string traceFile;

        int m_id;
        string mode;
        double m_dist;
        char network[127];

        unsigned int numStreams;
        list<int> m_pktNameSet;
        bool startFlag;
        simtime_t curTime;

        // variables for a trace file
        TraceFormat traceFormat;    ///< file format of trace file
        long numFrames; ///< total number of frames in a trace file
        double framePeriod;

        int appOverhead;
        int maxPayloadSize;
        bool frameSpreading;
        int actionMode;

        //Last Frame received
        int lastFrameReceived;

        Coord sourcePos;
        /*
         * These are vector structures for the frame information
         */
        LongVector frameNumberVector; ///< vector of frame numbers (display order) (only for verbose trace)
        DoubleVector frameTimeVector; ///< vector of cumulative frame display times (only for verbose trace)
        FrameTypeVector frameTypeVector; ///< vector of frame types (I, P, B, and IDR (H.264); only for verbose trace)
        LongVector frameSizeVector;


        /* Beacon parameters*/
        int beaconLengthBits;
        int beaconPriority;
        int beaconInterval;
        bool sendBeacons;
        simtime_t individualOffset;
        cMessage* sendBeaconEvt;
        double neighborValidityInterval;

        /*Video Server*/
        int idVideoRequest;
        int idVehicleRequest;


        ///*Neighbors Table*/
        map<int, double> maxDistanceRsu;
        int counterBeaconsReceived;




        VideoStreamMessages dataVideo;



protected:
    virtual void onBeacon(WaveShortMessage *wsm);
    virtual void onData(WaveShortMessage *wsm);
    virtual void handlePositionUpdate(cObject *obj);
    virtual void sendMessage(std::string blockedRoadId);

    virtual WaveShortMessage* prepareWSM(std::string name, int dataLengthBits, t_channel channel, int priority, int rcvId, int serial=0,
                    int carId=0, double speed=0, double angleRad=0, double vecX=0, double vecY=0, double vecZ=0, simtime_t idleTime=0);

    virtual void handleSelfMsg(cMessage *msg);

    /* methods for data disemination*/
    void forwardMessage(double p);
   // void calendarizador(cMessage *timeoutEvent, simtime_t timeout,bool isNewWarning,double probabilidadEnvio);
    double kiUi(double distanceFactor, double linkQualityFactor);
    bool hostIsClosestToJunction(std::string junctionId);
    double  getMin(std::map<std::string, double> mymap);
    double calcAbe(WaveShortMessage* wsm, double cpb);
    double calcDistJoin(WaveShortMessage* wsm);
    double getChannelQuality();
    double getCollisionProbability();
    double getSignalQuality(WaveShortMessage* wsm);
    double getLinkQuality(double chqb, double cpb, double sqb);


    /*methods for beaconing*/

    double beaconingMain(WaveShortMessage* wsm, int neighbors, double distanceFromPreviousNode, double distanceToIntersection );

    double beaconingChannel(double neighbors, double wc, WaveShortMessage* wsm);
    double beaconingPriority(double dEv, double dInt, WaveShortMessage* wsm);


    /*methods for game forwarding*/

    double distanceFactorCalc(double dsr, double dri, double rmax, bool inIntersection);
    bool vehicleOnJunction();
    std::string getIdJunction();


    //voulunteer's dilemma game with relatness
    double calBeq(double k, double u, double r, double n);
    double calAgeFactor(double timeElapsed);
    double calUtility(double distanceFactor, double linkQualityFactor);



    //voulunteer's dilemma game
    long double vodM(vector<long double> u);
   // double calBeqSimetrico(double k, double u, double r, double n);
   // double calBeqAsimetrico(double k, double u, double r, double n);
    double calBeqAsimetricoMult(double ki, double ui, double kjuj, double n);




    //Utility function of the forwarding game
    double forwardingGame(int N, int M, vector<long double> beta);
    double betaCalculation(int k, int n, double alfa);


    //Solving linear equations with Gaussian elimination

    // function to reduce matrix to r.e.f.  Returns a value to
     // indicate whether matrix is singular or not
     virtual int forwardElim(double** mat, int N, int M);

     // function to calculate the values of the unknowns
     virtual vector<long double> backSub(double** mat, int N, int M);


     // function to get matrix content
     virtual vector<long double> gaussianElimination(double** mat, int N, int M);

     // function for elemntary operation of swapping two rows
     virtual void swap_row(double** mat, int N, int M, int i, int j);

     // function to print matrix content at any stage
     virtual void print(double** mat, int N, int M);



  //   void LogToFile(VideoStreamMessage* msg, bool flag);
   //  void recvStream(VideoStreamMessage *pkt);
     long frameEncodingNumber(long frameNumber, int numBFrames,
             FrameType frameType);
     struct ReceivedMessage {
         long number;
         simtime_t time;
         long size;
         int type;
         bool endFlag;

         ReceivedMessage(long m_number, simtime_t m_time, int m_type,
                 long m_size, bool m_endFlag) {
             number = m_number;
             time = m_time;
             size = m_size;
             type = m_type;
             endFlag = m_endFlag;
         }
     };

     list<ReceivedMessage> m_recvMessageList;


     int frameCount;
     int packetCount;

     double accidentDuration;
     double accidentStart;
     int frameLost;

     int m_logIndex;
     int m_frameIndex;
     int m_lastFrameCount;
     int prevFrameNumber;

};

#endif // nsf_H
