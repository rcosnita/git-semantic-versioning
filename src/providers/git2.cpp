#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <git2.h>
#include <spdlog/spdlog.h>
#include <providers/provider.h>

namespace gitsemver {
namespace providers {
    /**
     * Git2 provides a specific implementation for the git provider relying on libgit2 library.
     */
    class Git2 : public GitProvider {
    public:
        template<typename T = std::string>
        Git2(T&& repositoryName) : GitProvider(std::forward<T>(repositoryName)) {
            git_libgit2_init();
            spdlog::debug("libgit2 initialised successfully.");
        }

        virtual ~Git2() noexcept {
            spdlog::debug("closing repository {}", repositoryName_);
            git_repository_free(underliningRepo_);
            git_libgit2_shutdown();
            spdlog::debug("libgit2 shutdown successfully.");
        }

        TagsCollection listTags() override {
            TagsCollection tags;
            TagsClosureContext ctx{this, &tags};

            int error = git_tag_foreach(underliningRepo_, [](const char* tagName, git_oid *oid, void* payload) {
                auto ctx = reinterpret_cast<TagsClosureContext*>(payload);
                auto provider = ctx->provider;
                auto repo = provider->underliningRepo_;

                git_object *obj;
                provider->throwIfGit2Error(git_object_lookup(&obj, repo, oid, GIT_OBJECT_ANY));
                auto gitObjPtr = provider->createAutoDeleter(obj, git_object_free);

                if (git_object_type(obj) == GIT_OBJECT_COMMIT) {
                    auto t = provider->buildFromCommit(oid, tagName);
                    ctx->tags->push_back(t);
                } else {
                    auto t = provider->buildFromAnnotatedTag(oid, tagName);
                    ctx->tags->push_back(t);
                }

                return 0;
            }, &ctx);
            throwIfGit2Error(error);
            return tags;
        }

        std::string getCurrentBranch() const override {
            git_reference* head;
            throwIfGit2Error(git_repository_head(&head, underliningRepo_));
            auto headPtr = createAutoDeleter(head, [](git_reference* head) {
                git_reference_free(head);
            });

            if (git_reference_is_branch(head)) {
                std::string branchName = git_reference_name(head);
                return branchName.replace(branchName.begin(), branchName.begin() + REF_HEADS_PREFIX_LENGTH + 1, "");
            }

            return "";
        }

        std::string getLatestCommit() const override {
            git_reference* head;
            throwIfGit2Error(git_repository_head(&head, underliningRepo_));
            auto headPtr = createAutoDeleter(head, [](git_reference* h) {
                git_reference_free(h);
            });

            auto oid = git_reference_target(head);
            char commitNumber[SHA_NUM_OF_CHARS];
            std::string sha = git_oid_tostr(commitNumber, SHA_NUM_OF_CHARS, oid);
            return std::move(sha);
        }

        bool isTag(std::string& tagName) override {
            git_reference* head;
            throwIfGit2Error(git_repository_head(&head, underliningRepo_));
            auto headPtr = createAutoDeleter(head, [](git_reference* h) {
                git_reference_free(h);
            });

            auto refName = git_reference_name(head);
            git_oid oid;
            throwIfGit2Error(git_reference_name_to_id(&oid, underliningRepo_, refName));

            char commitNumber[SHA_NUM_OF_CHARS];
            std::string sha = git_oid_tostr(commitNumber, SHA_NUM_OF_CHARS, &oid);
            auto tags = listTags();
            auto it = std::find_if(tags.begin(), tags.end(), [&sha](const GitTag& t) {
                return t.lastCommitSha() == sha;
            });

            tagName = "";
            if (it != tags.end()) {
                spdlog::debug("Tag name is: {}", it->tagName());
                tagName = it->tagName();
            }

            return it != tags.end();
        }

    protected:
        void init() override {
            auto repoName = repositoryName_.c_str();
            spdlog::debug("Opening repository {}", repoName);
            int errCode = git_repository_open(&underliningRepo_, repoName);
            throwIfGit2Error(errCode);
        }

        template<typename T, class U>
        friend std::shared_ptr<GitProvider> newProvider(T&& repositoryName);

    private:
        /**
         * Provides the context which must be passed to git2 tags iterator function.
         */
        struct TagsClosureContext {
            Git2* provider;
            TagsCollection* tags;
        };

        /**
         * Builds a mechanism for automatically releasing the given object when no one uses it anymore.
         * The deleter function is one of the methods from libgit2 library.
         */
        template<typename T, typename D = std::function<void(T*)>>
        std::unique_ptr<T, std::function<void(T*)>> createAutoDeleter(T* obj, D deleter) const {
            return std::unique_ptr<T, std::function<void(T*)>>(obj, [deleter](T* obj) {
                deleter(obj);
            });
        }

        /**
         * Builds a git tag from the underlining object referencing a commit.
         */
        GitTag buildFromCommit(git_oid* oid, const std::string& tagName) const {
            git_commit *commit;
            int error = git_commit_lookup(&commit, underliningRepo_, oid);
            throwIfGit2Error(error);
            auto commitPtr = createAutoDeleter(commit, git_commit_free);

            char commitId[SHA_NUM_OF_CHARS];
            std::string tag = tagName;
            std::string sha = git_oid_tostr(commitId, SHA_NUM_OF_CHARS, oid);

            auto author = git_commit_author(commit);
            std::string authorName = author->name;
            std::string authorEmail = author->email;

            return GitTag{tag, sha, authorName, authorEmail, author->when.time};
        }

        /**
         * Builds a git tag from the underlining object referencing an annotated tag.
         */
        GitTag buildFromAnnotatedTag(git_oid* oid, const std::string& tagName) const {
            git_tag* gitTag;
            int error = git_tag_lookup(&gitTag, underliningRepo_, oid);
            throwIfGit2Error(error);
            auto targetId = git_tag_target_id(gitTag);
            char commitId[SHA_NUM_OF_CHARS];
            std::string tag = tagName;
            std::string sha = git_oid_tostr(commitId, SHA_NUM_OF_CHARS, targetId);

            auto author = git_tag_tagger(gitTag);
            std::string authorName = author->name;
            std::string authorEmail = author->email;

            return GitTag{tag, sha, authorName, authorEmail, author->when.time};
        }

        /**
         * Throws an exception in case the give error code indicates an error (< 0).
         */
        void throwIfGit2Error(int errCode) const {
            if (errCode < 0) {
                auto err = git_error_last();
                throw std::logic_error(err->message);
            }
        }

    private:
        git_repository *underliningRepo_;
    };

    std::shared_ptr<GitProvider> newDefaultProvider(std::string&& repositoryName) {
        return newProvider<std::string, Git2>(std::move(repositoryName));
    }
}
} // namespace gitsemver