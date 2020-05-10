#include <iostream>
#include <spdlog/spdlog.h>
#include <providers/provider.h>
#include <semver.h>

int main() {
    #ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
    #endif

    auto provider = gitsemver::providers::newDefaultProvider("./");
    auto semver = gitsemver::Semver(provider.get(), 6);
    auto suggestedVersion = semver.nextVersion();
    std::cout << suggestedVersion;
    return 0;
}