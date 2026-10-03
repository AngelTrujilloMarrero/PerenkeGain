#include "storage/YouTubeAndroid.h"
#include "types/Text.h"

#if JUCE_ANDROID
#include <jni.h>
#include <juce_core/native/juce_JNIHelpers_android.h>
#endif

#if JUCE_ANDROID

namespace pg {
namespace {

juce::String jstr(JNIEnv *env, jstring s) {
  if (s == nullptr)
    return {};
  const char *c = env->GetStringUTFChars(s, nullptr);
  const juce::String out = c != nullptr ? juce::String(c) : juce::String();
  if (c != nullptr)
    env->ReleaseStringUTFChars(s, c);
  return out;
}

jstring toJstr(JNIEnv *env, const juce::String &s) {
  return env->NewStringUTF(s.toUTF8());
}

// FindClass desde un hilo nativo usa el classloader del sistema, que no
// ve las clases de la app: lo resolvemos via el ClassLoader del Context.
jclass findClass(JNIEnv *env, const char *name) {
  auto ctx = juce::getAppContext();
  if (ctx.get() == nullptr)
    return nullptr;
  jclass ctxCls = env->GetObjectClass(ctx.get());
  if (ctxCls == nullptr)
    return nullptr;
  jmethodID gcl = env->GetMethodID(ctxCls, "getClassLoader",
                                   "()Ljava/lang/ClassLoader;");
  if (gcl == nullptr) {
    env->DeleteLocalRef(ctxCls);
    return nullptr;
  }
  jobject loader = env->CallObjectMethod(ctx.get(), gcl);
  env->DeleteLocalRef(ctxCls);
  if (loader == nullptr)
    return nullptr;
  jclass clCls = env->FindClass("java/lang/ClassLoader");
  if (clCls == nullptr) {
    env->DeleteLocalRef(loader);
    return nullptr;
  }
  jmethodID load = env->GetMethodID(
      clCls, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
  env->DeleteLocalRef(clCls);
  if (load == nullptr) {
    env->DeleteLocalRef(loader);
    return nullptr;
  }
  jstring cn = env->NewStringUTF(name);
  jclass cls =
      (jclass)env->CallObjectMethod(loader, load, cn);
  env->DeleteLocalRef(cn);
  env->DeleteLocalRef(loader);
  return cls;
}

jclass bridgeClass(JNIEnv *env) {
  return findClass(env, "com.perenkegain.YouTubeBridge");
}

} // namespace

std::vector<YouTubeSearchItem>
YouTubeAndroid::search(const juce::String &query, int maxResults,
                       juce::String &error) {
  std::vector<YouTubeSearchItem> out;
  auto *env = juce::getEnv();
  if (env == nullptr) {
    error = PG_T("No se pudo conectar con la JVM");
    return out;
  }
  jclass cls = bridgeClass(env);
  if (cls == nullptr) {
    error = PG_T("Bridge Java no disponible");
    return out;
  }
  jmethodID mid = env->GetStaticMethodID(
      cls, "search",
      "(Landroid/content/Context;Ljava/lang/String;I)Ljava/lang/String;");
  if (mid == nullptr) {
    env->DeleteLocalRef(cls);
    error = PG_T("Bridge Java: search no encontrado");
    return out;
  }
  auto ctx = juce::getAppContext();
  jstring res = (jstring)env->CallStaticObjectMethod(
      cls, mid, ctx.get(), toJstr(env, query), (jint)maxResults);
  const juce::String raw = jstr(env, res);
  if (res != nullptr)
    env->DeleteLocalRef(res);
  env->DeleteLocalRef(cls);

  const auto parsed = juce::JSON::parse(raw);
  if (const auto *obj = parsed.getDynamicObject()) {
    if (obj->hasProperty("error")) {
      error = obj->getProperty("error").toString();
      return out;
    }
  }
  const auto *arr = parsed.getArray();
  if (arr == nullptr) {
    error = PG_T("Respuesta de búsqueda inválida");
    return out;
  }
  const int limit = juce::jmax(1, maxResults);
  for (const auto &item : *arr) {
    if ((int)out.size() >= limit)
      break;
    const auto *do = item.getDynamicObject();
    if (do == nullptr)
      continue;
    const juce::String title = do->getProperty("title").toString();
    const juce::String url = do->getProperty("url").toString();
    if (title.isEmpty() || !url.contains("watch?v="))
      continue;
    out.push_back({title, url, {}});
  }
  return out;
}

YouTubeResult
YouTubeAndroid::download(const juce::String &url,
                         const juce::File &outDir) {
  YouTubeResult r;
  auto *env = juce::getEnv();
  if (env == nullptr) {
    r.error = PG_T("No se pudo conectar con la JVM");
    return r;
  }
  jclass cls = bridgeClass(env);
  if (cls == nullptr) {
    r.error = PG_T("Bridge Java no disponible");
    return r;
  }
  jmethodID mid = env->GetStaticMethodID(
      cls, "download",
      "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)"
      "Ljava/lang/String;");
  if (mid == nullptr) {
    env->DeleteLocalRef(cls);
    r.error = PG_T("Bridge Java: download no encontrado");
    return r;
  }
  if (!outDir.isDirectory() && !outDir.createDirectory()) {
    env->DeleteLocalRef(cls);
    r.error = PG_T("No se pudo crear la carpeta destino");
    return r;
  }
  auto ctx = juce::getAppContext();
  jstring res = (jstring)env->CallStaticObjectMethod(
      cls, mid, ctx.get(), toJstr(env, url),
      toJstr(env, outDir.getFullPathName()));
  const juce::String raw = jstr(env, res);
  if (res != nullptr)
    env->DeleteLocalRef(res);
  env->DeleteLocalRef(cls);

  if (raw.startsWith("ERROR:")) {
    r.error = raw.substring(6).trim();
    return r;
  }
  juce::File f(raw);
  if (!f.existsAsFile()) {
    r.error = PG_T("No se encontro el archivo descargado");
    return r;
  }
  r.ok = true;
  r.file = f;
  return r;
}

} // namespace pg

#endif // JUCE_ANDROID
