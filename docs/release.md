The release process in software development is one of the trickiest parts of the job. Independently of the approach, Continuous deployment or a more traditional approach, a key part of this is the semantic versioning tags.

The current repository helps with always determining semantic version tags which can always be deployed and run. Here are the
potential tags the tool might return once you execute:

```bash
git_semver -repository ./ -shalength 6
```

# Covered scenarios

## 1.0.0-<branch>-<first 6 chars of the latest commit>

This is returned when there is no stable tag containing a semantic version in it.

## 1.0.1-<branch>-<first 6 chars of the latest commit>

The algorithm detected the latest semantic versioning tag as being: 1.0.0. It automatically incremented the patch version by 1.
Don't worry, you can still increase the rest of the tags.

## 1.1.0

You create the 1.1.0 or v1.1.0 or <prefix>1.1.0 tag and you execute the **git_semver** command line on it. This will return the
name of the tag because we assume a tag holds a potential releasable artefact to production.

Here, you can also add suffixes to the tag name. Subsequent invocations of the algorithm will behave as expected.

## Edge cases

### branch and tag pointing to the last commit

The tool will always return the name of the tag because this is the stable released version according to the git history.

# Integration in Continuous Integration

Integrating the tool in your continuous integration pipeline is extremely simple and convenient:

![Release CI](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/rcosnita/git-semantic-versioning/master/docs/diagrams/release.puml)
