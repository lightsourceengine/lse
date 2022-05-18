#!/bin/sh
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
  echo $(node -p "JSON.parse(require('fs').readFileSync('$1', 'utf8')).version")
}

create_npm_package() {
  TARGET_DIR="${SOURCE_ROOT}/build/npm/$1"
  SOURCE_DIR="${SOURCE_ROOT}/lse-package/$1"
  
  mkdir -p "${TARGET_DIR}"
  VERSION=$(get_version "${SOURCE_DIR}/package.json")

  _pushd "${SOURCE_DIR}"
  npm pack > /dev/null 2>&1
  tar -xzf lse-${1}-${VERSION}.tgz -C "${TARGET_DIR}"
  rm lse-${1}-${VERSION}.tgz
  _popd

  OVERLAY="${SOURCE_DIR}/publishing/package-overlay.json5"

  if [ -f "${OVERLAY}" ]; then
    yarn --cwd "${SOURCE_ROOT}" run package-overlay \
      "${SOURCE_DIR}/package.json" \
      "${OVERLAY}" \
      "${TARGET_DIR}/package/package.json"
  fi

  PATCH_DIR="${SOURCE_DIR}/publishing/patch"

  if [ -d "${PATCH_DIR}" ]; then
    cp -rf "${PATCH_DIR}/." "${TARGET_DIR}/package"
  fi

  cp "${SOURCE_ROOT}/LICENSE" "${SOURCE_ROOT}/NOTICE" "${TARGET_DIR}/package"

  # TODO: find a better home for this packaging step
  if [ "$1" = "core" ]; then
    function source_to_target() {
      S="${SOURCE_ROOT}/${1}"
      T="${TARGET_DIR}/package/addon/${2}"

      mkdir -p "${T}"
      cp -r "${S}" "${T}"
    }

    # edit paths in binding.gyp to include libs in package
    cat "${SOURCE_ROOT}/lse-package/core/binding.gyp" \
      | node -e "import('get-stdin').then(g => g.default()).then(b => console.log(b.replaceAll('../../external', 'addon/external').replaceAll('../../lse-lib', 'addon/lse')))" \
      > "${TARGET_DIR}/package/binding.gyp"

    # lse-lib
    source_to_target "lse-lib/inc" "lse"
    source_to_target "lse-lib/src" "lse"
    source_to_target "lse-lib/build.gypi" "lse"
    # freetype
    source_to_target "external/freetype/repo/include" "external/freetype/repo"
    source_to_target "external/freetype/repo/src" "external/freetype/repo"
    source_to_target "external/freetype/repo/LICENSE.TXT" "external/freetype/repo"
    source_to_target "external/freetype/src" "external/freetype"
    source_to_target "external/freetype/build.gypi" "external/freetype"
    # nanosvg
    source_to_target "external/nanosvg" "external"
    # SDL2
    source_to_target "external/SDL2" "external"
    # stb
    source_to_target "external/stb/src" "external/stb"
    source_to_target "external/stb/build.gypi" "external/stb"
    source_to_target "external/stb/repo/stb_image.h" "external/stb/repo"
    # STC
    source_to_target "external/STC/repo/include" "external/STC/repo"
    source_to_target "external/STC/repo/src" "external/STC/repo"
    source_to_target "external/STC/repo/LICENSE" "external/STC/repo"
    source_to_target "external/STC/src/threads.c" "external/STC/src"
    source_to_target "external/STC/build.gypi" "external/STC"
    # yoga
    source_to_target "external/yoga" "external"

    find "${TARGET_DIR}/package" -name “CMakeLists.txt” -depth -exec rm {} \;
  fi

  find "${TARGET_DIR}/package" -name “.DS_Store” -depth -exec rm {} \;

  _pushd "${TARGET_DIR}/package"
  tar -czf "${PUBLISHABLE_DIR}/lse-${1}-${PUBLISHING_VERSION}.tgz" .
  _popd
}

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -P)"
SOURCE_ROOT="${SCRIPT_DIR}/.."
PUBLISHABLE_DIR="${SOURCE_ROOT}/build/npm/publishable"
PUBLISHING_VERSION=$(get_version "${SOURCE_ROOT}/publishing/version.json")

_pushd "${SOURCE_ROOT}"

export npm_config_lse_install_opts="--release"

if [ "$1" = "--skip-yarn-build" ]; then
  yarn run bundle
else
  yarn run build --ignore-engines
fi

_popd

rm -rf "${SOURCE_ROOT}/build/npm"
mkdir -p "${PUBLISHABLE_DIR}"

create_npm_package "core"
create_npm_package "style"
create_npm_package "react"
create_npm_package "loader"

rm -r "${SOURCE_ROOT}/build/npm/core" "${SOURCE_ROOT}/build/npm/style" "${SOURCE_ROOT}/build/npm/react" "${SOURCE_ROOT}/build/npm/loader"
