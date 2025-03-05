#include <stdio.h>      // Стандарт оролт/гаралт үйлдлүүд
#include <stdlib.h>     // Санах ой хуваарилах, процесс удирдах, бусад стандарт функцууд
#include <string.h>     // Тэмдэгт мөр боловсруулах функцууд
#include <dirent.h>     // Сан дахь файлуудыг хянах функцууд
#include <unistd.h>     // POSIX үйлдлийн системийн API (файл, сан удирдах)
#include <sys/stat.h>   // Файл-ын хэмжээг нь олохын тулд ашиглаж 
#include <windows.h>    // Windows үйлдлийн системийн API (процесс удирдах)

#define MAX 256 // Хэрэглэгчийн коммандын хамгийн их уртыг тодорхойлно

void run_process(char *filename) {

    STARTUPINFO si; // Програмын ажиллуулах мэдээллийг хадгалах хувьсагч
    PROCESS_INFORMATION pi; // Програмын мэдээллийг хадгалах хувьсагч
    ZeroMemory(&si, sizeof(si)); // Програмын ажиллуулах мэдээллийг цэвэрлэх
    si.cb = sizeof(si); // Програмын ажиллуулах мэдээллийн хэмжээг тохируулах

    if (!CreateProcess (NULL, filename, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) { // Програмыг ажиллуулах
        DWORD error = GetLastError(); // Алдааны кодыг авах
        printf("CreateProcess failed (%lu).\n", error); // Алдаа гарч ирсэн тохиолдолд алдаа хэвлэнэ
        return;
    }
    WaitForSingleObject(pi.hProcess, INFINITE); // Програмыг дуусахад хүлээж авах

    DWORD exitCode; // Програмын гаралтын кодыг хадгалах хувьсагч
    if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
        printf("'%s' nertei program amjilttai ajilluullaa.\n", filename);
        printf("Program-iin garalt: %lu\n", exitCode);
    } else {
        printf("Process-iin garaltiin code oloh amjilttai boloogui!\n");
    }
    CloseHandle(pi.hProcess); // Програмын мэдээллийг хаана
    CloseHandle(pi.hThread); // Програмын мэдээллийг хаана
}

void File_Jagsaalt() {
    struct dirent *entry; // Сан дахь файлуудын мэдээллийг хадгалах хувьсагч
    DIR *dr = opendir("."); // Одоогийн санг онгойлгоно ("." нь одоогийн санг илэрхийлнэ)
    if (dr == NULL) { // Хэрэв онгойлгож чадахгүй бол алдаа хэвлэнэ
        perror("San dah file-uudig ongoilgoj cadsangui.");
        return;
    }
    printf("%-30s %-10s\n", "Ner", "Hemjee (byte-aar)");
    while ((entry = readdir(dr))) { // Сан дахь бүх файлуудыг жагсааж хэвлэнэ
        struct stat fileStat; // Файлын мэдээллийг авах
        if (stat(entry->d_name, &fileStat) == 0) {
            printf("%-30s %-10ld\n", entry->d_name, fileStat.st_size);
        } 
    }
    closedir(dr);
}
// Шинэ файл үүсгэх функц
void File_Uusgeh(char *filename) {
    FILE *file = fopen(filename, "w"); // "w" горимд файл үүсгэнэ (байхгүй бол шинээр үүсгэнэ)
    if (file == NULL) {
        perror("File uusgej cadsangui."); // Хэрэв онгойлгож чадахгүй бол алдаа хэвлэнэ
        return;
    }
    fclose(file); // Файлыг хаана
    printf("'%s' nertei file amjilttai uuslee.\n", filename);
}

// Файлд өгөгдөл бичих функц
void filed_nemeh(char *filename, char *data) {
    FILE *file = fopen(filename, "a"); // "a" горимд файл нээж, өгөгдөл нэмэх боломжтой
    if (file == NULL) {
        perror("File ongoilgoj cadsangui."); // Файл нээх боломжгүй бол алдаа хэвлэнэ
        return;
    }
    fprintf(file, "%s\n", data); // Өгөгдлийг файлд бичээд хаана
    fclose(file);
    printf("'%s' file-d amjilttai ugugdul nemlee.\n", filename);
}

