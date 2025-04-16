#include <drivers/drv_hrt.h>
#include <uORB/Publication.hpp>
#include <uORB/topics/test_motor.h>
#include <uORB/topics/debug_value.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/log.h>
#include <uORB/topics/rc_channels.h>

#define DC_MOTOR 0
#define SERVO_MOTOR 1

extern "C" __EXPORT int hello_world_main(int argc, char *argv[]);

int hello_world_main(int argc, char *argv[])
{

    px4_sleep(2);

    debug_value_s debug_data;
    int debug_handle = orb_subscribe(ORB_ID(debug_value));
    orb_set_interval(debug_handle, 500);

    test_motor_s test_motor;
    test_motor_s test_motor2;
    uORB::Publication<test_motor_s> test_motor_pub(ORB_ID(test_motor));

    px4_sleep(2);

    while(1)
    {
        orb_copy(ORB_ID(debug_value), debug_handle, &debug_data);

        test_motor.timestamp = hrt_absolute_time();
        test_motor.motor_number = DC_MOTOR;
        test_motor.action = test_motor_s::ACTION_RUN;
        test_motor.driver_instance = 0;
        test_motor.timeout_ms = 0;

        test_motor2.timestamp = hrt_absolute_time();
        test_motor2.motor_number = SERVO_MOTOR;
        test_motor2.action = test_motor_s::ACTION_RUN;
        test_motor2.driver_instance = 0;
        test_motor2.timeout_ms = 0;

        if((int)debug_data.value == 3) {
            test_motor.value = 0.5;
        }else if ((int)debug_data.value == 2){
            test_motor.value = 0.6;
        }else if ((int)debug_data.value == 1){
            test_motor.value = 0.9;
        }

        if((int)debug_data.ind == 0) {
            test_motor2.value = 1;
        }else if ((int)debug_data.ind == 2){
            test_motor2.value = 0;
        }else if ((int)debug_data.ind == 1){
            test_motor2.value = 0.5;
        }

        test_motor_pub.publish(test_motor);
        test_motor_pub.publish(test_motor2);

        px4_usleep(500000);
    }
    return 0;
}