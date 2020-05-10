#include <spdlog/spdlog.h>
#include <providers/provider.h>

int main() {
    #ifdef DEBUG
    spdlog::set_level(spdlog::level::debug);
    #endif

    auto provider = gitsemver::providers::newDefaultProvider("./");
    auto tags = provider->listTags();
    return 0;
}