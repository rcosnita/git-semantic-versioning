#include <stdexcept>


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

    std::ostream& operator<<(std::ostream& os, const GitTag& tag) {
        os << "Tag: " << tag.tagName() << "... Sha: " << tag.lastCommitSha() << " .. Author: " << tag.authorName() << " <" << tag.authorEmail() << ">";
        return os;
    }

    GitTag GitProvider::lastReleasedTag() {
        throw std::logic_error("not implemented yet ...");
    }
}
} // namespace gitsemver