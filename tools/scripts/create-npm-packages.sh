#!/bin/bash
set -e

# create-npm-packages.sh
#
# Creates npm publishable tarballs for @lse/core, @lse/react and @lse/loader packages. The packages
# are saved to $SRCROOT/build/npm/publishable/*.tgz.
#
# Before this script is run, the version in $SRCROOT/publishing/version.json should be manually changed. After this script
# is run, the created packages can be published with npm/yarn publish command.
#
# This script will build the Light Source Engine project and run tests to verify a valid state. Then, npm pack is
# run on each package. The package.json files field performs filtering of files. .npmignore is not used. The resulting
# tarball contains a package.json with monorepo configuration that should not be published. Also, the package may
# require additional hand patching that cannot be described with package.json files. The tarball is extracted to
# disk and the patches are made. Finally, the files are tarballed into the final package and stored in
# $SCROOT/build/npm/publishable.

_pushd () {
  cd "$@" > /dev/null 2>&1
}

_popd () {
  cd - > /dev/null 2>&1
}

get_version() {
  node -p "require('fs-extra').readJsonSync('$1').version"
}

create_npm_package() {
  TARGET_DIR="${SOURCE_ROOT}/build/npm/$1"
  SOURCE_DIR="${SOURCE_ROOT}/bindings/js/lse/$1"

  # first pass: use npm pack to create a package. it's missing stuff, so unpack it to the
  # package directory and apply additions
  mkdir -p "${TARGET_DIR}"
  VERSION=$(get_version "${SOURCE_DIR}/package.json")

  _pushd "${SOURCE_DIR}"
  npm pack > /dev/null 2>&1
  tar -xzf "lse-${1}-${VERSION}.tgz" -C "${TARGET_DIR}"
  rm "lse-${1}-${VERSION}.tgz"
  _popd

  # apply the overlay to the package.json
  OVERLAY="${SOURCE_DIR}/publishing/package-overlay.json5"

  if [ -f "${OVERLAY}" ]; then
    yarn --cwd "${SOURCE_ROOT}" run package-overlay \
      "${SOURCE_DIR}/package.json" \
      "${OVERLAY}" \
      "${TARGET_DIR}/package/package.json"
  fi

  # apply patches
  PATCH_DIR="${SOURCE_DIR}/publishing/patch"

  if [ -d "${PATCH_DIR}" ]; then
    cp -rf "${PATCH_DIR}/." "${TARGET_DIR}/package"
  fi

  # add LICENSE
  cp "${SOURCE_ROOT}/LICENSE" "${TARGET_DIR}/package"

  # npm pack should clear extraneous files, but just in case..
  find "${TARGET_DIR}/package" -name ".DS_Store" -depth -exec rm {} \;

  # second pass: now the package is ready for shipping
  _pushd "${TARGET_DIR}/package"
  tar -czf "${PUBLISHABLE_DIR}/lse-${1}-${PUBLISHING_VERSION}.tgz" .
  _popd

  # clean up
  rm -r "${TARGET_DIR}"
}

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -P)"
SOURCE_ROOT="${SCRIPT_DIR}/../.."
PUBLISHABLE_DIR="${SOURCE_ROOT}/build/npm/publishable"
PUBLISHING_VERSION=$(get_version "${SOURCE_ROOT}/config/publishing/version.json")

_pushd "${SOURCE_ROOT}"

export npm_config_lse_install_opts="--release"

yarn --cwd "${SOURCE_ROOT}" run sync-core-addon-source

if [ "$1" = "--skip-yarn-build" ]; then
  yarn run bundle
else
  yarn run rebuild --jobs max --release
fi

_popd

rm -rf "${SOURCE_ROOT}/build/npm"
mkdir -p "${PUBLISHABLE_DIR}"

create_npm_package "core"
create_npm_package "style"
create_npm_package "react"
create_npm_package "loader"
