#include "AGVControlApp.h"
#include "Constant.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "veins/modules/mobility/traci/TraCIConnection.h"

using namespace veins;

Register_Class(AGVControlApp);

void AGVControlApp::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);

    if (stage == 0) {
        if(Constant::activation == NULL){
            Constant::activation = mobility;
        }
        traci = Constant::activation->getCommandInterface();

        carBeacon = new TraCIDemo11pMessage();
        std::string content = std::to_string(simTime().dbl()) + " ";
        curPosition = mobility->getPositionAt(simTime());

        std::pair<double,double> coordTraCI = traci->getTraCIXY(curPosition);
//        EV << coordTraCI.first << coordTraCI.second << endl;

//        content = content + std::to_string(curPosition.x) + " " + std::to_string(curPosition.y);
        content = content + std::to_string(coordTraCI.first) + " " + std::to_string(coordTraCI.second);
        content = content + /*"Lid"*/ " " + traciVehicle->getLaneId();
        content = content + /*"L.P"*/ " " + std::to_string(traciVehicle->getLanePosition());
        content = content + /*"velo:"*/ " " + std::to_string(traciVehicle->getSpeed())
                        + " " + std::to_string(traciVehicle->getAcceleration());
        content = content + /*"dis:"*/ " " + std::to_string(traciVehicle->getDistanceTravelled());
        carBeacon->setDemoData(content.c_str());
        carBeacon->setSenderAddress(myId);


        int idDebug = getId();

//        sendBeacon= new cMessage("send Beacon");
        if(idDebug == 15){
//            mobility = TraCIMobilityAccess().get(getParentModule());
//            traciVehicle = mobility->getVehicleCommandInterface();

            ///subscribedServiceId = -1;
            //currentOfferedServiceId = 7;

            //wsaInterval = 5;
            //beaconInterval = 1;
        }
    }
    else if (stage == 1) {

        if (carBeacon->isScheduled())
        {
            cancelEvent(carBeacon);
        }
        scheduleAt(simTime() + 0.1, carBeacon);
        EV<<"Send to RSU. Waiting for response from RSU!";
    }
}

void AGVControlApp::finish()
{
    DemoBaseApplLayer::finish();
    //EV<<"Reach destination over here"<<endl;
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
    DemoBaseApplLayer::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
    //if(msg == sendBeacon)
    {
        TraCIDemo11pMessage* carBeacon = new TraCIDemo11pMessage();

        if(Constant::activation == NULL){
            Constant::activation = mobility;
        }
        traci = Constant::activation->getCommandInterface();

//        carBeacon = new TraCIDemo11pMessage();
        std::string content = std::to_string(simTime().dbl()) + " ";
        curPosition = mobility->getPositionAt(simTime());

        std::pair<double,double> coordTraCI = traci->getTraCIXY(curPosition);

//        content = content + std::to_string(curPosition.x) + " " + std::to_string(curPosition.y) + " ";
        content = content + std::to_string(coordTraCI.first) + " " + std::to_string(coordTraCI.second);
        content = content + /*"Lid"*/ " " + traciVehicle->getLaneId();
        content = content + /*"L.P"*/ " " + std::to_string(traciVehicle->getLanePosition());
        content = content + /*"velo:"*/ " " + std::to_string(traciVehicle->getSpeed())
                        + " " + std::to_string(traciVehicle->getAcceleration());
        content = content + /*"dis:"*/ " " + std::to_string(traciVehicle->getDistanceTravelled());
//        EV << Constant::FIRST <<endl;

        carBeacon->setDemoData(content.c_str());
        carBeacon->setSenderAddress(myId);

//        carBeacon->setDemoData(Constant::FIRST);
//        carBeacon->setSenderAddress(myId);

        BaseFrame1609_4* WSM = new BaseFrame1609_4();
        WSM->encapsulate(carBeacon);
        populateWSM(WSM);
        send(WSM,lowerLayerOut);
        return;
    }
}

void AGVControlApp::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class

}

void AGVControlApp::handleLowerMsg(cMessage* msg)
{
    BaseFrame1609_4* WSM = check_and_cast<BaseFrame1609_4*>(msg);
    cPacket* enc = WSM->getEncapsulatedPacket();
    if(TraCIDemo11pMessage* bc = dynamic_cast<TraCIDemo11pMessage*>(enc)){
        char *ret = mergeContent(myId);

        EV<<"Id: "<<myId<<". PATH "<<traciVehicle->getRoadId()<<endl;

        if(strcmp(ret, bc->getDemoData()) == 0){
            if(traciVehicle->getSpeed() <= 5){
                EV<<"My new speed: 20"<<endl;
                traciVehicle->setSpeedMode(0x1f);
                traciVehicle->setSpeed(20);
            }
            else{
                EV<<"Id: "<<myId<<". My curr speed: "<<traciVehicle->getSpeed()<<endl;
            }
        }
    }
    else{
        //EV<<"In "<<myId<<". At "<<simTime()
        //        <<" no TraCIDemo11Message but from "
        //        <<msg->getSenderModuleId()<<endl;
    }
}
