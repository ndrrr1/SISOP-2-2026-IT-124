# SISOP-2-2026-IT-124

* **Nama**  : Ndaru Satria Tama
* **NRP**   : 5027251124
* **Kelas** : C

---

## Struktur Repository

```text
.
├── README.md
├── soal_1
│   ├── buku_hutang.csv
│   ├── kasir_muthu.c
│   ├── rahasia_muthu.zip
│   └── brankas_kedai
│       ├── buku_hutang.csv
│       └── daftar_penunggak.txt
├── soal_2
│   ├── contract_daemon.c
│   ├── contract.txt
│   └── work.log
└── soal_3
    ├── angel.c
    ├── LoveLetter.txt
    └── ethereal.log
```

---

# Pendahuluan

Di modul ini saya mengerjakan tiga program C yang fokusnya beda-beda, tapi semuanya masih nyambung ke konsep dasar Sistem Operasi. Ada yang fokus ke proses berurutan, ada yang fokus ke daemon yang memantau file, dan ada juga yang fokus ke daemon yang berjalan dengan nama proses tertentu lalu melakukan encode/decode file.

Secara garis besar:

* **Soal 1** membahas proses berurutan menggunakan `fork()`, `exec()`, dan `waitpid()`
* **Soal 2** membahas daemon yang menjaga file tetap aman
* **Soal 3** membahas daemon dengan nama proses `maya`, pembuatan file berkala, encode/decode, dan logging

Jadi isi modul ini bukan cuma tentang bikin program jalan, tapi juga memahami bagaimana program bisa menjalankan command Linux, menjaga urutan eksekusi, dan tetap aktif di background sebagai daemon.

---

# Reporting Soal 1

## A. Deskripsi Soal

Pada soal pertama, dibuat program C bernama `kasir_muthu.c` untuk mengamankan data hutang milik Uncle Muthu. Program ini harus menjalankan beberapa langkah sistem secara **berurutan**, yaitu:

1. membuat folder `brankas_kedai`
2. menyalin file `buku_hutang.csv` ke folder tersebut
3. mengambil data dengan status **Belum Lunas**
4. menyimpan hasilnya ke `daftar_penunggak.txt`
5. mengarsipkan folder menjadi `rahasia_muthu.zip`

Karena prosesnya harus berjalan satu per satu dan tidak boleh lompat, program menggunakan kombinasi:

* `fork()`
* `exec()`
* `waitpid()`

---

## B. File yang Digunakan

* `soal_1/kasir_muthu.c`
* `soal_1/buku_hutang.csv`
* `soal_1/brankas_kedai/buku_hutang.csv`
* `soal_1/brankas_kedai/daftar_penunggak.txt`
* `soal_1/rahasia_muthu.zip`

---

## C. Kode Lengkap `kasir_muthu.c`

```c
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

    pid = fork();
    if (pid < 0) gagal();

    if (pid == 0) {
        execlp("mkdir", "mkdir", "-p", "brankas_kedai", NULL);
        exit(EXIT_FAILURE);
    }
    tunggu_anak(pid);

    pid = fork();
    if (pid < 0) gagal();

    if (pid == 0) {
        execlp("cp", "cp", "buku_hutang.csv", "brankas_kedai/", NULL);
        exit(EXIT_FAILURE);
    }
    tunggu_anak(pid);

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
```

---

## D. Cara Menjalankan dan Hasil per Tahap

### 1. Masuk ke folder soal 1

```bash
cd soal_1
```

Hasil:

* terminal berada di folder `soal_1`

### 2. Compile program

```bash
gcc kasir_muthu.c -o kasir_muthu
```

Hasil yang diharapkan:

* file executable `kasir_muthu` berhasil dibuat
* tidak ada error saat compile

Kalau mau memastikan:

```bash
ls
```

Biasanya akan terlihat:

```text
buku_hutang.csv
kasir_muthu
kasir_muthu.c
```

### 3. Jalankan program

```bash
./kasir_muthu
```

