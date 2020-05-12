FROM debian:buster-slim as build

ADD . /home/git-semantic-versioning
WORKDIR /home/git-semantic-versioning

RUN apt-get update -y && \
    apt-get install -y clang cmake openssl libssl-dev

RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=release ../ && \
    make -j8

FROM debian:buster-slim

COPY --from=build /usr/lib/x86_64-linux-gnu/libcrypto.so.1.1 /usr/lib/x86_64-linux-gnu/libcrypto.so.1.1
COPY --from=build /usr/lib/x86_64-linux-gnu/libssl.so.1.1 /usr/lib/x86_64-linux-gnu/libssl.so.1.1
COPY --from=build /home/git-semantic-versioning/build/git_semver /home/git-semantic-versioning/git_semver
COPY --from=build /home/git-semantic-versioning/build/git_semver /usr/local/bin/git_semver

FROM alpine:latest

COPY --from=build /home/git-semantic-versioning/build/git_semver /usr/local/bin/git_semver