#include <string>
#include <spdlog/spdlog.h>

#include <providers/provider.h>
#include <semver.h>

namespace gitsemver {
    static const std::string DEFAULT_VERSION = "1.0.0";

    std::string Semver::nextVersion() const {
        auto tag = gitProvider_->lastReleasedTag();
        if (tag == providers::NO_GIT_TAG) {
            return DEFAULT_VERSION;
        }

        if (gitProvider_->isTag()) {
            auto tagName = tag.tagName();
            return tagName.replace(tagName.begin(),
                tagName.begin() + providers::REF_TAGS_PREFIX_LENGTH + 1,
                "");
        }

        auto currVersion = providers::extractSemanticVersion(tag.tagName());
        auto newVersion = incrementVersion(currVersion);
        auto currBranch = gitProvider_->getCurrentBranch();
        auto latestCommit = gitProvider_->getLatestCommit().substr(0, shaLength_);

        return newVersion + "-" + currBranch + "-" + latestCommit;
    }

    std::string Semver::incrementVersion(const std::string& version) const {
        int major = -1;
        int minor = -1;
        int patch = -1;

        std::string current;
        for (auto c : version) {
            if (c != '.') {
                current += c;
                continue;
            }

            if (major == -1) {
                major = std::atoi(current.c_str());
            } else if (minor == -1) {
                minor = std::atoi(current.c_str());
            } else {
                patch = std::atoi(current.c_str());
            }

            current = "";
        }

        if (patch == -1 && !current.empty()) {
            patch = std::atoi(current.c_str());
            current = "";
        }

        patch += 1;
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }
}