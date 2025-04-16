#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/log.h>
#include <uORB/topics/sensor_combined.h>
#include <uORB/topics/rc_channels.h>

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/log.h>
#include <drivers/drv_hrt.h>
#include <uORB/Publication.hpp>
#include <uORB/topics/test_motor.h>

#define DC_MOTOR 0
#define SERVO_MOTOR 1

extern "C" __EXPORT int hello_world_main(int argc, char *argv[]);

int hello_world_main(int argc, char *argv[])
{
    int sensor_combined_handle;
    rc_channels_s sensor_data;
    sensor_combined_handle = orb_subscribe(ORB_ID(rc_channels));
    orb_set_interval(sensor_combined_handle, 200);

    test_motor_s test_motor;
    test_motor_s test_motor2;
    double motor_value_dc = 0; // a number between 0 to 1
    double motor_value_servo = 0; // a number between 0 to 1

    uORB::Publication<test_motor_s> test_motor_pub(ORB_ID(test_motor));

    while(1)
    {
        orb_copy(ORB_ID(rc_channels), sensor_combined_handle, &sensor_data);
//        PX4_INFO("1 = %f\t 2 = %f\t 3 = %f\t"
//                "4 = %f\t 5 = %f\t 6 = %f\t"
//                "7 = %f\t 8 = %f\t",
//		(double)sensor_data.channels[0],
//        (double)sensor_data.channels[1],
//        (double)sensor_data.channels[2],
//        (double)sensor_data.channels[3],
//        (double)sensor_data.channels[4],
//        (double)sensor_data.channels[5],
//        (double)sensor_data.channels[6],
//        (double)sensor_data.channels[7]);
        motor_value_dc = (((double)sensor_data.channels[1]) / 2) + 0.5;
        motor_value_servo = (((double)sensor_data.channels[3]) / 2) + 0.5;
//		 if (scanf("%lf", &motor_value) != 1) {
//			 // Clear the input buffer in case of invalid input
//			 while (getchar() != '\n');
//			 break;
//		 }

//		 if(motor_value > 1.0 || motor_value < 0)
//		 break;

        PX4_INFO("DC speed is %f", motor_value_dc);
		test_motor.timestamp = hrt_absolute_time();
		test_motor.motor_number = DC_MOTOR;
		test_motor.value = (float)motor_value_dc;
		test_motor.action = test_motor_s::ACTION_RUN;
		test_motor.driver_instance = 0;
		test_motor.timeout_ms = 0;
		test_motor_pub.publish(test_motor);

		PX4_INFO("Servo Angle is %f", motor_value_servo);
		test_motor2.timestamp = hrt_absolute_time();
		test_motor2.motor_number = SERVO_MOTOR;
		test_motor2.value = (float)motor_value_servo;
		test_motor2.action = test_motor_s::ACTION_RUN;
		test_motor2.driver_instance = 0;
		test_motor2.timeout_ms = 0;
		test_motor_pub.publish(test_motor2);

    }

     PX4_INFO("The motor will be stopped");
	 test_motor.timestamp = hrt_absolute_time();
	 test_motor.motor_number = DC_MOTOR;
	 test_motor.value = 0.5;
	 test_motor.driver_instance = 0;
	 test_motor.timeout_ms = 0;

	 test_motor_pub.publish(test_motor);

	 PX4_INFO("The motor will be stopped");
	 test_motor2.timestamp = hrt_absolute_time();
	 test_motor2.motor_number = SERVO_MOTOR;
	 test_motor2.value = 0.5;
	 test_motor2.driver_instance = 0;
	 test_motor2.timeout_ms = 0;

	 test_motor_pub.publish(test_motor2);

    return 0;
}