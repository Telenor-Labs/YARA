#include <stdio.h>
#include <sys/sysinfo.h>

int main() {
    struct sysinfo info;
    if (sysinfo(&info) < 0){
        perror(NULL);
    };

    for (int i = 1; i <= 5; ++i) {
        printf("The system has been up for %ld secondsn\n",info.uptime);
    }
    

    return 0;
}