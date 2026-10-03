import json
import os
import urllib.parse

import certifi
import yt_dlp

os.environ.setdefault("SSL_CERT_FILE", certifi.where())


def _base_opts():
    return {"quiet": True, "no_warnings": True}


def search(query, max_results):
    url = "https://music.youtube.com/search?q=" + urllib.parse.quote(query)
    try:
        with yt_dlp.YoutubeDL(
            {**_base_opts(), "skip_download": True, "extract_flat": True}
        ) as ydl:
            info = ydl.extract_info(url, download=False)
    except Exception as e:
        return json.dumps({"error": str(e)})
    results = []
    for entry in (info or {}).get("entries") or []:
        title = (entry.get("title") or "").strip()
        link = entry.get("webpage_url") or entry.get("url") or ""
        if not title or title == "NA":
            continue
        if "watch?v=" not in link:
            continue
        results.append({"title": title, "url": link})
        if len(results) >= max_results:
            break
    return json.dumps(results, ensure_ascii=False)


def download(url, out_dir):
    outtmpl = os.path.join(out_dir, "%(title)s.%(ext)s")
    try:
        with yt_dlp.YoutubeDL(
            {**_base_opts(), "format": "bestaudio", "noplaylist": True,
             "outtmpl": outtmpl}
        ) as ydl:
            info = ydl.extract_info(url, download=True)
    except Exception as e:
        return "ERROR:" + str(e)
    path = (info or {}).get("filepath")
    if not path or not os.path.isfile(path):
        path = _newest_file(out_dir)
    if not path or not os.path.isfile(path):
        return "ERROR:no se encontro el archivo descargado"
    return path


def _newest_file(out_dir):
    best = None
    try:
        for name in os.listdir(out_dir):
            full = os.path.join(out_dir, name)
            if os.path.isfile(full) and (
                best is None or os.path.getmtime(full) > os.path.getmtime(best)
            ):
                best = full
    except OSError:
        pass
    return best
