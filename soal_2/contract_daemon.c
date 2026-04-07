#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>

FILE *file_log;

char *status_list[] = {"awake", "drifting", "numbness"};

void tulis_log(char *msg) {
    file_log = fopen("work.log", "a");
    if (file_log == NULL) return;

    fprintf(file_log, "%s\n", msg);
    fclose(file_log);
}

void buat_contract() {
    FILE *f = fopen("contract.txt", "w");
    if (f == NULL) return;

    time_t now = time(NULL);
    fprintf(f, "A promise to keep going, even when unseen.\n");
    fprintf(f, "created at: %s", ctime(&now));

    fclose(f);
}

void handler(int sig) {
    (void)sig;
    tulis_log("We really weren't meant to be together.");
    exit(0);
}

int main() {
    pid_t pid = fork();

    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    signal(SIGTERM, handler);
    signal(SIGINT, handler);

    srand(time(NULL));

    char last_content[256] = "";

    while (1) {
        int r = rand() % 3;

        char msg[100];
        sprintf(msg, "still working... [%s]", status_list[r]);
        tulis_log(msg);

        FILE *f = fopen("contract.txt", "r");

        if (f == NULL) {
            buat_contract();

            char restore_msg[100];

            // 🔥 BAGIAN YANG SUDAH DIPERBAIKI
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            sprintf(restore_msg, "restored at: %02d:%02d:%02d",
                    tm_info->tm_hour,
                    tm_info->tm_min,
                    tm_info->tm_sec);

            tulis_log(restore_msg);
        } else {
            char buffer[256] = "";
            fread(buffer, 1, sizeof(buffer) - 1, f);
            fclose(f);

            if (strlen(last_content) != 0 && strcmp(buffer, last_content) != 0) {
                tulis_log("contract violated.");
            }

            strcpy(last_content, buffer);
        }

        sleep(5);
    }

    return 0;
}
