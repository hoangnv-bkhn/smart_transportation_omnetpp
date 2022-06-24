#ifndef VEINS_INET_RSUCONTROLAPP_H_
#define VEINS_INET_RSUCONTROLAPP_H_

#pragma once

#include "veins/veins.h"

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/TraCIDemoRSU11p.h"
#include "CustomRectangle.h"
#include "Crossing.h"
#include "Constant.h"

using namespace omnetpp;
using people = std::vector<std::tuple<std::string, double, double, double>>;
using personPosition = std::tuple<std::string, double, double, double>;

namespace veins {

class HospitalControlApp : public TraCIDemoRSU11p {
public:
    void initialize(int stage) override;
    void finish() override;
    double getAvailablePerdestrian(std::string crossId, double _time);
    double getVeloOfPerdestrian(std::string crossId, double _time);
    double predictDisperseTime(std::string crossId, int _t, int k);
    people getPeopleByTime(people list, int u);
    double calculateSum(personPosition elem, personPosition elem2, double L);

protected:
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
private:
    bool hasStopped = false;
    int subscribedServiceId = 0;
    cMessage* sendBeacon;
};
}

#endif /* VEINS_INET_RSUCONTROLAPP_H_ */
