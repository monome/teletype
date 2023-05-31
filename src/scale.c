#include "scale.h"


void init_cal_data(cal_data_t* data) {
    if (!data) {
        return;
    }

    data->p_min = 0;
    data->p_max = 16383;
    data->i_min = 0;
    data->i_max = 16383;

    for (uint8_t i = 0; i < 64; i++) {
        data->f_min[i] = 0;
        data->f_max[i] = 16383;
    }

    for (uint8_t j = 0; j < 4; j++) {
        data->cv_scale[j].b = 0;
        data->cv_scale[j].m = 1;
    }
};
