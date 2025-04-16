#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/log.h>
#include <uORB/topics/sensor_combined.h>
#include <uORB/topics/rc_channels.h>
extern "C" __EXPORT int hello_world_main(int argc, char *argv[]);

int hello_world_main(int argc, char *argv[])
{
    int sensor_combined_handle;
    rc_channels_s sensor_data;
    sensor_combined_handle = orb_subscribe(ORB_ID(rc_channels));
    orb_set_interval(sensor_combined_handle, 200);
    while(1)
    {
        orb_copy(ORB_ID(rc_channels), sensor_combined_handle, &sensor_data);
        PX4_INFO("1 = %f\t 2 = %f\t 3 = %f\t"
                "4 = %f\t 5 = %f\t 6 = %f\t"
                "7 = %f\t 8 = %f\t 9 = %f\t"
                "10 = %f\t 11 = %f\t 12 = %f\t"
                "13 = %f\t 14 = %f\t 15 = %f\t"
                "16 = %f\t 17 = %f\t 18 = %f\t", (double)sensor_data.channels[0],
        (double)sensor_data.channels[1],
        (double)sensor_data.channels[2],
        (double)sensor_data.channels[3],
        (double)sensor_data.channels[4],
        (double)sensor_data.channels[5],
        (double)sensor_data.channels[6],
        (double)sensor_data.channels[7],
        (double)sensor_data.channels[8],
        (double)sensor_data.channels[9],
        (double)sensor_data.channels[10],
        (double)sensor_data.channels[11],
        (double)sensor_data.channels[12],
        (double)sensor_data.channels[13],
        (double)sensor_data.channels[14],
        (double)sensor_data.channels[15],
        (double)sensor_data.channels[16],
        (double)sensor_data.channels[17]);
        px4_usleep(200000);
    }
    return 0;
}