Hasil yang diharapkan:

```text
[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.
```

Kalau pesan ini muncul, berarti seluruh proses berhasil dijalankan.

### 4. Cek hasil di folder utama

```bash
ls
```

Hasil yang diharapkan:

* folder `brankas_kedai` sudah ada
* file `rahasia_muthu.zip` sudah ada

### 5. Cek isi folder hasil

```bash
ls brankas_kedai
```

Hasil yang diharapkan:

```text
buku_hutang.csv
daftar_penunggak.txt
```

### 6. Lihat isi daftar penunggak

```bash
cat brankas_kedai/daftar_penunggak.txt
```

Hasil yang diharapkan:

* file hanya berisi data pelanggan dengan status `Belum Lunas`

### 7. Cek isi file zip

```bash
unzip -l rahasia_muthu.zip
```

Hasil yang diharapkan:

* file zip berisi folder `brankas_kedai`
* di dalamnya ada `buku_hutang.csv` dan `daftar_penunggak.txt`

---

## E. Penjelasan Kode per Bagian

### 1. Header Library

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
```

Bagian ini memanggil library yang dibutuhkan:

* `stdio.h` untuk `printf`
* `stdlib.h` untuk `exit()`
* `unistd.h` untuk `fork()` dan `execlp()`
* `sys/wait.h` untuk `waitpid()`

### 2. Fungsi `gagal()`

```c
void gagal() {
    printf("[ERROR] Aiya! Proses gagal, file atau folder tidak ditemukan.\n");
    exit(EXIT_FAILURE);
}
```

Fungsi ini dipakai untuk menangani error. Kalau ada proses yang gagal, program langsung menampilkan pesan error lalu berhenti.

### 3. Fungsi `tunggu_anak(pid_t pid)`

```c
void tunggu_anak(pid_t pid) {
    int status;
    waitpid(pid, &status, 0);

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        gagal();
    }
}
```

Fungsi ini dipakai untuk:

1. menunggu child process selesai
2. mengecek apakah child berhasil atau gagal

Bagian ini yang membuat proses berjalan **urut**, bukan lompat-lompat.

### 4. Proses membuat folder

```c
pid = fork();
if (pid < 0) gagal();

if (pid == 0) {
    execlp("mkdir", "mkdir", "-p", "brankas_kedai", NULL);
    exit(EXIT_FAILURE);
}
tunggu_anak(pid);
```

Child process menjalankan command `mkdir`, lalu parent menunggu proses selesai.

### 5. Proses menyalin file CSV

```c
pid = fork();
if (pid < 0) gagal();

if (pid == 0) {
    execlp("cp", "cp", "buku_hutang.csv", "brankas_kedai/", NULL);
    exit(EXIT_FAILURE);
}
tunggu_anak(pid);
```

Bagian ini menyalin file asli ke folder hasil.

### 6. Proses memfilter data belum lunas

```c
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
```

Karena command ini memakai redirect `>`, maka dijalankan lewat `bash -c`. Hasilnya ditulis ke file `daftar_penunggak.txt`.

### 7. Proses membuat arsip zip

```c
pid = fork();
if (pid < 0) gagal();

if (pid == 0) {
    execlp("zip", "zip", "-r", "rahasia_muthu.zip", "brankas_kedai", NULL);
    exit(EXIT_FAILURE);
}
tunggu_anak(pid);
```

Bagian ini membuat file zip dari seluruh folder hasil.

---

## F. Hasil Akhir Soal 1

```text
soal_1/
├── buku_hutang.csv
├── kasir_muthu.c
├── rahasia_muthu.zip
└── brankas_kedai/
    ├── buku_hutang.csv
    └── daftar_penunggak.txt
