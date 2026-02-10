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
    void set_map_limit(float map_min, float map_max); // radian
    void set_volume_limit(int volume_min, int volume_max); // analog value
    // init all
    void init(int id, int dir, int pos_ini, float map_min, float map_max);

    // return radian value
    float read();

private:
    int id;
    int dir;

    int pos_ini;
    float map_min;
    float map_max;
    int volume_min = VOLUME_MIN;
    int volume_max = VOLUME_MAX;
};
