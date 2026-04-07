#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void gagal() {
    printf("[ERROR] Aiya! Proses gagal, file atau folder tidak ditemukan.\n");
    exit(EXIT_FAILURE);
}

void tunggu_anak(pid_t pid) {
    int status;
    waitpid(pid, &status, 0);

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        gagal();
    }
}

int main() {
    pid_t pid;

    // Langkah 1: buat folder brankas_kedai
    pid = fork();
    if (pid < 0) gagal();

    if (pid == 0) {
        execlp("mkdir", "mkdir", "-p", "brankas_kedai", NULL);
        exit(EXIT_FAILURE);
    }
    tunggu_anak(pid);

    // Langkah 2: copy buku_hutang.csv ke brankas_kedai
    pid = fork();
    if (pid < 0) gagal();

    if (pid == 0) {
        execlp("cp", "cp", "buku_hutang.csv", "brankas_kedai/", NULL);
        exit(EXIT_FAILURE);
    }
    tunggu_anak(pid);

    // Langkah 3: ambil data dengan status Belum Lunas
    pid = fork();
    if (pid < 0) gagal();

    if (pid == 0) {
        execlp(
            "bash", "bash", "-c",
            "grep 'Belum Lunas' brankas_kedai/buku_hutang.csv > brankas_kedai/daftar_penunggak.txt",
            NULL
        );
        exit(EXIT_FAILURE);
    }
    tunggu_anak(pid);

    // Langkah 4: zip folder menjadi rahasia_muthu.zip
    pid = fork();
    if (pid < 0) gagal();

    if (pid == 0) {
        execlp("zip", "zip", "-r", "rahasia_muthu.zip", "brankas_kedai", NULL);
        exit(EXIT_FAILURE);
    }
    tunggu_anak(pid);

    printf("[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.\n");
    return 0;
}
