package com.perenkegain;

import android.content.Context;
import com.arthenica.ffmpegkit.FFmpegKit;
import com.arthenica.ffmpegkit.FFmpegSession;
import com.arthenica.ffmpegkit.FFmpegSessionCompleteCallback;
import com.arthenica.ffmpegkit.ReturnCode;
import com.chaquo.python.Python;
import com.chaquo.python.android.AndroidPlatform;
import java.io.File;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

public class YouTubeBridge {
    private static Python python(Context ctx) {
        synchronized (YouTubeBridge.class) {
            if (!Python.isStarted()) {
                Python.start(new AndroidPlatform(ctx));
            }
        }
        return Python.getInstance();
    }

    public static String search(Context ctx, String query, int max) {
        try {
            Object r = python(ctx).getModule("ytbridge")
                    .callAttr("search", query, max);
            return r.toString();
        } catch (Exception e) {
            return "{\"error\": \"" + esc(e.getMessage()) + "\"}";
        }
    }

    public static String download(Context ctx, String url, String outDir) {
        try {
            Object r = python(ctx).getModule("ytbridge")
                    .callAttr("download", url, outDir);
            final String input = r.toString();
            if (input.startsWith("ERROR:")) {
                return input;
            }
            final String out = input.replaceAll("\\.[^.]+$", "") + ".mp3";
            final CountDownLatch latch = new CountDownLatch(1);
            final int[] rc = new int[]{-1};
            FFmpegKit.executeAsync(
                    "-y -loglevel error -i " + sh(input)
                            + " -vn -c:a libmp3lame -q:a 0 " + sh(out),
                    new FFmpegSessionCompleteCallback() {
                        @Override
                        public void apply(FFmpegSession session) {
                            ReturnCode code = session.getReturnCode();
                            rc[0] = code != null ? code.getValue() : -1;
                            latch.countDown();
                        }
                    });
            latch.await(240, TimeUnit.SECONDS);
            if (rc[0] == 0 && new File(out).exists()) {
                return out;
            }
            return "ERROR:conversion a MP3 fallida (codigo " + rc[0] + ")";
        } catch (Exception e) {
            return "ERROR:" + e.getMessage();
        }
    }

    private static String sh(String s) {
        return "'" + s.replace("'", "'\\''") + "'";
    }

    private static String esc(String s) {
        if (s == null) {
            return "";
        }
        return s.replace("\\", "\\\\").replace("\"", "\\\"")
                .replace("\n", " ").replace("\r", " ");
    }
}
