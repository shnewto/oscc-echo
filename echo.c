#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <bits/time.h>
#include <sys/timeb.h>

#include <stdio.h> // printf
#include <signal.h> // SIGINT
#include <unistd.h> // usleep
#include "oscc.h" // oscc_open, oscc_disable, oscc_close and each subscribe.


// Global indication of Ctrl+C.
static int EXIT_SIGNALED = OSCC_OK;
static FILE * GLOBAL_FILE_POINTER = NULL;

// Help exiting gracefully.
void signal_handler(int signal_number)
{
    if (signal_number == SIGINT)
    {
        EXIT_SIGNALED = OSCC_ERROR;
    }
}

unsigned long long get_timestamp()
{
    unsigned long long timestamp;
    struct timespec timespec;
    clock_gettime( CLOCK_REALTIME, &timespec );
    // to micro seconds
    timestamp = (unsigned long long)timespec.tv_sec * 1000000ULL;
    // remainder to micro seconds
    timestamp += (unsigned long long)timespec.tv_nsec  / 1000ULL;

    return timestamp;
}

static void obd_callback(struct can_frame * frame)
{
    unsigned long long timestamp = get_timestamp();

    if(frame->can_id == KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID)
    {
        double lf = 0;
        double rf = 0;
        double lr = 0;
        double rr = 0;

        (void)get_wheel_speed_left_front(frame, &lf);
        (void)get_wheel_speed_right_front(frame, &rf);
        (void)get_wheel_speed_left_rear(frame, &lr);
        (void)get_wheel_speed_right_rear(frame, &rr);

        // RR
        printf("\nRR: %f", rr);

        // LR
        printf("\tLR: %f", lr);

        // RF
        printf("\tRF: %f", rf);

        // LF
        printf("\tLF: %f \n", lf);

        if(GLOBAL_FILE_POINTER != NULL)
        {
            fprintf(GLOBAL_FILE_POINTER, "%llu,%f,%f,%f,%f,", timestamp, rr, lr, rf, lf);
        }
    }

    if(frame->can_id == KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID)
    {
        double wheel_angle = 0;
        (void)get_steering_wheel_angle(frame, &wheel_angle);
        printf("\n steering wheel angle: %f\n", wheel_angle);
    }

    if(frame->can_id == KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID)
    {
        double brake_pressure = 0;
        (void) get_brake_pressure(frame, &brake_pressure);
        printf("\n brake pressure: %f \n", brake_pressure);
    }

}

int main()
{
    GLOBAL_FILE_POINTER = fopen("wheel_speed.csv", "w+");
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
        (void) usleep(50);
    }
    oscc_disable();
    oscc_close(channel);
    fclose(GLOBAL_FILE_POINTER);
}