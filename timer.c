#include <stdio.h>
#include <time.h>
#include <unistd.h>

void print_timer() {
    int hours = 0, minutes = 0, seconds = 0;
    while(1) {
        printf("%02d:%02d:%02d\n", hours, minutes, seconds);
        seconds++;
        if (seconds == 60) {
            seconds = 0;
            minutes++;
        }
        if (minutes == 60) {
            minutes = 0;
            hours++;
        }
        if (hours == 24) {
            hours = 0;
        }
        sleep(1); // Wait for 1 second before printing the next time
    }
}

int main() {
    print_timer();
    return 0;
}