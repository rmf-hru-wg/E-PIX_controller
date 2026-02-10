#include "ControllerVolume.h"

ControllerVolume::ControllerVolume(){}

void ControllerVolume::set_volume(int id, int dir){
    this->id = id;
    this->dir = dir;
}
void ControllerVolume::set_pos_ini(int pos_ini){
    this->pos_ini = pos_ini - (VOLUME_MAX+1)/2;
}
void ControllerVolume::set_limit(float min, float max){
    this->min = min;
    this->max = max;
}
void ControllerVolume::init(int id, int dir, int pos_ini, float min, float max){
    set_volume(id, dir);
    set_pos_ini(pos_ini);
    set_limit(min, max);
}

float ControllerVolume::read(){
    int analog_value = analogRead(this->id);
    analog_value = analog_value - this->pos_ini;

    float value = map(analog_value,
                    VOLUME_MIN, VOLUME_MAX,
                    this->min, this->max);
    return value*this->dir;
}
