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

#include "HospitalControlApp.h"
#include "Constant.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include <fstream>
#include <string.h>

using namespace veins;
using namespace std;

//HospitalControlApp::HospitalControlApp() {
//    // TODO Auto-generated constructor stub
//
//}
//
//HospitalControlApp::~HospitalControlApp() {
//    // TODO Auto-generated destructor stub
//}

Register_Class(HospitalControlApp);

std::vector<Crossing> crossings;

void HospitalControlApp::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {

        std::string line;
        std::ifstream MyReadFile("crossing.txt");
        getline(MyReadFile, line);
        int numberOfCrossing =std::stoi(line);

//        Crossing crossings[numberOfCrossing];
//        crossings[numberOfCrossing];
        int k = 0;

        while (getline(MyReadFile, line)) {
            size_t pos;
            std::string token;
            Crossing tmp;

            for (int i = 0; i < 4; i++) {
                pos = line.find(" ");
                token = line.substr(0, pos);
                EV << token <<endl;

                if (i == 0) tmp.id = token;
                if (i == 1) tmp.length = std::atof(token.c_str());
//                if (i == 1) crossings[k].length = *token;
                if (i == 2) tmp.from = token;
                if (i == 3) tmp.to = token;
                line.erase(0, pos + 1);

            }
            tmp.rec = CustomRectangle(line);
            crossings.push_back(tmp);
//            EV << line <<endl;
            k++;
        }

        MyReadFile.close();

//        EV << crossings.at(1).id;
//        EV << crossings[1].rec.A.x;

        sendBeacon= new cMessage("send Beacon");
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }


}

void HospitalControlApp::finish()
{
    //Duoc goi khi RSU ket thuc cong viec
    DemoBaseApplLayer::finish();
    // statistics recording goes here
}

void HospitalControlApp::onBSM(DemoSafetyMessage* bsm)
{
    //for my own simulation circle
}

void HospitalControlApp::onWSM(BaseFrame1609_4* wsm)
{
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
    cPacket* enc = wsm->getEncapsulatedPacket();

    if(TraCIDemo11pMessage* bc = dynamic_cast<TraCIDemo11pMessage*>(enc)){
        if(strcmp(Constant::FIRST, bc->getDemoData()) == 0){
            EV << "my message = " <<bc->getDemoData()<<" from "<<bc->getSenderAddress()<<endl;
            if(sendBeacon != NULL){
                if (sendBeacon->isScheduled())
                {
                    cancelEvent(sendBeacon);
                }

                if(traci == NULL){
                       if(Constant::activation != NULL)
                       traci = Constant::activation->getCommandInterface();
                }

                std::list<std::string> allPeople = traci->getPersonIds();

//                for(std::list<std::string>::const_iterator i = allPeople.begin(); i != allPeople.end(); ++i)
//                {
//                     printf("%s\n", i->c_str());
//                }

//                int numCrossing = (sizeof(crossings)/sizeof(*crossings));

                for (auto elem: allPeople) {
                    std::string personId = elem;
                    Coord allPeoplePosition = traci->getPersonPosition(personId);
                    EV << personId + ' ' ;
                    EV << allPeoplePosition;
                    EV << ' ' +to_string(simTime().dbl()) << std::endl;

                    for(int i = 0; i < crossings.size(); i++){
                         if(crossings.at(i).rec.checkInside(allPeoplePosition)){
                             EV << '1234' <<endl;
                             crossings.at(i).peoples.push_back(std::make_tuple(personId, allPeoplePosition.x, allPeoplePosition.y, simTime().dbl()));
                         }

//                         EV << crossings.at(i).peoples.at(0) <<endl;

//                         for (auto [ X, Y, Z, P ] : crossings.at(i).peoples)
//                         {
//                               EV<< X << " " << Y << " " << Z << " " <<P <<endl;
//                         }
                    }
                }

                TraCIDemo11pMessage* rsuBeacon = new TraCIDemo11pMessage();

                char *ret = mergeContent(bc->getSenderAddress());
                rsuBeacon->setDemoData(ret);
                rsuBeacon->setSenderAddress(myId);
                BaseFrame1609_4* WSM = new BaseFrame1609_4();
                WSM->encapsulate(rsuBeacon);
                populateWSM(WSM);
                send(WSM,lowerLayerOut);
            }
        }
    }
}

void HospitalControlApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void HospitalControlApp::handleSelfMsg(cMessage* msg)
{
    DemoBaseApplLayer::handleSelfMsg(msg);
}

void HospitalControlApp::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class
}

double getAvailablePerdestrian(std::string crossId, double _time) {
//    double m = std::max({_time - Constant::DELTA_T, 0});
    return 0;

}


