#include "ui/YouTubeSearchTask.h"

namespace pg {

YouTubeSearchTask::YouTubeSearchTask(juce::String q, int max, DoneFn done)
    : juce::Thread("pg-ytsearch"), query(std::move(q)), maxResults(max),
      onDone(std::move(done)) {}

void YouTubeSearchTask::run() {
  juce::String error;
  auto result = YouTubeSearch::search(query, maxResults, error);
  auto cb = onDone;
  juce::MessageManager::callAsync(
      [cb, result, error]() mutable {
        if (cb)
          cb(std::move(result), error);
      });
}

} // namespace pg
