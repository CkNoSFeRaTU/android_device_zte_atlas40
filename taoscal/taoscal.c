#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <math.h>
#include <android/log.h>
#include "taos_common.h"


#define LOG_TAG "TaosCal"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define TAOS_DEV "/dev/taos"
#define CAL_FILE "/data/misc/prox/prox_threshold.txt"

int main(int argc, char **argv) {
    struct taos_cfg cfg;
    int ra = 0;
    int rv = 0;
    FILE *fh = NULL;

    int fd = open(TAOS_DEV, O_RDONLY);
    if (fd > 0) {
        LOGI("Using taos sensor");

        fh = fopen(CAL_FILE,"r");
        if (fh == NULL)
        {
            system("/system/bin/proximity.init -r");
            fh = fopen(CAL_FILE,"r");
        }
        if (fh)
        {
            rv = ioctl(fd, TAOS_IOCTL_CONFIG_GET, &cfg);
            if (rv)
                LOGE("Proximity & light sensor calibration data retrieve failed!!\n");
            else {

                while (fscanf(fh,"%c%u", &ra, &rv) != EOF)
                {
                    switch(ra)
                    {
                        case 'H': cfg.prox_threshold_hi = rv; break;
                        case 'L': cfg.prox_threshold_lo = rv; break;
                        case 'I': cfg.prox_int_time = rv; break;
                        case 'W': cfg.prox_wait_time = rv; break;
                        case 'F': cfg.prox_intr_filter = rv; break;
                        case 'C': cfg.prox_config = rv; break;
                        case 'P': cfg.prox_pulse_cnt = rv; break;
                        case 'G': cfg.prox_gain = rv; break;
                    }
                }

                //v880e ds hacks
                cfg.prox_threshold_lo = cfg.prox_threshold_hi - 128;
                cfg.prox_int_time = 246;
                cfg.prox_adc_time = 255;
                cfg.prox_wait_time = 255;
                cfg.prox_intr_filter = 0;
                cfg.prox_config = 0;
                //v880e ds hacks

                rv = ioctl(fd, TAOS_IOCTL_CONFIG_SET, &cfg);

                LOGI("calibrate_target = %u\n", cfg.calibrate_target);
                LOGI("als_time = %u\n", cfg.als_time);
                LOGI("scale_factor = %u\n", cfg.scale_factor);
                LOGI("gain_trim = %u\n", cfg.gain_trim);
                LOGI("filter_history = %u\n", cfg.filter_history);
                LOGI("filter_count = %u\n", cfg.filter_count);
                LOGI("gain = %u\n", cfg.gain);

                LOGI("prox_theshold_hi = %u\n", cfg.prox_threshold_hi);
                LOGI("prox_theshold_lo = %u\n", cfg.prox_threshold_lo);
                LOGI("prox_int_time = %u\n", cfg.prox_int_time);
                LOGI("prox_adc_time = %u\n", cfg.prox_adc_time);
                LOGI("prox_wait_time = %u\n", cfg.prox_wait_time);
                LOGI("prox_intr_filter = %u\n", cfg.prox_intr_filter);
                LOGI("prox_config = %u\n", cfg.prox_config);
                LOGI("prox_pulse_cnt = %u\n", cfg.prox_pulse_cnt);
                LOGI("prox_gain = %u\n", cfg.prox_gain);

                if (rv)
                    LOGE("Proximity & Light sensor calibration data restore failed!!\n");
                else
                    LOGI("Proximity & Light sensor calibration data restored\n");
            }
        } else {
            LOGE("Proximity & Light sensor calibration data not found!!\n");
        }
        fclose(fh);

        close(fd);
    }

    exit(EXIT_SUCCESS);
}

