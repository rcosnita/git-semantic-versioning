#include <iostream>
#include <gflags/gflags.h>
#include <spdlog/spdlog.h>
#include <providers/provider.h>
#include <semver.h>

DEFINE_int32(shalength, 6, "Controls how many characters to keep from the latest commit sha.");
DEFINE_string(repository, "./", "Controls the repository for which we try to determine the next release version.");
DEFINE_string(filter, "", "Optionally apply a regex filter in order to accommodate mono repos. In case this is not specified all tags are considered.");

int main(int argc, char** argv) {
    #ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
    #endif

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    auto shaLength = FLAGS_shalength;
    auto repository = FLAGS_repository.c_str();
    std::string tagsFilter = FLAGS_filter;

    if (!tagsFilter.empty()) {
        spdlog::debug("Using {} filtering for the semantic versioning.", tagsFilter);
    }

    auto provider = gitsemver::providers::newDefaultProvider(repository);
    auto semver = gitsemver::Semver(provider.get(), shaLength);
    auto suggestedVersion = semver.nextVersion(tagsFilter);
    std::cout << suggestedVersion;
    return 0;
}