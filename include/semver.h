#ifndef GITSEMVER_INCLUDE_SEMVER_H_
#define GITSEMVER_INCLUDE_SEMVER_H_

#include <exception>

#include <providers/provider.h>

namespace gitsemver {
    /**
     * Semver provides the logic for interacting with an underlining repository.
     */
    class Semver {
    public:
        template<typename T>
        Semver(T&& gitProvider, int shaLength = providers::SHA_NUM_OF_CHARS)
            : gitProvider_(std::forward<T>(gitProvider)),
                shaLength_(shaLength) {

        }

        /**
         * Retrieves the next potential version of the code base based on the past created tags.
         * In case no previous tag was created, we will suggest 0.1.0-<branch-name>-<commit sha>.
         * Branch name will be omitted for develop and master.
         * In case the tool is executed against a tag, the tag name will be returned.
         */
        std::string nextVersion() const;

        virtual ~Semver() noexcept = default;

    private:
        /**
         * Increments the patch version by 1 and returns the new version.
         */
        std::string incrementVersion(const std::string& version) const;

    private:
        providers::GitProvider* gitProvider_;
        int shaLength_;
    };
}

#endif // GITSEMVER_INCLUDE_SEMVER_H_