```

---

# Reporting Soal 2

## A. Deskripsi Soal

Pada soal kedua, dibuat program daemon bernama `contract_daemon.c` yang berjalan di background untuk memantau file `contract.txt`.

Program ini punya beberapa tugas utama:

* menulis log setiap 5 detik
* memastikan file `contract.txt` selalu ada
* membuat ulang file jika dihapus
* mendeteksi perubahan isi file
* kalau isi file diubah, isi file harus **dibalikin lagi**
* menulis pesan penutup saat daemon dihentikan

Jadi program ini berfungsi seperti penjaga file yang terus aktif.

---

## B. File yang Digunakan

* `soal_2/contract_daemon.c`
* `soal_2/contract.txt`
* `soal_2/work.log`

---

## C. Kode Lengkap `contract_daemon.c`

```c
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
    tulis_log("contract violated.");
    buat_contract();
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
```

---

## D. Cara Menjalankan dan Hasil per Tahap

### 1. Masuk ke folder soal 2

```bash
cd soal_2
```

Hasil:

* terminal berada di folder `soal_2`

### 2. Compile program

```bash
gcc contract_daemon.c -o contract_daemon
```

Hasil yang diharapkan:

* file executable `contract_daemon` berhasil dibuat
* tidak ada error saat compile

### 3. Jalankan daemon

```bash
./contract_daemon &
```

Hasil yang diharapkan:

* program berjalan di background
* prompt terminal kembali muncul

### 4. Cek file awal yang dibuat

```bash
ls
```

Hasil yang diharapkan:

* `contract.txt` sudah ada
* `work.log` sudah ada

### 5. Lihat isi log awal

```bash
cat work.log
```

Hasil yang diharapkan:

* muncul status seperti:

```text
still working... [awake]
still working... [drifting]
still working... [numbness]
```

Ini menunjukkan daemon sedang aktif dan terus menulis log.

### 6. Uji hapus file contract

```bash
rm contract.txt
```

Lalu tunggu beberapa detik dan cek log:

```bash
cat work.log
```

Hasil yang diharapkan:

* muncul log `restored at: ...`

Artinya daemon mendeteksi file hilang lalu membuat ulang file tersebut.

### 7. Uji perubahan isi file

```bash
nano contract.txt
```

Ubah isi file, simpan, lalu cek isi file dan log:

```bash
cat contract.txt
cat work.log
```

Hasil yang diharapkan:

* isi file kembali ke versi asli
* muncul log `contract violated.`
* muncul log `restored at: ...`

Jadi di versi ini file bukan cuma terdeteksi berubah, tapi juga **dibalikin** ke kondisi semula.

### 8. Hentikan daemon

```bash
pkill contract_daemon
```

Lalu cek log lagi:

```bash
cat work.log
```

Hasil yang diharapkan:

* muncul log:

```text
We really weren't meant to be together.
```

---

## E. Penjelasan Kode per Bagian

### 1. Variabel global

```c
FILE *file_log;

char *status_list[] = {"awake", "drifting", "numbness"};
const char *isi_asli = "A promise to keep going, even when unseen.\n";
```

`status_list` berisi daftar status acak untuk log, sedangkan `isi_asli` menyimpan isi utama yang harus dipertahankan di `contract.txt`.

### 2. Fungsi `tulis_log()`

```c
void tulis_log(const char *msg) {
    file_log = fopen("work.log", "a");
    if (file_log == NULL) return;

    fprintf(file_log, "%s\n", msg);
    fclose(file_log);
}
```

Fungsi ini menambahkan log ke file `work.log`. Karena memakai mode `"a"`, isi lama tidak dihapus.

### 3. Fungsi `buat_contract()`

```c
void buat_contract() {
    FILE *f = fopen("contract.txt", "w");
    if (f == NULL) return;

    time_t now = time(NULL);
    fprintf(f, "%s", isi_asli);
    fprintf(f, "created at: %s", ctime(&now));

    fclose(f);
}
```

Fungsi ini membuat ulang file `contract.txt` dan mengisinya dengan isi awal plus timestamp.

### 4. Fungsi `log_restore_time()`

```c
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
```

Fungsi ini dipakai untuk menuliskan waktu restore ke log.

### 5. Fungsi `restore_contract_karena_diubah()`

```c
void restore_contract_karena_diubah() {
    tulis_log("contract violated.");
    buat_contract();
    log_restore_time();
}
```

Ini bagian penting dari perbaikan bug. Kalau isi file diubah, program:

1. menulis `contract violated.`
2. membalikkan isi file ke versi asli
3. menulis waktu restore

### 6. Fungsi `handler(int sig)`

```c
void handler(int sig) {
    (void)sig;
    tulis_log("We really weren't meant to be together.");
    exit(0);
}
```

Fungsi ini dipanggil saat daemon dihentikan. Sebelum keluar, program menulis pesan terakhir ke log.

### 7. Membuat daemon

```c
pid_t pid = fork();

