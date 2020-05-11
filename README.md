This command line provides a convenient method for detecting the semantic versioning for a given github repository.
It comes handy to use this in a Continuous Integration pipeline where it is necessary to produce artefacts.

The command line currently supports the following git versioning schemes:

* [Gitflow](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow)
* [Github flow](https://guides.github.com/introduction/flow/)

# Prerequisites

* CMake 3.17
* gcc / clang compiler versions supporting C++17 at least (e.g clang 11.0.3).
* Openssl 1.x

# Getting started

```bash
cd <workspace>/build
cmake ../
make -j8
cd ../
./build/git_semver -repository ./ -shalength 10
```

## Build for production

```bash
cd <workspace>/build
cmake -DCMAKE_BUILD_TYPE=release ../
make -j8
```

# Potential use cases

* Continuous integration pipelines which must generate intermediate artefacts for continuous deployment.
* Automatic tagging or code or docker images.

You can either execute the docker container or simply download the binary for your OS flavour.

## Next steps

* [Release document](docs/release.md)