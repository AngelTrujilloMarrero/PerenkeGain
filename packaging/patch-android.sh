#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
python3 - "$@" <<'EOF'
import re

PROJ = "android/build.gradle"
APP = "android/app/build.gradle"


def sub_once(path, pattern, repl):
    with open(path) as f:
        text = f.read()
    text2, n = re.subn(pattern, repl, text, count=1, flags=re.M)
    if n == 0:
        print("ya aplicado/sin coincidencia")
        return
    with open(path, "w") as f:
        f.write(text2)
    print("aplicado en %s" % path)


def ensure(path, needle, pattern, repl):
    with open(path) as f:
        if needle in f.read():
            print("ya aplicado: %s" % needle[:50])
            return
    sub_once(path, pattern, repl)


ensure(PROJ, "com.chaquo.python:gradle",
       r"(classpath 'com\.android\.tools\.build:gradle:[^']*')",
       r"\1\n        classpath 'com.chaquo.python:gradle:17.0.0'")

ensure(APP, "apply plugin: 'com.chaquo.python'",
       r"^(apply plugin: 'com\.android\.application')",
       r"\1\napply plugin: 'com.chaquo.python'")

ensure(APP, '"../../android-java"',
       r'(\.\./\.\./JUCE/modules/juce_gui_basics/native/javaopt/app")',
       r'\1,\n             "../../android-java"')

ensure(APP, 'version "3.12"',
       r"^(        targetSdkVersion 35)$",
       "        ndk {\n            abiFilters \"arm64-v8a\"\n        }\n"
       "        python {\n            version \"3.12\"\n"
       "            pip {\n                install \"yt-dlp\"\n"
       "                install \"certifi\"\n            }\n        }\n\\1")

ensure(APP, "ffmpeg-kit-audio-lgpl",
       r"(    dependencies \{\n)(    \})",
       r"\1        implementation "
       "'com.arthenica:ffmpeg-kit-audio-lgpl:6.0'\n\2")

sub_once(APP,
         r'abiFilters "armeabi-v7a", "x86", "arm64-v8a", "x86_64"',
         'abiFilters "arm64-v8a"')

with open(APP) as f:
    app = f.read()
if "release_ {\n            ndk" not in app:
    sub_once(APP, r"((?:^|\n)        release_ \{\n)",
             r"\1            ndk {\n"
             r"                abiFilters \"arm64-v8a\"\n"
             r"            }\n")
print("parche android completo")
EOF
