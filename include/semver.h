#ifndef GITSEMVER_INCLUDE_SEMVER_H_
#define GITSEMVER_INCLUDE_SEMVER_H_

#include <providers/provider.h>

namespace gitsemver {
    /**
     * Semver provides the logic for interacting with an underlining repository.
     */
    class Semver {
    public:
        template<typename T>
        Semver(T&& gitProvider) : gitProvider_(std::forward<T>(gitProvider)) {

        }

        virtual ~Semver() noexcept {
        }

    private:
        providers::GitProvider gitProvider_;
    };
}

#endif // GITSEMVER_INCLUDE_SEMVER_H_