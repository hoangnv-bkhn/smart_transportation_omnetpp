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
#include <set>
#include <string>
#include<math.h>

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

static std::vector<Crossing> crossings;

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

//        veins::Coord newCoord;
//        newCoord.x = 101.6;
//        newCoord.y = 103.339;
//        newCoord.z = 0;
//        EV << newCoord <<endl;
//
//        for (auto elem : crossings) {
//            EV << elem.id << elem.rec.A << elem.rec.B << elem.rec.C << elem.rec.D << endl;
//            if (elem.rec.checkInside(newCoord)) {
//                EV << "inside: " << endl;
//            }
//        }

        sendBeacon= new cMessage("send Beacon");
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }


}

void HospitalControlApp::finish()
{
    //Duoc goi khi RSU ket thuc cong viec
//    EV << "QD: "<< crossings[1].peoples.size() <<endl;
    DemoBaseApplLayer::finish();
    double a = getAvailablePerdestrian(":J1_c0_0", 9);
    EV << "Xac suat: "<< a <<endl;
    double b = getVeloOfPerdestrian(":J1_c0_0", 9);
    EV << "Van toc trung binh: "<< b <<endl;


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

//    EV << "Num 0: "<< crossings[0].peoples.size() <<endl;
//    EV << "Num 1: "<< crossings[1].peoples.size() <<endl;
//    EV << "Num 2: "<< crossings[2].peoples.size() <<endl;

    if(TraCIDemo11pMessage* bc = dynamic_cast<TraCIDemo11pMessage*>(enc)){
//        if(strcmp(Constant::FIRST, bc->getDemoData()) == 0){
            EV << "My message = " <<bc->getDemoData()<<" from "<<bc->getSenderAddress()<<endl;
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

//                int numCrossing = (sizeof(crossings)/sizeof(*crossings));

                for(int i = 0; i < crossings.size(); i++){
                    for (auto elem: allPeople) {
                        std::string personId = elem;
                        Coord peoplePosition = traci->getPersonPosition(personId);
                        std::pair<double,double> coordTraCI = traci->getTraCIXY(peoplePosition);
//                        EV << personId + ' ' ;
//    //                    EV << peoplePosition;
//                        EV << std::to_string(coordTraCI.first) + ' ' + std::to_string(coordTraCI.second);
//                        EV << ' ' +to_string(simTime().dbl()) << std::endl;
                        veins::Coord newCoord;
                        newCoord.x = coordTraCI.first;
                        newCoord.y = coordTraCI.second;
                        newCoord.z = 0;
//                        EV << newCoord <<endl;
                        if (crossings[i].rec.checkInside(newCoord)) {
//                            EV << crossings[i].id << endl ;
//                            EV << personId + ' ' ;
//                            EV << std::to_string(coordTraCI.first) + ' ' + std::to_string(coordTraCI.second);
//                            EV << ' ' +to_string(simTime().dbl()) << std::endl;
//                            EV << "inside: " << endl;
                            crossings[i].peoples.push_back(std::make_tuple(personId, newCoord.x, newCoord.y, simTime().dbl()));
//                            elem1.peoples.insert(elem1.peoples.end(), std::make_tuple(personId, newCoord.x, newCoord.y, simTime().dbl()));
//                            EV << "Hien tai 4: "<< crossings[i].peoples.size() <<endl;
                        }
                    }
                }

//                for (auto elem: allPeople) {
//                    std::string personId = elem;
//                    Coord peoplePosition = traci->getPersonPosition(personId);
//                    std::pair<double,double> coordTraCI = traci->getTraCIXY(peoplePosition);
//                    EV << personId + ' ' ;
////                    EV << peoplePosition;
//                    EV << std::to_string(coordTraCI.first) + ' ' + std::to_string(coordTraCI.second);
//                    EV << ' ' +to_string(simTime().dbl()) << std::endl;
//                    veins::Coord newCoord;
//                    newCoord.x = coordTraCI.first;
//                    newCoord.y = coordTraCI.second;
//                    newCoord.z = 0;
//                    EV << newCoord <<endl;
//
//
//                    for(int i = 0; i < crossings.size(); i++){
////                        EV << crossings.at(i).rec.A <<endl;
//                         if(crossings.at(i).rec.checkInside(newCoord)){
//                             EV << "inside" <<endl;
////                             crossings.at(i).peoples.push_back(std::make_tuple(personId, peoplePosition.x, peoplePosition.y, simTime().dbl()));
//                         }
//
////                         EV << crossings.at(i).peoples.at(0) <<endl;
//
////                         for (auto [ X, Y, Z, P ] : crossings.at(i).peoples)
////                         {
////                               EV<< X << " " << Y << " " << Z << " " <<P <<endl;
////                         }
//                    }
//                }

                TraCIDemo11pMessage* rsuBeacon = new TraCIDemo11pMessage();

                char *ret = mergeContent(bc->getSenderAddress());
                rsuBeacon->setDemoData(ret);
                rsuBeacon->setSenderAddress(myId);
                BaseFrame1609_4* WSM = new BaseFrame1609_4();
                WSM->encapsulate(rsuBeacon);
                populateWSM(WSM);
                send(WSM,lowerLayerOut);
            }
//        }
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

double HospitalControlApp::getAvailablePerdestrian(std::string crossId, double _time) {
    int count = 0;
    double start = 0;

    if (_time - Constant::DELTA_T > 0) {
        start = _time - Constant::DELTA_T;
    }
    double tmp = (_time - start)/0.1;
    EV << "Start: "<<start << " End: "<< _time <<endl;
    auto it = find_if(crossings.begin(), crossings.end(), [&crossId](const Crossing& obj) {return obj.id.compare(crossId) == 0;});
    if (it != crossings.end())
    {
//        EV << (it->rec).A <<endl;
        double pivot = start;
        do {
            for(auto elem : it->peoples) {
                if (pivot <= get<3>(elem) && get<3>(elem) < pivot + 0.1) {
//                    EV << "People: " << get<0>(elem) <<endl;
//                    EV <<pivot << " " << pivot + 0.1 <<" Count: " << count <<endl;
                    count++;
                    break;
                }
            }
//            EV << "Num duration: " << count <<endl;
            pivot = pivot + 0.1;
        } while(pivot < _time && count != tmp);
    }

//    EV << "Num duration: " << count <<endl;
//    EV << "Total: " << tmp <<endl;

//    EV << count/tmp <<endl;
    return count/tmp;

}

double HospitalControlApp::getVeloOfPerdestrian(std::string crossId, double _time) {
    double start = 0;
    if (_time - Constant::DELTA_T > 0) {
        start = _time - Constant::DELTA_T;
    }
    EV << "Start: "<<start << " End: "<< _time <<endl;

    std::set < std::string > personIds;
    double sum = 0;
    int numPeople = 0;
    auto it = find_if(crossings.begin(), crossings.end(), [&crossId](const Crossing& obj) {return obj.id.compare(crossId) == 0;});
    if (it != crossings.end())
    {
        for(auto elem : it->peoples) {
            if (start <= get<3>(elem) && get<3>(elem) < _time) {
                personIds.insert(get<0>(elem));
            }
        }
        numPeople = personIds.size();
//        EV <<"Num of People: " << numPeople <<endl;
        for(auto person : personIds) {
            std::vector<std::tuple<std::string, double, double, double>> tmp;
            for(auto elem : it->peoples) {
                if(get<0>(elem).compare(person) == 0 && start <= get<3>(elem) && get<3>(elem) <= _time) {
                    tmp.push_back(elem);
                }
            }
            int n = tmp.size();
//            EV <<"tmp.size(): " << n <<endl;
            if (n > 1){
                sum += sqrt(
                    (get<1>(tmp[n-1]) - get<1>(tmp[0]))*(get<1>(tmp[n-1]) - get<1>(tmp[0]))
                    + (get<2>(tmp[n-1]) - get<2>(tmp[0]))*(get<2>(tmp[n-1]) - get<2>(tmp[0]))
                    ) / (get<3>(tmp[n-1]) - get<3>(tmp[0]));
            }

//            EV <<"Sum: " << sum <<endl;
        }
    }

    double averageSpeed = sum / numPeople;

    return averageSpeed;
}


