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
// along with this program. If not, see http://www.gnu.org/licenses/.
// 

#include "HospitalControlApp.h"
#include <stdio.h>
#include <iostream>
#include "Zone.h"
#include "Graph.h"
//#include "HashAPI.cpp"
//#include "UnitTest.cpp"
//#include "ReadFile.cpp"



using namespace veins;

Register_Class(HospitalControlApp);

void HospitalControlApp::initialize(int stage)
{
    TraCIDemoRSU11p::initialize(stage);
    if(graphGenerator == NULL){
        graphGenerator = new Parser();
        //graph = new Graph();
        //graphGenerator->readFile();
    }
    if (stage == 0) {
        this->readCrossing();

        sendBeacon= new cMessage("send Beacon");
        graph = new Graph();
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }
}

void HospitalControlApp::finish()
{
    //Duoc goi khi RSU ket thuc cong viec
    TraCIDemoRSU11p::finish();
    if(traci == NULL){
        EV<<"NULL eventually"<<endl;
    }
    if(Constant::activation == NULL){
            EV<<"Constant is helpless eventually"<<endl;
    }
//    for (auto it = crossings.begin(); it != crossings.end(); it++){
//        EV<<it->id << " Name: "<<it->name<<endl;
//    }

    //if period is 0.1 => count = 1453
    //if period is 0.2 => count = 801
//    EV<<"#calling traci->getPersonIds(): "<<count<<endl;
//    EV<<"Total waiting time: "<<Constant::TOTAL_WAITING_TIME*0.1<<"(s)"<<endl;
//    EV<<"Total travelling time: "<<Constant::TOTAL_TRAVELLING_TIME<<"(s)"<<endl;
//    double percentage = Constant::TOTAL_WAITING_TIME*10/Constant::TOTAL_TRAVELLING_TIME;
//    EV<<"% of waiting time: "<<percentage<<endl;
    // statistics recording goes here
    double a = getAvailablePerdestrian(":J43", "c1", 9);
    EV << "Xac suat: " <<a << endl;
    double b = getVeloOfPerdestrian(":J43", "c1", 9);
    EV << "Average Velocity: " <<b << endl;
}

void HospitalControlApp::onBSM(DemoSafetyMessage* bsm)
{
    //for my own simulation circle
}

void HospitalControlApp::onWSM(BaseFrame1609_4 *wsm){
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
    cPacket *enc = wsm->getEncapsulatedPacket();
    if (TraCIDemo11pMessage *bc = dynamic_cast<TraCIDemo11pMessage*>(enc)) {
        if (sendBeacon != NULL) {
            if (sendBeacon->isScheduled()) {
                cancelEvent(sendBeacon);
            }

            if(traci == NULL){
                if(Constant::activation != NULL)
                traci = Constant::activation->getCommandInterface();
            }

            if(simTime().dbl() - lastUpdate >= 0.2){
                count++;
                std::list<std::string> allPeople = traci->getPersonIds();
                double x, y;
                //for(int i = 0; i < crossings.size(); i++){
                for (auto elem: allPeople) {
                    std::string personId = elem;
//                    EV << personId <<endl;
                    Coord peoplePosition = traci->getPersonPosition(personId);
                    std::pair<double,double> coordTraCI = traci->getTraCIXY(peoplePosition);
                    //veins::Coord newCoord;
                    x = coordTraCI.first;
                    y = coordTraCI.second;
//                    EV <<x << " " <<y <<endl;
                    //newCoord.z = 0;

                    for(int i = 0; i < crossings.size(); i++){
//                        EV << "Hello " <<crossings[i].rec->checkInside(x, y)<<endl;
                        if (crossings[i].rec->checkInside(x, y)) {
//                            EV <<"Nguoi o: "<<crossings[i].id<<endl;
                            crossings[i].peoples.push_back(std::make_tuple(personId, x, y, simTime().dbl()));
                            break;
                        }
                        else if (crossings[i].rec->checkAround(x, y)){
                            break;
                        }
                    }
                }

                lastUpdate = simTime().dbl();
            }

            TraCIDemo11pMessage *rsuBeacon = new TraCIDemo11pMessage();

            char *ret = mergeContent(bc->getSenderAddress());
            rsuBeacon->setDemoData(ret);
            rsuBeacon->setSenderAddress(myId);
            BaseFrame1609_4 *WSM = new BaseFrame1609_4();
            WSM->encapsulate(rsuBeacon);
            populateWSM(WSM);
            send(WSM, lowerLayerOut);
        }
        readMessage(bc);

    }

}

void HospitalControlApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void HospitalControlApp::handleSelfMsg(cMessage* msg)
{
    TraCIDemoRSU11p::handleSelfMsg(msg);
}

void HospitalControlApp::handlePositionUpdate(cObject* obj)
{
    TraCIDemoRSU11p::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class

}

void HospitalControlApp::readCrossing(){
    std::string line;
    std::ifstream MyReadFile("crossing.txt");
    getline(MyReadFile, line);
    int numberOfCrossing =std::stoi(line);

    int k = 0;

    while (getline(MyReadFile, line)) {
        size_t pos;
        std::string token;
        Crossing tmp;

        for (int i = 0; i < 2; i++) {
            pos = line.find(" ");
            token = line.substr(0, pos);

            if (i == 0) tmp.id = token;
            if (i == 1) tmp.name = token; //std::atof(token.c_str());
            //if (i == 2) tmp.from = token;
            //if (i == 3) tmp.to = token;
            line.erase(0, pos + 1);

        }
        tmp.rec = new CustomRectangle(line);
        crossings.push_back(tmp);
        k++;
    }

    MyReadFile.close();
}

