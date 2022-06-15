#!/bin/bash
set -e

# test-npm-packages-install.sh
#
# Tests npm packages created by create-npm-packages.sh can be installed and imported by code.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd -P)"
SOURCE_ROOT="${SCRIPT_DIR}/../.."
TEST_DIR="${SOURCE_ROOT}/build/npm/test"
GET_VERSION_SCRIPT="JSON.parse(require('fs').readFileSync('${SOURCE_ROOT}/config/publishing/version.json', 'utf8')).version"
# paths relative to TEST_DIR to avoid absolute path issues on windows
PUBLISHABLE_DIR="../publishable"

# Setup test directory

mkdir -p "${TEST_DIR}"
cd "${TEST_DIR}"

# Get current publishing version

if [ "$OSTYPE" = "msys" ]; then
  # use node.exe in sub-shell on git bash for windows; otherwise, stdout does not work
  PUBLISHING_VERSION=$(node.exe -p -e "${GET_VERSION_SCRIPT}")
else
  PUBLISHING_VERSION=$(node -p -e "${GET_VERSION_SCRIPT}")
fi

# Install all packages to the working directory

rm -rf package*.json node_modules

echo "{}" > package.json
npm install \
  "${PUBLISHABLE_DIR}/lse-core-${PUBLISHING_VERSION}.tgz" \
  "${PUBLISHABLE_DIR}/lse-style-${PUBLISHING_VERSION}.tgz" \
  "${PUBLISHABLE_DIR}/lse-react-${PUBLISHING_VERSION}.tgz" \
  "${PUBLISHABLE_DIR}/lse-loader-${PUBLISHING_VERSION}.tgz"

# Try to import packages and check that version matches

echo "import('@lse/core').then(({version}) => console.assert(version === '$PUBLISHING_VERSION', '@lse/core version mismatch'))" | node
echo "import('@lse/style').then(({version}) => console.assert(version === '$PUBLISHING_VERSION', '@lse/core version mismatch'))" | node
# XXX: react is not shutting down properly, force exit
echo "import('@lse/react').then(({version}) => { console.assert(version === '$PUBLISHING_VERSION', '@lse/react version mismatch'); process.exit(0); })" | node
