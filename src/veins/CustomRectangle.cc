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

#include "CustomRectangle.h"

CustomRectangle::CustomRectangle() {
    // TODO Auto-generated constructor stub

}

CustomRectangle::CustomRectangle(std::string points) {
    size_t pos;
    std::string token;

    for (int i = 0; i < 4; i++) {
        pos = points.find(" ");
        token = points.substr(0, pos);

        if (i == 0) this->A = string2Coord(token);
        if (i == 1) this->B = string2Coord(token);
        if (i == 2) this->C = string2Coord(token);
        if (i == 3) this->D = string2Coord(token);

        points.erase(0, pos + 1);
    }
}

//inet::Coord CustomRectangle::string2Coord(std::string point) {
veins::Coord CustomRectangle::string2Coord(std::string point) {
//    inet::Coord p;
    veins::Coord p;

    size_t pos = point.find(",");
    std::string token;
    token = point.substr(0, pos);

    p.x = std::stof(token);
    point.erase(0, pos + 1);
    p.y = std::stof(point);
    p.z = 0;

    return p;
}

double CustomRectangle::rectangleArea() {
    double x = A.distance(B);
    double y = B.distance(C);
    return x * y;
}

//double CustomRectangle::triangleArea(inet::Coord A, inet::Coord B, inet::Coord C) {
double CustomRectangle::triangleArea(veins::Coord A, veins::Coord B, veins::Coord C) {
    double p = A.x * (B.y - C.y);
    double q = B.x * (C.y - A.y);
    double r = C.x * (A.y - B.y);
    return fabs((p + q + r) / 2);
}

bool CustomRectangle::checkInside(veins::Coord I) {
    double ABI = triangleArea(A, B, I);
    double BCI = triangleArea(B, C, I);
    double CDI = triangleArea(C, D, I);
    double DAI = triangleArea(D, A, I);
    double ABCD = rectangleArea();
    if (ABI + BCI + CDI + DAI == ABCD) return true;
    else return false;
}

CustomRectangle::~CustomRectangle() {
    // TODO Auto-generated destructor stub
}