void HospitalControlApp::exponentialSmoothing(NodeVertex *nv, double stopTime) {
    if (nv->v->k == 0) {
        nv->v->predictW = stopTime;
        nv->v->d = nv->v->q = 0;
        nv->v->k++;
    } else {
        double error = stopTime - nv->v->predictW;
        nv->v->q = Constant::GAMMA * error - (1 - Constant::GAMMA) * nv->v->q;
        nv->v->d = Constant::GAMMA * abs(error)
                - (1 - Constant::GAMMA) * nv->v->d;
        double lambda = abs(nv->v->q / nv->v->d);
        nv->v->predictW = lambda * stopTime + (1 - lambda) * nv->v->predictW;
//        mes = mes + " " + std::to_string(nv->v->predictW);
    }
}

void HospitalControlApp::readLane(AGV *cur, std::string str) {
    double stopTime = cur->itinerary->stopTime * 0.1;
    str.erase(str.find("_"));
    cur->itinerary->laneId = str;
    if (cur->itinerary->prevLaneId.length() == 0) {
        cur->itinerary->prevLaneId = str;
    } else if (cur->itinerary->laneId.compare(cur->itinerary->prevLaneId)
            != 0) {
        std::string mes;
        if (cur->itinerary->prevLaneId.front() == ':') {
            mes = cur->itinerary->laneId + " " + std::to_string(stopTime);
            NodeVertex *nv = graph->searchVertex(cur->itinerary->prevLaneId);
            exponentialSmoothing(nv, stopTime);
            nv->v->setW(stopTime);
            message.push_back(mes);
        } else if (cur->itinerary->laneId.front() == ':') {
            std::string full_name = cur->itinerary->prevLaneId + "-" + str;
            mes = full_name + " " + std::to_string(stopTime);
            NodeVertex *nv = graph->searchVertex(full_name);
            if(nv != NULL){
                exponentialSmoothing(nv, stopTime);
                nv->v->setW(stopTime);
            }
            message.push_back(mes);
        }
        if (cur->itinerary->prevLaneId.front()
                != cur->itinerary->laneId.front())
            cur->itinerary->stopTime = 0;
        cur->itinerary->prevLaneId = cur->itinerary->laneId;
    }
}

void HospitalControlApp::readMessage(TraCIDemo11pMessage *bc) {
    std::stringstream streamData(bc->getDemoData());
    std::string str;
    AGV *cur = NULL;
    for (auto a : vhs) {
        if (a->id.compare(std::to_string(bc->getSenderAddress())) == 0)
            cur = a;
    }
    if (cur == NULL) {
        cur = new AGV();//3 dong sau ghep thanh 1 phan ptkd cua AGV co tham so truyen vao
        cur->id = std::to_string(bc->getSenderAddress());
        cur->itinerary = new ItineraryRecord();
        cur->itinerary->stopTime = 0;
        vhs.push_back(cur);
    }
    int i = 0;
    while (getline(streamData, str, ' ')) {
        if (i == 0) {
            readLane(cur, str);
        } else if (i == 2) {
            if (std::stod(str) == 0) {
                cur->itinerary->stopTime++;
            }
        }
        i++;
    }
}

double HospitalControlApp::getAvailablePerdestrian(std::string crossId, std::string name, double _time) {
    int count = 0;
    double start = 0;

    if (_time - Constant::DELTA_T > 0) {
        start = _time - Constant::DELTA_T;
    }
    double tmp = (_time - start)/0.1;
    EV << "Start: "<<start << " End: "<< _time <<endl;
    auto it = find_if(crossings.begin(), crossings.end(), [&crossId, &name](const Crossing& obj) {return obj.id.compare(crossId) == 0 && obj.name.compare(name) == 0;});
    if (it != crossings.end())
    {
        double pivot = start;
        do {
//            EV << it->peoples.size() <<endl;
            for(auto elem : it->peoples) {
                if (pivot <= std::get<3>(elem) && std::get<3>(elem) < pivot + 0.1) {
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

double HospitalControlApp::getVeloOfPerdestrian(std::string crossId, std::string name, double _time) {
    double start = 0;
    if (_time - Constant::DELTA_T > 0) {
        start = _time - Constant::DELTA_T;
    }
    //EV << "Start: "<<start << " End: "<< _time <<endl;

    std::set < std::string > personIds;
    double sum = 0;
    int numPeople = 0;
    auto it = find_if(crossings.begin(), crossings.end(), [&crossId, &name](const Crossing& obj) {return obj.id.compare(crossId) == 0 && obj.name.compare(name) == 0;});
    if (it != crossings.end())
    {
        for(auto elem : it->peoples) {
            if (start <= std::get<3>(elem) && std::get<3>(elem) < _time) {
                personIds.insert(std::get<0>(elem));
            }
        }
        numPeople = personIds.size();
        for(auto person : personIds) {
            std::vector<std::tuple<std::string, double, double, double>> tmp;
            for(auto elem : it->peoples) {
                if(std::get<0>(elem).compare(person) == 0 && start <= std::get<3>(elem) && std::get<3>(elem) <= _time) {
                    tmp.push_back(elem);
                }
            }
            int n = tmp.size();
            if (n > 1){
                sum += sqrt(
                    (std::get<1>(tmp[n-1]) - std::get<1>(tmp[0]))*(std::get<1>(tmp[n-1]) - std::get<1>(tmp[0]))
                    + (std::get<2>(tmp[n-1]) - std::get<2>(tmp[0]))*(std::get<2>(tmp[n-1]) - std::get<2>(tmp[0]))
                    ) / (std::get<3>(tmp[n-1]) - std::get<3>(tmp[0]));
            }

        }
    }

    double averageSpeed = sum / numPeople;

    return averageSpeed;
}


