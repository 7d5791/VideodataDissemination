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

class ADD_ForwarGame : public BaseWaveApplLayer
{

public:
    struct CompareSecond
    {
        bool operator()(const MyPairType& left, const MyPairType& right) const
        {
        return left.second < right.second;
        }
    };
    virtual void initialize(int stage);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
    ADD_ForwarGame();
    virtual ~ADD_ForwarGame();

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


    std::map<int,beaconFat*>    neighborsTable;

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
    map<long, long double> vectorAlfa;
    map<long, long double> vectorUtility;

    //beaconing rate
    map<simtime_t, double> beaconingRate;

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
    double kiUi(WaveShortMessage* wsm);
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
    double betaCalculation(int k, int n, double alfa);


    //voulunteer's dilemma game with relatness
    double calBeq(double k, double u, double r, double n);
    double calAgeFactor(double timeElapsed);
    double calUtility(double age, double distanceFactor, double linkQualityFactor);



    //voulunteer's dilemma game
    long double vodM(vector<long double> u);
   // double calBeqSimetrico(double k, double u, double r, double n);
   // double calBeqAsimetrico(double k, double u, double r, double n);
    double calBeqAsimetricoMult(double ki, double ui, double kjuj, double n);




    //Utility function of the forwarding game
    double forwardingGame(int N, int M, vector<long double> beta);
    double availabilityA(double d, double ab);


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

};

#endif // nsf_H
