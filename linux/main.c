#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int installnf(const char *font) {
    char font_url[512];
    snprintf(font_url, sizeof(font_url),
             "https://github.com/ryanoasis/nerd-fonts/releases/latest/download/%s.zip",
             font);

    const char *home = getenv("HOME");
    if (!home) return 1;

    char font_dir[512];
    snprintf(font_dir, sizeof(font_dir), "%s/.local/share/fonts", home);
    mkdir(font_dir, 0755);

    char zip_path[256];
    snprintf(zip_path, sizeof(zip_path), "/tmp/%s.zip", font);

    char curl_cmd[1024];
    snprintf(curl_cmd, sizeof(curl_cmd),
             "curl -L -o \"%s\" \"%s\"", zip_path, font_url);

    if (system(curl_cmd) != 0) return 1;

    char unzip_cmd[1024];
    snprintf(unzip_cmd, sizeof(unzip_cmd),
             "unzip -o \"%s\" -d \"%s\"", zip_path, font_dir);

    if (system(unzip_cmd) != 0) return 1;

    system("fc-cache -f");
    return 0;
}

int rmkdir(const char *dir) {
    char tmp[256];
    char path[256] = "";
    char *p = NULL;

    snprintf(tmp, sizeof(tmp), "%s", dir);

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    return mkdir(tmp, 0755);
}

int menu_select(const char *title, const char *subtitle,
                const char *items[], int count) {
    int highlight = 0;
    int ch;

    while (1) {
        clear();
        mvprintw(1, 2, "%s", title);
        mvprintw(2, 2, "----------------------------------------");
        mvprintw(4, 2, "%s", subtitle);

        for (int i = 0; i < count; i++) {
            if (i == highlight) attron(A_REVERSE);
            mvprintw(7 + i, 4, "%s", items[i]);
            if (i == highlight) attroff(A_REVERSE);
        }

        ch = getch();
        switch (ch) {
            case KEY_UP:
                highlight = (highlight == 0) ? count - 1 : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == count - 1) ? 0 : highlight + 1;
                break;
            case '\n':
                return highlight;
        }
    }
}

// Install wizard
int install_wizard() {
    FILE *fp;

    fp = popen("pwsh -noprofile -command 'echo $PROFILE'", "r");
    if (!fp) return 1;

    char profile[256];
    if (!fgets(profile, sizeof(profile), fp)) {
        pclose(fp);
        return 1;
    }
    pclose(fp);

    profile[strcspn(profile, "\r\n")] = 0;

    FILE *pf = fopen(profile, "a");
    if (!pf) {
        rmkdir(profile);
        pf = fopen(profile, "a");
        if (!pf) return 1;
    }

    const char *step1_items[] = {
        "Yes, it looks like a diamond",
        "No, it does not look like a diamond",
        "Abort"
    };

    int s1 = menu_select(
        "[ Step 1 / 3 ] Nerd Font Detection",
        "Does this look like a diamond?\n\n                    ◆",
        step1_items, 3
    );

    if (s1 == 2) return 0;

    if (s1 == 1) {
        /* Install Nerd Font */
        const char *step2_items[] = {
            "Yes, install Nerd Font",
            "No, skip installation"
        };

        int s2 = menu_select(
            "[ Step 2 / 3 ] Nerd Font Installation",
            "Install Nerd Font?",
            step2_items, 2
        );

        if (s2 == 0) installnf("Jet");
    }

    const char *step3_items[] = {
        "Yes, it looks like a lock",
        "No, it does not look like a lock",
        "Restart",
        "Abort"
    };

    int s3 = menu_select(
        "[ Step 3 / 3 ] Lock Glyph Test",
        "Does this look like a lock?\n\n                    ",
        step3_items, 4
    );

    if (s3 == 3) return 0;
    if (s3 == 2) return install_wizard();

    clear();
    mvprintw(2, 2, "Writing configuration to PowerShell profile...");
    fprintf(pf, "\nfunction prompt { \"Hi > \" }\n");
    fclose(pf);

    mvprintw(4, 2, "Done!");
    mvprintw(6, 2, "Press any key to exit.");
    getch();

    return 0;
}

int main() {
    initscr();
    noecho();
    keypad(stdscr, TRUE);

    install_wizard();

    endwin();
    return 0;
}