if (pid < 0) {
    printf("[ERROR] Gagal membuat daemon.\n");
    exit(EXIT_FAILURE);
}

if (pid > 0) {
    exit(EXIT_SUCCESS);
}
```

Bagian ini membuat process child. Parent keluar, child lanjut berjalan sebagai daemon.

### 8. Memasang signal handler

```c
signal(SIGTERM, handler);
signal(SIGINT, handler);
```

Bagian ini membuat daemon bisa menangani penghentian dengan rapi.

### 9. Loop utama

```c
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
```

Setiap 5 detik program akan:

1. memilih status acak
2. menulis log
3. mengecek apakah file masih ada
4. membuat ulang kalau hilang
5. mendeteksi perubahan isi
6. mengembalikan isi file kalau diubah

---

## F. Hasil Akhir Soal 2

```text
soal_2/
├── contract_daemon.c
├── contract.txt
└── work.log
```

---

# Reporting Soal 3

## A. Deskripsi Soal

Pada soal ketiga, dibuat program daemon `angel.c` yang berjalan dengan nama proses `maya`.

Program ini punya beberapa fitur:

* membuat `LoveLetter.txt` setiap 10 detik
* memilih isi file secara acak
* mengubah isi file ke base64
* menyediakan mode `-decrypt`
* menyediakan mode `-kill`
* mencatat semua aktivitas ke `ethereal.log`
* menampilkan pesan error yang jelas saat terjadi kondisi tertentu

Karena daemon terus menulis ulang file, maka alur `-decrypt` harus dijalankan dengan benar, yaitu **daemon dimatikan dulu**, baru file didecrypt.

---

## B. File yang Digunakan

* `soal_3/angel.c`
* `soal_3/LoveLetter.txt`
* `soal_3/ethereal.log`

---

## C. Kode Lengkap `angel.c`

```c
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
```

---

## D. Cara Menjalankan dan Hasil per Tahap

### 1. Masuk ke folder soal 3

```bash
cd soal_3
```

Hasil:

* terminal berada di folder `soal_3`

### 2. Compile program

```bash
gcc angel.c -o angel
```

Hasil yang diharapkan:

* file executable `angel` berhasil dibuat
* tidak ada error saat compile

### 3. Jalankan daemon

```bash
./angel -daemon
```

Hasil yang diharapkan:

```text
[INFO] Daemon berhasil dijalankan.
```

### 4. Cek nama proses

```bash
ps aux | grep maya
```

Hasil yang diharapkan:

* muncul proses dengan nama `maya`

Ini membuktikan rename process berhasil.

### 5. Cek file hasil

```bash
ls
```

Hasil yang diharapkan:

* `LoveLetter.txt` sudah ada
* `ethereal.log` sudah ada

### 6. Lihat isi file sebelum decrypt

```bash
cat LoveLetter.txt
```

Hasil yang diharapkan:

* isi file masih berupa base64

Artinya encode berhasil dijalankan.

### 7. Uji decrypt saat daemon masih aktif

```bash
./angel -decrypt
```

Hasil yang diharapkan:

```text
[ERROR] Daemon masih berjalan. Matikan dulu dengan ./angel -kill sebelum decrypt.
```

Ini memang disengaja supaya file tidak langsung ditimpa lagi oleh daemon.

### 8. Hentikan daemon

```bash
./angel -kill
```

Hasil yang diharapkan:

```text
[INFO] Daemon berhasil dihentikan.
```

### 9. Jalankan decrypt setelah daemon mati

```bash
./angel -decrypt
```

Hasil yang diharapkan:

```text
[INFO] File berhasil didecrypt.
```

Lalu cek isi file:

```bash
cat LoveLetter.txt
```

Hasil yang diharapkan:

* isi file berubah menjadi kalimat normal

### 10. Cek log

```bash
cat ethereal.log
```

Hasil yang diharapkan:

* muncul log seperti:

```text
[04:23:16][maya][RUNNING]
[04:23:19][maya][SUCCESS]
```

---

## E. Penjelasan Kode per Bagian

### 1. Array `kata[]`

```c
char *kata[] = {
    "aku akan fokus pada diriku sendiri",
    "aku mencintaimu dari sekarang hingga selamanya",
    "aku akan menjauh darimu, hingga takdir mempertemukan kita",
    "kalau aku dilahirkan kembali, aku tetap akan menyayangimu"
};
```

Array ini berisi daftar kalimat yang nanti dipilih secara acak untuk ditulis ke `LoveLetter.txt`.

### 2. Fungsi `log_event()`

```c
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
```

Fungsi ini dipakai untuk mencatat aktivitas program ke file log dalam format waktu, nama proses, dan status.

### 3. Fungsi `buat_file()`

```c
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
```

Fungsi ini membuat file `LoveLetter.txt` lalu mengisinya dengan satu kalimat acak.

### 4. Fungsi `encode_base64()`

```c
void encode_base64() {
    int hasil = system("base64 LoveLetter.txt > temp.txt && mv temp.txt LoveLetter.txt");
    if (hasil != 0) {
        log_event("ERROR");
        printf("[ERROR] Gagal melakukan encode file.\n");
    }
}
```

Fungsi ini mengubah isi file ke bentuk base64. Kalau gagal, program menulis log sekaligus menampilkan pesan error.

### 5. Fungsi `jalan_maya()`

```c
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
```

Ini adalah inti daemon. Di sini program:

1. menyimpan PID
2. membuat file
3. encode file
4. menulis log
5. menunggu 10 detik
6. mengulang lagi

### 6. Mode `-daemon`

```c
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
```

Mode ini dipakai untuk memulai daemon. Hal paling penting di sini adalah command:

```bash
exec -a maya ./angel -worker
```

Bagian ini yang membuat nama proses berubah menjadi `maya`.

### 7. Mode `-worker`

```c
else if (strcmp(argv[1], "-worker") == 0) {
    jalan_maya();
}
```

Mode ini dipakai secara internal untuk menjalankan loop utama daemon.

### 8. Mode `-decrypt`

```c
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
```

Mode ini dipakai untuk mengembalikan isi file dari base64 ke teks biasa. Di versi ini, decrypt hanya boleh dilakukan kalau daemon sudah dimatikan.

### 9. Mode `-kill`

```c
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
```

Mode ini membaca PID daemon dari file, lalu menghentikan daemon dengan aman.

---

## F. Hasil Akhir Soal 3

```text
soal_3/
├── angel.c
├── LoveLetter.txt
└── ethereal.log
```

---

# Kesimpulan

Dari tiga soal ini, saya jadi lebih paham bahwa process di Linux bukan cuma soal menjalankan program, tapi juga soal bagaimana process dibuat, dikendalikan, disinkronkan, dan dipakai untuk menyelesaikan tugas tertentu.

* **Soal 1** lebih fokus ke urutan eksekusi process
* **Soal 2** lebih fokus ke daemon dan pengawasan file
* **Soal 3** menggabungkan daemon, penamaan process, encode/decode, dan logging

Secara keseluruhan, semua program sudah berhasil dijalankan sesuai kebutuhan soal dan menghasilkan output yang diminta.

```
```
