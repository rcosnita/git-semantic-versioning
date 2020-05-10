#include <iostream>
#include <gflags/gflags.h>
#include <spdlog/spdlog.h>
#include <providers/provider.h>
#include <semver.h>

DEFINE_int32(shalength, 6, "Controls how many characters to keep from the latest commit sha.");
DEFINE_string(repository, "./", "Controls the repository for which we try to determine the next release version.");

int main(int argc, char** argv) {
    #ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
    #endif

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    auto shaLength = FLAGS_shalength;
    auto repository = FLAGS_repository.c_str();

    auto provider = gitsemver::providers::newDefaultProvider(repository);
    auto semver = gitsemver::Semver(provider.get(), shaLength);
    auto suggestedVersion = semver.nextVersion();
    std::cout << suggestedVersion;
    return 0;
}