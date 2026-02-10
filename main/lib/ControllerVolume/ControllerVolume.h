#include <Arduino.h>

#define FORWARD 1
#define REVERSE -1
#define VOLUME_MIN 0
#define VOLUME_MAX 4095

class ControllerVolume{
public:
    ControllerVolume();

    void set_volume(int id, int dir);
    void set_pos_ini(int pos_ini); // position 3500-11500
    void set_limit(float min, float max); // radian
    // init all
    void init(int id, int dir, int pos_ini, float min, float max);

    // return radian value
    float read();

private:
    int id;
    int dir;

    int pos_ini;
    float max;
    float min;
};
