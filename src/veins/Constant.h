#ifndef VEINS_INET_CONSTANT_H_
#define VEINS_INET_CONSTANT_H_

#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include "veins/modules/mobility/traci/TraCIMobility.h"


class Constant {
public:
    static constexpr const char* FIRST = "First vehicle";
    static constexpr const char* RSU_IDENTIFY = "RSU";

//    inline static veins::TraCIMobility* activation = NULL;
    static veins::TraCIMobility* activation;
    static constexpr const char* DELTA_T = 0;

    Constant();
    virtual ~Constant();
};

static char* mergeContent(long Id){
    char *cstr = new char[strlen(Constant::RSU_IDENTIFY) + 1];
    strcpy(cstr, Constant::RSU_IDENTIFY);
    std::string str = std::to_string(Id);
    char *new_str = new char[str.length() + 1];
    strcpy(new_str, str.c_str());
    char *ret = new char[strlen(Constant::RSU_IDENTIFY) + strlen(new_str) + 1];
    strcpy(ret, cstr);
    strcat(ret, new_str);
    return ret;
}

#endif /* VEINS_INET_CONSTANT_H_ */
