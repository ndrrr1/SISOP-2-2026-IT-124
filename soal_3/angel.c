#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>

char *kata[] = {
    "aku akan fokus pada diriku sendiri",
    "aku mencintaimu dari sekarang hingga selamanya",
    "aku akan menjauh darimu, hingga takdir mempertemukan kita",
    "kalau aku dilahirkan kembali, aku tetap akan menyayangimu"
};

void log_event(const char *status) {
    FILE *f = fopen("ethereal.log", "a");
    if (f == NULL) return;

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    fprintf(f, "[%02d:%02d:%02d][maya][%s]\n",
            tm_info->tm_hour,
            tm_info->tm_min,
            tm_info->tm_sec,
            status);

    fclose(f);
}

void buat_file() {
    FILE *f = fopen("LoveLetter.txt", "w");
    if (f == NULL) {
        log_event("ERROR");
        return;
    }

    int r = rand() % 4;
    fprintf(f, "%s\n", kata[r]);
    fclose(f);
}

void encode_base64() {
    int hasil = system("base64 LoveLetter.txt > temp.txt && mv temp.txt LoveLetter.txt");
    if (hasil != 0) {
        log_event("ERROR");
        printf("[ERROR] Gagal melakukan encode file.\n");
    }
}

void jalan_maya() {
    FILE *pf = fopen("angel.pid", "w");
    if (pf != NULL) {
        fprintf(pf, "%d\n", getpid());
        fclose(pf);
    }

    srand(time(NULL) ^ getpid());

    while (1) {
        buat_file();
        encode_base64();
        log_event("RUNNING");
        sleep(10);
    }
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: ./angel -daemon | -decrypt | -kill\n");
        return 1;
    }

    if (strcmp(argv[1], "-daemon") == 0) {
        pid_t pid = fork();

        if (pid < 0) {
            log_event("ERROR");
            printf("[ERROR] Gagal membuat daemon.\n");
            exit(1);
        }

        if (pid > 0) {
            printf("[INFO] Daemon berhasil dijalankan.\n");
            exit(0);
        }

        execlp("bash", "bash", "-c", "exec -a maya ./angel -worker", NULL);
        log_event("ERROR");
        printf("[ERROR] Gagal mengubah nama proses menjadi maya.\n");
        exit(1);
    }

    else if (strcmp(argv[1], "-worker") == 0) {
        jalan_maya();
    }

    else if (strcmp(argv[1], "-decrypt") == 0) {
        FILE *pf = fopen("angel.pid", "r");
        if (pf != NULL) {
            fclose(pf);
            printf("[ERROR] Daemon masih berjalan. Matikan dulu dengan ./angel -kill sebelum decrypt.\n");
            log_event("ERROR");
            return 1;
        }

        int hasil = system("base64 -d LoveLetter.txt > temp.txt && mv temp.txt LoveLetter.txt");
        if (hasil == 0) {
            log_event("SUCCESS");
            printf("[INFO] File berhasil didecrypt.\n");
        } else {
            log_event("ERROR");
            printf("[ERROR] Gagal decrypt file.\n");
        }
    }

    else if (strcmp(argv[1], "-kill") == 0) {
        FILE *pf = fopen("angel.pid", "r");
        if (pf == NULL) {
            log_event("ERROR");
            printf("[ERROR] PID daemon tidak ditemukan.\n");
            return 1;
        }

        int pid;
        fscanf(pf, "%d", &pid);
        fclose(pf);

        if (kill(pid, SIGTERM) == 0) {
            remove("angel.pid");
            log_event("SUCCESS");
            printf("[INFO] Daemon berhasil dihentikan.\n");
        } else {
            log_event("ERROR");
            printf("[ERROR] Gagal menghentikan daemon.\n");
        }
    }

    else {
        printf("[ERROR] Argumen tidak valid.\n");
        printf("Usage: ./angel -daemon | -decrypt | -kill\n");
        return 1;
    }

    return 0;
}
