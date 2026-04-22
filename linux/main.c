#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// recursive mkdir (linux only)
int rmkdir(const char *dir) {
    char buf[256];
    char buf2[256];
    char *token;
    char *ptr;
    int count = 0;

    strncpy(buf, dir, sizeof(buf));
    buf[sizeof(buf)-1] = 0;

    ptr = buf;
    while ((token = strsep(&ptr, "/")) != NULL) {
        if (*token != '\0')
            count++;
    }

    if (count <= 1)
        return 0; // nothing to create

    strncpy(buf2, dir, sizeof(buf2));
    buf2[sizeof(buf2)-1] = 0;

    char path[256] = "";
    ptr = buf2;
    int idx = 0;

    while ((token = strsep(&ptr, "/")) != NULL) {
        if (*token == '\0')
            continue;

        // stop before last element
        if (idx == count - 1)
            break;

        strcat(path, "/");
        strcat(path, token);

        printf("mkdir: %s\n", path);
        mkdir(path, 0755);

        idx++;
    }

    return 0;
}

int install() {
    FILE *fp;

#ifdef _WIN32
    printf("This system is not supported by this installer\n");
    return 1;
#else
    fp = popen("pwsh -noprofile -command 'echo $PROFILE'", "r");
#endif

    if (!fp) {
        printf("Could not find $PROFILE, aborting\n");
        return 1;
    }

    char profile[256];
    if (!fgets(profile, sizeof(profile), fp)) {
        pclose(fp);
        printf("Could not read $PROFILE, aborting\n");
        return 1;
    }
    pclose(fp);

    // Remove newline
    profile[strcspn(profile, "\r\n")] = 0;

    printf("Detected PowerShell profile: %s\n", profile);

    FILE *ex = fopen(profile, "r");
    if (ex) {
        printf("Powershell profile file found @ %s\n", profile);
        fclose(ex);
    } else {
        printf("No powershell profile file found.\n");
    }

    // Try to open for writing
    FILE *pf = fopen(profile, "w");
    if (!pf) {
        printf("Could not open $PROFILE, creating directories...\n");
        rmkdir(profile);
        pf = fopen(profile, "w");
        if (!pf) {
            printf("Could not create directories or open $PROFILE, aborting\n");
            return 1;
        }
    }

    // Nerd Font TUI
    printf("\033[2J");   // clear screen
    printf("\033[H");    // move cursor home

    printf("PowerShellLevel10K Installer\n");
    printf("----------------------------\n\n");

    printf("Initiating Nerd Font support check...\n\n");
    printf("Does this look like a lock?\n\n");
    printf("   \n\n");
    printf("[y] Yes, it looks like a lock\n");
    printf("[n] No, it doesn't look like a lock\n");
    printf("[q] Quit installer and don't save changes\n\n");
    printf("Please provide a choice [ynq]: ");

    char buffer[16];
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        printf("\nError reading input, aborting\n");
        fclose(pf);
        return 1;
    }

    buffer[strcspn(buffer, "\r\n")] = 0; // strip newline

    if (buffer[0] == 'q') {
        printf("Quitting without writing to %s\n", profile);
        fclose(pf);
        return 0;
    } else if (buffer[0] == 'n') {
        printf("Warning: Nerd Font not detected. Prompt may look broken.\n");
    } else if (buffer[0] == 'y') {
        printf("Nerd Font confirmed.\n");
    } else {
        printf("Unknown choice '%s', continuing anyway.\n", buffer);
    }

    fprintf(pf, "\nfunction prompt { \"Hi > \" }\n");
    printf("Writing data to %s...\n", profile);
    fclose(pf);

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Initiating PowerShellLevel10K...\n");
        return install();
    } else if (argc == 2) {
        if (strcmp(argv[1], "configure") == 0) {
            printf("onfiguring\n");
        } else {
            printf("Unknown argument %s\n", argv[1]);
        }
    } else {
        printf("Unknown command\n");
    }

    return 0;
}
