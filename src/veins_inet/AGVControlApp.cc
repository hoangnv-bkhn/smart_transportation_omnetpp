//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "AGVControlApp.h"
//#include "HospitalControlApp.h"
#include "Constant.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"


using namespace veins;

Register_Class(AGVControlApp);

void AGVControlApp::initialize(int stage)
{
    TraCIDemo11p::initialize(stage);
    sentFirstMessage = false;
    if (stage == 0) {
        int idDebug = getId();
        sendBeacon= new cMessage("send Beacon");
        {
            mobility = TraCIMobilityAccess().get(getParentModule());
            traciVehicle = mobility->getVehicleCommandInterface();
        }
    }
    else if (stage == 1) {

        if (sendBeacon->isScheduled())
        {
            cancelEvent(sendBeacon);
        }
        {
            scheduleAt(simTime() + 0.1, sendBeacon);
            this->travellingTime = simTime().dbl();
            //EV<<"Initialize AGV at "<<simTime().dbl()<<" ";
            curPosition = mobility->getPositionAt(simTime());
        }
        if(Constant::activation == NULL){
            Constant::activation = mobility;
        }
    }
}

void AGVControlApp::finish()
{
    this->travellingTime = //traciVehicle->getWaitingTime();
    //                    traciVehicle->getAccumulatedWaitingTime();
            simTime().dbl() - this->travellingTime;
    //EV<<"This AGV spends "<<this->travellingTime<<" for travelling"<<endl;
    Constant::TOTAL_TRAVELLING_TIME += this->travellingTime;
    Constant::TOTAL_WAITING_TIME += this->waitingIntervals;
    TraCIDemo11p::finish();
    if(Constant::activation == NULL){
            EV<<"Constant is helpless eventually"<<endl;
    }
    // statistics recording goes here
}

void AGVControlApp::onBSM(DemoSafetyMessage* bsm)
{
    //for my own simulation circle
}

void AGVControlApp::onWSM(BaseFrame1609_4* wsm)
{
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void AGVControlApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void AGVControlApp::handleSelfMsg(cMessage* msg)
{
    TraCIDemo11p::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
    {
        TraCIDemo11pMessage* carBeacon = new TraCIDemo11pMessage("test", 0);
        {
           std::string content = //std::to_string(simTime().dbl()) + " ";
           //curPosition = mobility->getPositionAt(simTime());
           //content = content +
           //             std::to_string(curPosition.x) + " "
           //             + std::to_string(curPosition.y);
           /*content = content +*/ /*"Lid"*/ /*" " +*/
                   traciVehicle->getLaneId();
           double speed = traciVehicle->getSpeed();
           if(speed == 0.0){
               this->waitingIntervals++;
           }
           content = content + /*"L.P"*/ " " + std::to_string(traciVehicle->getLanePosition());
           content = content + /*"velo:"*/ " " + std::to_string(speed)
                           //+ /*"/"*/ " " + std::to_string(traciVehicle->getAcceleration())
                                   ;
           //content = content + /*"dis:"*/ " " + std::to_string(traciVehicle->getDistanceTravelled());
           //content = content + "aW: " + std::to_string(traciVehicle->getAccumulatedWaitingTime());
           carBeacon->setDemoData(content.c_str());
           carBeacon->setSenderAddress(myId);
           BaseFrame1609_4* WSM = new BaseFrame1609_4();
           WSM->encapsulate(carBeacon);
           populateWSM(WSM);
           send(WSM,lowerLayerOut);
           return;
        }
    }
}

void AGVControlApp::handlePositionUpdate(cObject* obj)
{
    TraCIDemo11p::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class

}

void AGVControlApp::handleLowerMsg(cMessage* msg)
{
    BaseFrame1609_4* WSM = check_and_cast<BaseFrame1609_4*>(msg);
    cPacket* enc = WSM->getEncapsulatedPacket();
    if(TraCIDemo11pMessage* bc = dynamic_cast<TraCIDemo11pMessage*>(enc)){
        char *ret = mergeContent(myId);

        //if(/*strcmp(ret, bc->getDemoData()) == 0 &&*/ myId == 16)
        {
            if(!receivedReRouting){
                int length = strlen(bc->getDemoData());
                std::string str = std::string(bc->getDemoData(), length);

                std::vector<std::string> v = split(str, " ");
                std::list<std::string> l(v.begin(), v.end());
                //bool change = traciVehicle->changeVehicleRoute(l);
                //traciVehicle->moveToXY("E1", 1, 4.3, 10.90, 1.57);
                if(myId == 16){
                    //EV<<str<<". Could change route? "<<change<<endl;
                }
                receivedReRouting = true;
            }
        }
    }
    else{

    }
}
