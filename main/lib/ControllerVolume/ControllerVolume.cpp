#include "ControllerVolume.h"

ControllerVolume::ControllerVolume(){}

void ControllerVolume::set_volume(int id, int dir){
    this->id = id;
    this->dir = dir;
}
void ControllerVolume::set_pos_ini(int pos_ini){
    this->pos_ini = pos_ini - (VOLUME_MAX+1)/2;
}
void ControllerVolume::set_map_limit(float map_min, float map_max){
    this->map_min = map_min;
    this->map_max = map_max;
}
void ControllerVolume::set_volume_limit(int volume_min, int volume_max){
    this->volume_min = volume_min;
    this->volume_max = volume_max;
}
void ControllerVolume::init(int id, int dir, int pos_ini, float map_min, float map_max){
    set_volume(id, dir);
    set_pos_ini(pos_ini);
    set_map_limit(map_min, map_max);
}

float ControllerVolume::read(){
    int analog_value = analogRead(this->id);

    float value = map(analog_value,
                    float(this->volume_min), float(this->volume_max),
                    this->map_min, this->map_max);

    float pos_ini = map(this->pos_ini,
                    float(VOLUME_MIN), float(VOLUME_MAX),
                    float(this->volume_min), float(this->volume_max));
    return (value-pos_ini)*this->dir;
    // return analog_value;
}
