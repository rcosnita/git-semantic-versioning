FROM debian:buster-slim as build

RUN apt-get update -y && \
    apt-get install -y clang cmake openssl libssl-dev

ADD . /home/git-semantic-versioning
WORKDIR /home/git-semantic-versioning

RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=release ../ && \
    make -j8

FROM debian:buster-slim

COPY --from=build /home/git-semantic-versioning/build/git_semver /home/git-semantic-versioning/git_semver
COPY --from=build /home/git-semantic-versioning/build/git_semver /usr/local/bin/git_semver
