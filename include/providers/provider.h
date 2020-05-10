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
    static const std::string REF_HEADS_PREFIX = "ref/heads/";
    static const size_t REF_HEADS_PREFIX_LENGTH = REF_HEADS_PREFIX.length();

    static const std::string REF_TAGS_PREFIX = "ref/tags/";
    static const size_t REF_TAGS_PREFIX_LENGTH = REF_TAGS_PREFIX.length();

    /**
     * Detemines if the given tag name describes a semantic version or not. We support the following conventions:
     * 
     * <prefix><semver>-<suffix>
     * If the given string follows the above convention this function will return true. Otherwise it will be false.
     */
    std::string extractSemanticVersion(const std::string& tagName);

    /**
     * GitTag provides a simple model for describing a git tag.
     */
    class GitTag {
    public:
        template<typename T = std::string>
        GitTag(T tagName, T lastCommitSha, T authorName, T authorEmail, int64_t time) noexcept :
            tagName_(std::forward<T>(tagName)),
            lastCommitSha_(std::forward<T>(lastCommitSha)),
            authorName_(std::forward<T>(authorName)),
            authorEmail_(std::forward<T>(authorEmail)),
            time_(time) {

        }

        std::string tagName() const noexcept;

        std::string lastCommitSha() const noexcept;

        std::string authorName() const noexcept;

        std::string authorEmail() const noexcept;

        int64_t time() const noexcept;

        bool operator==(const GitTag& t);

        friend std::ostream& operator<<(std::ostream& os, const GitTag& tag);

    private:
        std::string tagName_;
        std::string lastCommitSha_;
        std::string authorName_;
        std::string authorEmail_;
        int64_t time_;
    };

    /**
     * Provides a simple placeholder for signaling a no tag situation.
     */
    static const GitTag NO_GIT_TAG{"", "", "", "", 0};

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
         * Returns a list of available tags.
         */
        virtual TagsCollection listTags() = 0;

        /**
         * Returns the latest released tag for the underlining repository.
         */
        GitTag lastReleasedTag();

        /**
         * Determines if the current head is a tag or not.
         */
        virtual bool isTag() = 0;

        /**
         * Retrieves the name of the current branch for the underlining repository.
         */
        virtual std::string getCurrentBranch() const = 0;

        /**
         * Retrieves the latest commit sha from the underlining repository head.
         */
        virtual std::string getLatestCommit() const = 0;

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