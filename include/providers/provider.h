#ifndef GITSEMVER_INCLUDE_PROVIDERS_PROVIDER_H_
#define GITSEMVER_INCLUDE_PROVIDERS_PROVIDER_H_

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace gitsemver {
namespace providers {
    static const int SHA_NUM_OF_CHARS = 40;

    /**
     * GitTag provides a simple model for describing a git tag.
     */
    class GitTag {
    public:
        template<typename T = std::string>
        GitTag(T tagName, T lastCommitSha, T authorName, T authorEmail) noexcept :
            tagName_(std::forward<T>(tagName)),
            lastCommitSha_(std::forward<T>(lastCommitSha)),
            authorName_(std::forward<T>(authorName)),
            authorEmail_(std::forward<T>(authorEmail)) {

        }

        std::string tagName() const noexcept;

        std::string lastCommitSha() const noexcept;

        std::string authorName() const noexcept;

        std::string authorEmail() const noexcept;

        friend std::ostream& operator<<(std::ostream& os, const GitTag& tag);

    private:
        std::string tagName_;
        std::string lastCommitSha_;
        std::string authorName_;
        std::string authorEmail_;
    };

    /**
     * GitProvider provides a simple contract for working interacting with the git repositories.
     */
    class GitProvider {
    public:
        using TagsCollection = std::vector<GitTag>;

        template<typename T = std::string>
        GitProvider(T&& repositoryName) noexcept : repositoryName_(std::forward<T>(repositoryName)) {
            static_assert(std::is_convertible<T, std::string>::value);
        }

        /**
         * Returns a list of available tags ordered by creation timestamp.
         */
        virtual TagsCollection listTags() = 0;

        /**
         * Returns the latest released tag for the underlining repository.
         */
        GitTag lastReleasedTag();

    protected:
        /**
         * Each specific provider will implement this in order to initialise all required resources.
         */
        virtual void init() = 0;
    protected:
        std::string repositoryName_;
    };

    /**
     * Instantiates a new git provider with the given arguments. Each provider must accept
     * at least a repository name.
     */
    template<typename T = std::string, class U>
    std::shared_ptr<GitProvider> newProvider(T&& repositoryName) {
        auto provider = new U(std::forward<T>(repositoryName));
        provider->init();
        std::shared_ptr<GitProvider> providerPtr;
        providerPtr.reset(provider);
        return providerPtr;
    }

    /**
     * Instantiates the new git default provider.
     */
    std::shared_ptr<GitProvider> newDefaultProvider(std::string&& repositoryName);
}
} // namespace gitsemver

#endif // GITSEMVER_INCLUDE_PROVIDERS_PROVIDER_H_