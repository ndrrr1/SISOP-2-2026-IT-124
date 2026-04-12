#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>

FILE *file_log;

char *status_list[] = {"awake", "drifting", "numbness"};
const char *isi_asli = "A promise to keep going, even when unseen.\n";

void tulis_log(const char *msg) {
    file_log = fopen("work.log", "a");
    if (file_log == NULL) return;

    fprintf(file_log, "%s\n", msg);
    fclose(file_log);
}

void buat_contract() {
    FILE *f = fopen("contract.txt", "w");
    if (f == NULL) return;

    time_t now = time(NULL);
    fprintf(f, "%s", isi_asli);
    fprintf(f, "created at: %s", ctime(&now));

    fclose(f);
}

void log_restore_time() {
    char restore_msg[100];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    sprintf(restore_msg, "restored at: %02d:%02d:%02d",
            tm_info->tm_hour,
            tm_info->tm_min,
            tm_info->tm_sec);

    tulis_log(restore_msg);
}

void restore_contract_karena_diubah() {
    buat_contract();
    tulis_log("contract violated.");
    log_restore_time();
}

void handler(int sig) {
    (void)sig;
    tulis_log("We really weren't meant to be together.");
    exit(0);
}

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        printf("[ERROR] Gagal membuat daemon.\n");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    signal(SIGTERM, handler);
    signal(SIGINT, handler);

    srand(time(NULL));

    if (access("contract.txt", F_OK) != 0) {
        buat_contract();
    }

    while (1) {
        int r = rand() % 3;

        char msg[100];
        sprintf(msg, "still working... [%s]", status_list[r]);
        tulis_log(msg);

        FILE *f = fopen("contract.txt", "r");

        if (f == NULL) {
            buat_contract();
            log_restore_time();
        } else {
            char buffer[512] = "";
            fread(buffer, 1, sizeof(buffer) - 1, f);
            fclose(f);

            if (strstr(buffer, isi_asli) == NULL) {
                restore_contract_karena_diubah();
            }
        }

        sleep(5);
    }

    return 0;
}
