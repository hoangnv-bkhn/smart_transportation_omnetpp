#include "VehicleControlApp.h"
#include "Constant.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

using namespace veins;

Register_Class(VehicleControlApp);

void VehicleControlApp::initialize(int stage)
{
//    EV<<"123123123 "<<endl;
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {

        int idDebug = getId();
        sendBeacon= new cMessage("send Beacon");
        if(idDebug == 15){
            mobility = TraCIMobilityAccess().get(getParentModule());
            traciVehicle = mobility->getVehicleCommandInterface();
            ///subscribedServiceId = -1;
            //currentOfferedServiceId = 7;

            //wsaInterval = 5;
            //beaconInterval = 1;
        }
    }
    else if (stage == 1) {

        if (sendBeacon->isScheduled())
        {
            cancelEvent(sendBeacon);
        }
        scheduleAt(simTime() + 0.1, sendBeacon);
        EV<<"Send to RSU. Waiting for response from RSU!";
    }
}

void VehicleControlApp::finish()
{
    DemoBaseApplLayer::finish();
    //EV<<"Reach destination over here"<<endl;
    // statistics recording goes here
}

void VehicleControlApp::onBSM(DemoSafetyMessage* bsm)
{
    //for my own simulation circle
}

void VehicleControlApp::onWSM(BaseFrame1609_4* wsm)
{
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void VehicleControlApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void VehicleControlApp::handleSelfMsg(cMessage* msg)
{
    DemoBaseApplLayer::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
    //if(msg == sendBeacon)
    {
        TraCIDemo11pMessage* carBeacon = new TraCIDemo11pMessage();
        carBeacon->setDemoData(Constant::FIRST);
        carBeacon->setSenderAddress(myId);
        BaseFrame1609_4* WSM = new BaseFrame1609_4();
        WSM->encapsulate(carBeacon);
        populateWSM(WSM);
        send(WSM,lowerLayerOut);
        return;
    }
}

void VehicleControlApp::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class

}

void VehicleControlApp::handleLowerMsg(cMessage* msg)
{
    BaseFrame1609_4* WSM = check_and_cast<BaseFrame1609_4*>(msg);
    cPacket* enc = WSM->getEncapsulatedPacket();
    if(TraCIDemo11pMessage* bc = dynamic_cast<TraCIDemo11pMessage*>(enc)){
        char *ret = mergeContent(myId);

        EV<<"Id: "<<myId<<". PATH "<<traciVehicle->getRoadId()<<endl;
        srand(time(0));
        int randomNumber = (rand() % 2) + 1;
        EV<<"Random Number: "<<randomNumber<<endl;

//        if (randomNumber == 1) {
////            if (traciVehicle->getRoadId().compare("-gneE0") != 0 && traciVehicle->getRoadId().compare("-gneE5") != 0 && traciVehicle->getRoadId().compare("gneE4") != 0) {
////                traciVehicle->changeVehicleRoute({traciVehicle->getRoadId(), "-gneE0"});
////            }
//            if (traciVehicle->getRoadId().compare("gneE3") == 0) {
//                traciVehicle->changeVehicleRoute({traciVehicle->getRoadId(), "-gneE0"});
//            }
//
//        } else if (randomNumber == 2) {
////            if (traciVehicle->getRoadId().compare("-gneE5") != 0 && traciVehicle->getRoadId().compare("-gneE0") != 0 && traciVehicle->getRoadId().compare("gneE4") != 0) {
////                traciVehicle->changeVehicleRoute({traciVehicle->getRoadId(), "-gneE5", "gneE4"});
////            }
//            if (traciVehicle->getRoadId().compare("gneE3") == 0) {
//                traciVehicle->changeVehicleRoute({traciVehicle->getRoadId(), "-gneE5",  "gneE4"});
//            }
//        }


        if (randomNumber == 1) {
           if (traciVehicle->getRoadId().compare("gneE7") == 0) {
               traciVehicle->changeVehicleRoute({traciVehicle->getRoadId(), "gneE47", "gneE8", "gneE9", "gneE10", "gneE11", "gneE49", "gneE52", "gneE53",
                   "gneE54", "gneE100", "gneE101", "gneE140", "gneE141", "gneE142", "gneE143", "gneE144",
                   "gneE134", "gneE135", "gneE186", "gneE187", "gneE188", "gneE189", "gneE190", "gneE191"});
           }

        } else if (randomNumber == 2) {
           if (traciVehicle->getRoadId().compare("gneE7") == 0) {
               traciVehicle->changeVehicleRoute({traciVehicle->getRoadId(), "gneE22", "gneE23", "gneE24", "gneE25", "gneE26", "gneE27", "gneE40",
                                  "gneE42", "gneE43", "gneE44", "gneE87", "gneE88"});
           }
        }

//        if (traciVehicle->getRoadId().compare("gneE7") == 0) {
//            EV<<"Hello World"<<endl;
//            traciVehicle->changeVehicleRoute({traciVehicle->getRoadId(), "gneE47"});
//        }


//        if (randomNumber == 1 && routeChanged == false) {
//            traciVehicle->changeVehicleRoute({traciVehicle->getRoadId(), "gneE532", "gneE533", "gneE414", "gneE415",
//                "gneE416", "gneE417", "gneE418", "gneE419", "gneE420", "-gneE413" "-gneE412" "-gneE454" "-gneE453",
//                "-gneE452", "-gneE451", "gneE458", "gneE459", "gneE460"});
//            routeChanged = true;
//        }

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
