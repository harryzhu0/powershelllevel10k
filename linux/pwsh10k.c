#include "pwsh10k.h"

void write_pwsh10k_prompt(FILE *pf) {
    const char *ps1   = "";
    const char *bg    = "\x1b[48;5;8m";
    const char *fg    = "\x1b[38;5;8m";
    const char *hi    = "\x1b[96m";
    const char *grey  = "\x1b[90m";
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
    "                \"%s%s$seg%s\"\n"
    "            } else {\n"
    "                \"%s$seg%s\"\n"
    "            }\n"
    "        }\n"
    "        $pathString = $colored -join '%s/%s'\n"
    "        return \"%s $pathString%s %s%s%s%s \"\n"
    "    }\n"
    "\n"
    "    $colored = for ($i=0; $i -lt $parts.Count; $i++) {\n"
    "        $seg = $parts[$i]\n"
    "\n"
    "        if ($i -eq 0) {\n"
    "            \"%s%s$seg%s\"\n"
    "        }\n"
    "        elseif ($i -eq $parts.Count - 1) {\n"
    "            \"%s%s$seg%s\"\n"
    "        }\n"
    "        elseif ($i -eq 1 -and $parts[0] -eq '~') {\n"
    "            \"%s%s$($seg.Substring(0,[Math]::Min(3,$seg.Length)))%s\"\n"
    "        }\n"
    "        else {\n"
    "            \"%s%s$($seg[0])%s\"\n"
    "        }\n"
    "    }\n"
    "\n"
    "    $pathString = $colored -join '%s/%s'\n"
    "    return \"%s $pathString%s %s%s%s%s \"\n"
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
}