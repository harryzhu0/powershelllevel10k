#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <locale.h>
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

// recursive make directory
int rmkdir(const char *dir) {
    char tmp[256];
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

        int i = 0;

        for (i = 0; i < count; i++) {
            if (i == highlight) {
                mvprintw(8 + i, 2, "> %s", items[i]);
            } else {
                mvprintw(8 + i, 2, "  %s", items[i]);
            }
        }

        mvprintw(8 + i, sizeof(items[i]) + 2, "\n  ");

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

int menu_select_c(const char *title, const char *subtitle,
                const char *items[], int count, int oline) {
    int highlight = 0;
    int ch;

    while (1) {
        clear();
        mvprintw(1, 2, "%s", title);
        mvprintw(2, 2, "----------------------------------------");
        mvprintw(4, 2, "%s", subtitle);

        int i = 0;

        for (i = 0; i < count; i++) {
            if (i == highlight) {
                mvprintw(oline + i, 2, "> %s", items[i]);
            } else {
                mvprintw(oline + i, 2, "  %s", items[i]);
            }
        }

        mvprintw(oline + i, sizeof(items[i]) + 2, "\n  ");

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

// Quit function because why not

int quit() {
    printf("Configuration aborted, no changes written to Powershell profile.\n");
    printf("Run \033[36mpwsh10k\033[0m to run the installer again\n");
    return 0;
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
        "PowerShell10K Installer",
        "Does this look like a diamond?\n\n                --> ◆ <--",
        step1_items, 3
    );

    if (s1 == 2) return quit();

    if (s1 == 1) {
        /* Install Nerd Font */
        const char *step2_items[] = {
            "Yes, install Nerd Font",
            "No, skip installation"
        };

        int s2 = menu_select_c(
            "PowerShell10K Installer",
            "Install Nerd Font?",
            step2_items, 2, 6
        );

        if (s2 == 0) {
            const char *inst_items[] = {
                "JetBrainsMono",
                "FiraCode",
                "Hack",
                "Meslo",
                "GeistMono",
                "Noto",
                "ZedMono"
            };

            int i1 = menu_select_c(
                "PowerShell10K Installer",
                "Please choose a font",
                inst_items, 7, 6
            );

            installnf(inst_items[i1]);
        } else {
            const char *warn_items[] = { "Yes", "No" };

            int i2 = menu_select_c(
                "PowerShell10K installer",
                "Warning: PowerShellLevel10K may not work well without nerd fonts.\nContinue installation?",
                warn_items, 2, 7
            );

            if (i2 == 2) {
                return quit();
            }
        }
    }

    const char *step3_items[] = {
        "Yes, it looks like a lock",
        "No, it does not look like a lock",
        "Restart",
        "Abort"
    };

    int s3 = menu_select(
        "PowerShell10K Installer",
        "Does this look like a lock?\n\n                -->  <--",
        step3_items, 4
    );


    if (s3 == 3) return quit();
    if (s3 == 2) return install_wizard();

    clear();
    mvprintw(2, 2, "Writing configuration to PowerShell profile...");
   
   
    const char *ps1         = "";
    const char *bg = "\x1b[48;5;8m";
    const char *fg = "\x1b[38;5;8m";
    const char *hi = "\x1b[96m";
    const char *grey = "\x1b[90m";
    const char *reset = "\x1b[0m";

    fprintf(pf,
    "function prompt {\n"
    "    $p = (Get-Location).Path.Replace($HOME,'~')\n"
    "    $parts = $p -split '[\\\\/]' | Where-Object { $_ -ne '' }\n"
    "\n"
    "    if ($p.Length -lt 30) {\n"
    "        $colored = for ($i=0; $i -lt $parts.Count; $i++) {\n"
    "            $seg = $parts[$i]\n"
    "            if ($i -eq 0 -or $i -eq $parts.Count - 1) {\n"
    "                \"%s%s$seg%s\"\n"   // bg + highlight + reset
    "            } else {\n"
    "                \"%s$seg%s\"\n"     // bg + reset
    "            }\n"
    "        }\n"
    "        $pathString = $colored -join '%s/%s'\n" // bg + reset
    "        return \"%s $pathString%s %s%s%s%s \"\n"   // glyph color + glyph + reset
    "    }\n"
    "\n"
    "    $colored = for ($i=0; $i -lt $parts.Count; $i++) {\n"
    "        $seg = $parts[$i]\n"
    "\n"
    "        if ($i -eq 0) {\n"
    "            \"%s%s$seg%s\"\n"   // bg + highlight + reset
    "        }\n"
    "        elseif ($i -eq $parts.Count - 1) {\n"
    "            \"%s%s$seg%s\"\n"   // bg + highlight + reset
    "        }\n"
    "        elseif ($i -eq 1 -and $parts[0] -eq '~') {\n"
    "            \"%s%s$($seg.Substring(0,[Math]::Min(3,$seg.Length)))%s\"\n" // bg + grey + reset
    "        }\n"
    "        else {\n"
    "            \"%s%s$($seg[0])%s\"\n" // bg + grey + reset
    "        }\n"
    "    }\n"
    "\n"
    "    $pathString = $colored -join '%s/%s'\n" // bg + reset
    "    return \"%s $pathString%s %s%s%s%s \"\n"   // glyph color + glyph + reset
    "}\n"
    ,
    
    bg, hi, reset,
    bg, reset,
    bg, reset,
    bg, bg, reset, fg, ps1, reset,

    bg, hi, reset,
    bg, hi, reset,
    bg, grey, reset,
    bg, grey, reset,

    bg, reset,
    bg, bg, reset, fg, ps1, reset
    );


    fclose(pf);

    mvprintw(4, 2, "Done!");
    mvprintw(6, 2, "Press any key to exit.");
    getch();

    return 0;
}

int main() {
    setlocale(LC_ALL, "");

    initscr();
    noecho();
    keypad(stdscr, TRUE);

    install_wizard();

    endwin();
    return 0;
}