// Файлын нэр өөрчлөх функц
void ner_solih(char *oldname, char *newname) {
    if (rename(oldname, newname) != 0) { // Файлын нэрийг өөрчилнө, амжилтгүй бол алдаа хэвлэнэ
        perror("File-iin neriig solij cadsangui.");
        return;
    }
    printf("'%s' nertei file amjilttai '%s' nertei bollo.\n", oldname, newname);
}

// Файл устгах функц
void ustgah(char *filename) {
    if (remove(filename) != 0) { // Файлыг устгана, амжилтгүй бол алдаа хэвлэнэ
        perror("File-iig ustgaj cadsangui.");
        return;
    }
    printf(" '%s' nertei file amjilttai ustlaa.\n", filename);
}

// Одоогийн ажлын санг өөрчлөх функц
void directory_solih(char *path) {
    if (chdir(path) != 0) { // Санг өөрчилнө, амжилтгүй бол алдаа хэвлэнэ
        perror("directory solij cadsangui.");
        return;
    }
    if (strcmp(path, "..") == 0) {
        printf("Amjilttai huucin directoryruu shiljlee.\n");
    } else {
        printf("Amjilttai '%s' directoryruu shiljlee.\n", path);
    }
}

int main() {
    char command[MAX]; // Хэрэглэгчийн оруулах коммандыг хадгалах хувьсагч

    while (1) { // Коммандын гүйцэтгэлийг тасралтгүй хийх давталт
        printf(">>> ");
        fgets(command, MAX, stdin); // Хэрэглэгчийн коммандыг уншина
        command[strcspn(command, "\n")] = 0; // Шинэ мөрийн тэмдэгтийг устгана

        char *cmd = strtok(command, " "); // Коммандын эхний үгийг ялгаж авна
        if (cmd == NULL) continue;

        if (strcmp(cmd, "dir") == 0) { // 'dir' комманд бол файлуудыг жагсаана
            File_Jagsaalt();
        } else if (strcmp(cmd, "create") == 0) { // 'create' комманд бол файл үүсгэнэ
            char *filename = strtok(NULL, " ");
            if (filename) File_Uusgeh(filename);
            else printf("Usage: create <filename>\n");
        } else if (strcmp(cmd, "input") == 0) { // 'input' комманд бол файлд өгөгдөл бичнэ
            char *filename = strtok(NULL, " ");
            char *data = strtok(NULL, "");
            if (filename && data) filed_nemeh(filename, data);
            else printf("Usage: input <filename> <data>\n");
        } else if (strcmp(cmd, "rename") == 0) { // 'rename' комманд бол файлын нэр өөрчилнө
            char *oldname = strtok(NULL, " ");
            char *newname = strtok(NULL, " ");
            if (oldname && newname) ner_solih(oldname, newname);
            else printf("Usage: rename <oldname> <newname>\n");
        } else if (strcmp(cmd, "del") == 0) { // 'del' комманд бол файл устгана
            char *filename = strtok(NULL, " ");
            if (filename) ustgah(filename);
            else printf("Usage: del <filename>\n");
        } else if (strcmp(cmd, "cd") == 0) { // 'cd' комманд бол сан өөрчилнө
            char *path = strtok(NULL, " ");
            if (path) directory_solih(path);
            else printf("Usage: cd <directory>\n");
        } else if (strcmp(cmd, "exit") == 0) {
            break;
        } else if( strcmp(cmd, "run") == 0) {
            char *filename = strtok(NULL, " ");
            if (filename) run_process(filename);
            else printf("Usage: run <filename>\n");
        } else if (strcmp(cmd, "help") == 0) {
            printf("dir       File iin directory harah\n");
            printf("create    Shine file uusgeh, ashiglahdaa (create) (file-iin ner)\n");
            printf("input     File ruu bicwer nemeh, ashiglahdaa (input) (file-iin ner) (nemeh bicwer)\n");
            printf("rename    File-iin ner solih, ashiglahdaa (rename) (huucin ner) (solih ner)\n");
            printf("del       File ustgah, ashiglahdaa(del) (file-iin ner)\n");
            printf("cd        Odoo baigaa directory solih, ashiglahdaa (cd) (solij oroh folder) hervee umnuh folderluuga ocih bol \"..\" ashiglana.\n");
            printf("exit      Programmaas garah\n");
            printf("run       exe ajilluulahdaa (run) (file-iin ner)\n");
            printf("help      Programmii hamaaral\n");
        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }

    return 0;
}