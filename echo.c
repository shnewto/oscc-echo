#include <stdio.h> // printf
#include <signal.h> // SIGINT
#include <unistd.h> // usleep
#include "oscc.h" // oscc_open, oscc_disable, oscc_close and each subscribe.


// Global indication of Ctrl+C.
static int EXIT_SIGNALED = OSCC_OK;

// Help exiting gracefully.
void signal_handler(int signal_number)
{
    if (signal_number == SIGINT)
    {
        EXIT_SIGNALED = OSCC_ERROR;
    }
}

static void obd_callback(struct can_frame * frame)
{
    // if(frame->can_id == KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID)
    // {
    //     double fl = 0;
    //     double fr = 0;
    //     double rl = 0;
    //     double rr = 0;

    //     (void)get_wheel_speed_front_left(frame, &fl);
    //     (void)get_wheel_speed_front_right(frame, &fr);
    //     (void)get_wheel_speed_rear_left(frame, &rl);
    //     (void)get_wheel_speed_rear_right(frame, &rr);

    //     printf("\n fl: %f -- fr: %f -- rl: %f -- rr: %f \n", fl, fr, rl, rr);
    // }

    if(frame->can_id == KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID)
    {
        double wheel_angle = 0;
        (void)get_steering_wheel_angle(frame, &wheel_angle);
        printf("\n steering wheel angle: %f\n", wheel_angle);
    }

    // if(frame->can_id == KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID)
    // {
    //     double brake_pressure = 0;
    //     (void) get_brake_pressure(frame, &brake_pressure);
    //     printf("\n brake pressure: %f \n", brake_pressure);
    // }

}

int main()
{
    struct sigaction sig;
    sig.sa_handler = signal_handler;
    sigaction(SIGINT, &sig, NULL);
    oscc_subscribe_to_obd_messages(obd_callback);
    oscc_result_t return_value;
    int channel = 0;
    return_value = oscc_open(channel);
    if(return_value != OSCC_OK)
    {
        printf("Error in oscc_open!\n");
        EXIT_SIGNALED = OSCC_ERROR;
    }
    while(EXIT_SIGNALED == OSCC_OK)
    {
        (void) usleep(50000);
    }
    oscc_disable();
    oscc_close(channel);
}