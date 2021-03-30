#include <algorithm>
#include <regex>
#include <stdexcept>

#include <spdlog/spdlog.h>
#include <providers/provider.h>

namespace gitsemver {
namespace providers {
    std::string GitTag::tagName() const noexcept {
        return tagName_;
    }

    std::string GitTag::lastCommitSha() const noexcept {
        return lastCommitSha_;
    }

    std::string GitTag::authorName() const noexcept {
        return authorName_;
    }

    std::string GitTag::authorEmail() const noexcept {
        return authorEmail_;
    }

    bool GitTag::operator==(const GitTag& t) {
        return authorEmail() == t.authorEmail() && \
            authorName() == t.authorName() && \
            lastCommitSha() == t.lastCommitSha() && \
            tagName() == t.tagName() && \
            time() == t.time();
    }

    int64_t GitTag::time() const noexcept {
        return time_;
    }

    std::ostream& operator<<(std::ostream& os, const GitTag& tag) {
        os << "Tag: " << tag.tagName() << "... Sha: " << tag.lastCommitSha() << " .. Author: " << tag.authorName() << " <" << tag.authorEmail() << ">";
        return os;
    }

    const std::regex semVerRegex{".*([0-9]{1,}\\.[0-9]{1,}\\.[0-9]{1,}).*",
        std::regex::extended | std::regex_constants::icase};

    /**
     * Detemines if the given tag name describes a semantic version or not. We support the following conventions:
     * 
     * <prefix><semver>-<suffix>
     * If the given string follows the above convention this function will return true. Otherwise it will be false.
     */
    std::string extractSemanticVersion(const std::string& tagName) {
        if (tagName.empty()) {
            return "";
        }

        std::smatch m;
        if (std::regex_search(tagName, m, semVerRegex)) {
            spdlog::debug("semver tag {}", tagName);
            auto version = m[1].str();
            return std::move(version);
        }

        return "";
    }

    bool isMatchingTag(const std::regex& tagRegex, const GitTag& tag) {
        auto isMatching = std::regex_match(tag.tagName(), tagRegex);
        return isMatching;
    }

    GitTag GitProvider::lastReleasedTag(const std::string& tagsFilter) {
        auto tags = listTags();

        std::sort(tags.begin(), tags.end(), [](GitTag a, GitTag b) {
            return a.time() > b.time();
        });


        std::regex tagRegex(tagsFilter);
        TagsCollection semanticVersions;
        std::copy_if(tags.begin(), tags.end(), std::back_inserter(semanticVersions),
                     [&tagsFilter, &tagRegex](const GitTag& t) {
            if (tagsFilter.empty()) {
                return !extractSemanticVersion(t.tagName()).empty();;
            }

            return isMatchingTag(tagRegex, t) && !extractSemanticVersion(t.tagName()).empty();
        });

        if (semanticVersions.size() == 0) {
            spdlog::debug("No tags available.");
            return NO_GIT_TAG;
        }

        return semanticVersions[0];
    }
}
} // namespace gitsemver