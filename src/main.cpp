#include <iostream>
#include <providers/provider.h>

int main() {
    auto provider = gitsemver::providers::newDefaultProvider("./");
    auto tags = provider->listTags();

    for (auto& t : tags) {
        std::cout << t << std::endl;
    }

    return 0;
}