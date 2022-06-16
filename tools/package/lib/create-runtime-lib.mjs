/*
 * Copyright (c) 2022 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

import { basename, dirname, join } from 'path'
import { PROJECT_ROOT, CONFIG_ROOT } from '@internal/common/paths'
import { access, chmod, readFile, writeFile, rename } from 'fs/promises'
import Handlebars from 'handlebars'
import Spinnies from 'spinnies'
import { copy, emptyDir, ensureDir, remove } from 'fs-extra'
import fetch from 'node-fetch'
import { EOL, tmpdir } from 'os'
import { spawn } from 'child_process'
import tar from 'tar'
import { createWriteStream } from 'fs'
import Zip from 'adm-zip'
import JSON5 from 'json5'
import { pipeline } from 'stream'
import { promisify } from 'util'
import { resolveInternalPackagePath, resolveLsePackagePath } from './paths.mjs'
import { URL } from 'url'

const GAME_CONTROLLER_DB_URL =
  'https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt'
const ENTRY_POINT_PERMISSIONS = 0o777
const TEMP_DIR = tmpdir()
const VERSION_REGEX = /^(?:\d+|\*)(?:\.(?:\d+|\*))*$/
const LSE_CI_URL = 'https://github.com/lightsourceengine/ci/releases/download/v2.0.0/'
const SDL_ORG_URL = 'https://www.libsdl.org/release/'

const Platform = Object.freeze({
  windows: 'windows',
  macos: 'macos',
  linux: 'linux'
})

const Format = Object.freeze({
  tgz: 'tgz',
  hmod: 'hmod',
  zip: 'zip'
})

const PlatformType = Object.freeze({
  pi: 'pi',
  nclassic: 'nclassic'
})

const Arch = Object.freeze({
  x64: 'x64',
  armv6l: 'armv6l',
  armv7l: 'armv7l',
  arm64: 'arm64'
})

const NodeSourceAlias = Object.freeze({
  nodejs: 'https://nodejs.org/dist/',
  unofficial: 'https://unofficial-builds.nodejs.org/download/release/',
  ci: 'https://github.com/lightsourceengine/custom-node-builds/releases/download/'
})

const NodePlatform = Object.freeze({
  [Platform.windows]: 'win',
  [Platform.macos]: 'darwin',
  [Platform.linux]: 'linux'
})

const ARG_DEFAULTS = Object.freeze({
  recipe: '',
  skipCompile: false,
  preserveStaging: false,

  installGameControllerDb: false,
  platform: process.platform,
  platformType: '',
  targetArch: process.arch,
  sdlRuntime: '',
  sdlMixerRuntime: '',
  nodeBinCompress: false,
  nodeBinStrip: false,
  nodeMinimal: true,
  nodeSrc: NodeSourceAlias.nodejs,
  nodeCustomTag: '',
  coreBinCompress: false,
  coreBinStrip: false,
  format: Format.tgz
})

const assert = (condition, message) => {
  if (!condition) {
    console.error(message)
    process.exit(1)
  }
}

const checkOption = (options, name, values) => {
  assert(options[name] in values, `${name}: invalid value of ${options[name]}`)
}

const getCrossProfile = (options) => {
  if (options.platformType === PlatformType.nclassic) {
    return options.platformType
  } else {
    return options.targetArch
  }
}

const getBinary = (ctx, binaryId) => {
  if (binaryId === 'node') {
    if (ctx.options.platform === Platform.windows) {
      return join(ctx.staging.node, 'node.exe')
    } else {
      return join(ctx.staging.node, 'bin', 'node')
    }
  } else if (binaryId === 'core') {
    // find lse-core.node staging location via the manifest
    const e = ctx.manifest.find(value => value.src.endsWith('lse-core.node'))

    return join(e.dest, basename(e.src))
  }
}

const getStrip = (options) => {
  const { CROSS_TOOLCHAIN_PREFIX } = process.env

  if (CROSS_TOOLCHAIN_PREFIX && [Arch.armv6l, Arch.armv7l, Arch.arm64].includes(options.targetArch)) {
    return `${CROSS_TOOLCHAIN_PREFIX}-strip`
  }

  return 'strip'
}

const copyTo = async (srcFile, destDir, destFile) => copy(srcFile, join(destDir, destFile ?? basename(srcFile)))

const spawnAsync = async (program, args, options) => new Promise((resolve, reject) => {
  const subprocess = spawn(program, args, options)
  let out = ''

  subprocess.stdout.setEncoding('utf8').on('data', (data) => { out += data.toString() })
  subprocess.stderr.setEncoding('utf8').on('data', (data) => { out += data.toString() })
  subprocess.on('close', (code) => {
    if (code !== 0) {
      console.error(out)
      reject(Error(`${args[0]} error ${code}`))
    } else {
      resolve()
    }
  })
  subprocess.on('error', reject)
})

const exec = async (program, ...args) => spawnAsync(program, args, { shell: true, cwd: PROJECT_ROOT, env: { ...process.env } })

const isZip = url => url.endsWith('.zip')

const isTarGz = url => url.endsWith('.tgz') || url.endsWith('.tar.gz')

const isDmg = url => url.endsWith('.dmg')

const isFramework = url => url.endsWith('.framework')

const isFrameworkOf = (url, name) => url.endsWith(name + '.framework')

const createZip = async (localPath, zipPath, outFile) => {
  const z = new Zip()

  z.addLocalFolder(localPath, zipPath)

  await new Promise((resolve, reject) => z.writeZip(outFile, (err) => { err ? reject(err) : resolve() }))
}

const createTarGz = async (workingDir, files, outFile) => {
  await tar.create({
    file: outFile,
    gzip: true,
    C: workingDir
  }, files)
}

const emptyTempDir = async (name) => {
  const temp = join(TEMP_DIR, name)

  await emptyDir(temp)

  return temp
}

const downloadFile = async (url, options = {}) => {
  const file = basename(url)
  const cache = join(TEMP_DIR, 'lse-create-runtime-cache')
  const cacheFile = join(cache, file)

  try {
    await access(cacheFile)
    return cacheFile
  } catch (e) {
    // not in cache
  }

  await ensureDir(cache)

  const streamPipeline = promisify(pipeline)
  const response = await fetch(url, { redirect: 'follow' })

  assert(response.ok, `unexpected response ${response.statusText}`)

  await streamPipeline(response.body, createWriteStream(cacheFile))

  return cacheFile
}

const extract = async (url, workingDir, options = {}) => {
  const { files } = options

  if (url.startsWith('https://')) {
    if (isTarGz(url)) {
      const file = await downloadFile(url, options)

      await tar.x({ file, C: workingDir }, files)
    } else if (isZip(url)) {
      const file = await downloadFile(url, options)

      await extractZip(file, workingDir, files)
    } else if (isDmg(url)) {
      const file = await downloadFile(url, options)

      await spawnAsync('hdiutil', ['attach', file, '-mountpoint', 'LSE_TEMP'])

      try {
        await copy('./LSE_TEMP', workingDir)
      } finally {
        await spawnAsync('hdiutil', ['detach', 'LSE_TEMP'])
      }
    }
  } else if (isZip(url)) {
    await extractZip(url, workingDir, files)
  } else if (isTarGz(url)) {
    await tar.x({ C: workingDir }, files)
  } else if (isDmg(url)) {
    await spawnAsync('hdiutil', ['attach', url, '-mountpoint', 'LSE_TEMP'])

    try {
      await copy('./LSE_TEMP', workingDir)
    } finally {
      await spawnAsync('hdiutil', ['detach', 'LSE_TEMP'])
    }

    // TODO: 7z does not extract symlinks correctly!
    // await spawnAsync('7z', ['x', url], { cwd: workingDir, shell: true })
  } else {
    assert(false, `unsupported archive: ${url}`)
  }
}

const extractZip = async (zipFile, workingDir, files) => {
  const zip = new Zip(zipFile)

  if (files && files.length) {
    const writes = []

    zip.getEntries()
      .filter(({ entryName }) => files.includes(entryName))
      .forEach(entry => {
        const write = new Promise((resolve, reject) => {
          return entry.getDataAsync((buffer, err) => err ? reject(Error(err)) : resolve(buffer))
        }).then((buffer) => {
          const file = join(workingDir, entry.entryName)
          return ensureDir(dirname(file)).then(() => writeFile(file, buffer))
        })

        writes.push(write)
      })

    if (writes.length) {
      await Promise.all(writes)
    }
  } else {
    await new Promise((resolve, reject) => {
      zip.extractAllToAsync(workingDir, false, true, (err) => err ? reject(err) : resolve())
    })
  }
}

export const setupTemplateEngine = () => {
  Handlebars.registerHelper('isMacOS', (value) => value === Platform.macos)
  Handlebars.registerHelper('isLinux', (value) => value === Platform.linux)
  Handlebars.registerHelper('isWindows', (value) => value === Platform.windows)
  Handlebars.registerHelper('isNClassic', (value) => value === PlatformType.nclassic)
  Handlebars.registerHelper('isPi', (value) => value === PlatformType.pi)
  Handlebars.registerHelper('isAbsolute', (value) => value.startsWith('/'))
}

const createStatusReporter = () => {
  const spinnies = new Spinnies()
  const Status = class {
    constructor (id) {
      this.id = id
      spinnies.add(id, { text: `[${id}]: waiting` })
    }

    update (text) {
      spinnies.update(this.id, { text: `[${this.id}]: ${text}` })
    }

    succeed (text = 'done') {
      spinnies.succeed(this.id, { text: `[${this.id}]: ${text}` })
    }
  }

  return {
    node: new Status('node'),
    compile: new Status('compile'),
    lib: new Status('lib'),
    assets: new Status('assets'),
    entryPoint: new Status('entryPoint'),
    package: new Status('package')
  }
}

export const buildOptions = (args, recipe) => {
  checkOption(recipe, 'format', Format)
  checkOption(recipe, 'platform', Platform)

  const options = {
    ...ARG_DEFAULTS,
    ...args,
    ...recipe,
    id: `lse-${recipe.platformType || recipe.platform}-${recipe.targetArch}`
  }

  if (options.nodeSrc && options.nodeSrc in NodeSourceAlias) {
    options.nodeSrc = NodeSourceAlias[options.nodeSrc]
  }

  options.isCrossCompile = (options.platform === Platform.linux && options.targetArch !== process.arch)

  if (options.isCrossCompile) {
    assert(process.env.CROSS_TOOLCHAIN_PREFIX,
      'CROSS_TOOLCHAIN_PREFIX must be set before cross compiler can run.')
  }

  if (VERSION_REGEX.test(options.sdlRuntime)) {
    let runtimeUrl

    switch (options.platform) {
      case Platform.macos:
        runtimeUrl = new URL(`SDL2-${options.sdlRuntime}.dmg`, SDL_ORG_URL)
        break
      case Platform.windows:
        runtimeUrl = new URL(`SDL2-${options.sdlRuntime}-win32-${options.targetArch}.zip`, SDL_ORG_URL)
        break
      case Platform.linux:
        if (options.platformType === PlatformType.pi) {
          runtimeUrl = new URL(`SDL2-${options.sdlRuntime}-${options.targetArch}-${options.platformType}.tgz`, LSE_CI_URL)
        }
        break
      default:
        break
    }

    assert(runtimeUrl, 'No prebuilt SDL2 libraries for this platform')

    options.sdlRuntime = runtimeUrl?.href ?? ''
  }

  if (options.sdlRuntime) {
    switch (options.platform) {
      case Platform.macos:
        assert(isDmg(options.sdlRuntime) ||
          isFrameworkOf(options.sdlRuntime, 'SDL2'), 'expected runtime to be .dmg or .framework')
        break
      case Platform.windows:
        assert(isZip(options.sdlRuntime), 'expected runtime to be ZIP')
        break
      case Platform.linux:
        assert(isTarGz(options.sdlRuntime), 'expected runtime to be .tgz or tar.gz')
        break
    }
  }

  return options
}

export const loadRecipe = async (recipe) => {
  let recipeFile

  if (recipe.startsWith('recipe://')) {
    recipeFile = join(CONFIG_ROOT, 'runtime', 'recipes', recipe.replace('recipe://', ''))

    if (!recipeFile.endsWith('.json5')) {
      recipeFile += '.json5'
    }
  } else {
    recipeFile = recipe
  }

  const contents = await readFile(recipeFile, 'utf8')
  const options = JSON5.parse(contents)
  const result = {}

  for (const [key, value] of Object.entries(options)) {
    const camelCaseKey = key
      .split('-')
      .map((v, i) => i > 0 ? v[0].toUpperCase() + v.substr(1) : v)
      .join('')

    result[camelCaseKey] = value
  }

  return result
}

export const parallel = async (...args) => Promise.all(args)

export const installEntryPoint = async (ctx) => {
  ctx.status.entryPoint.update('installing script')

  const entryPoint = ctx.options.platform === Platform.windows ? 'lse.cmd' : 'lse'
  const contents = await readFile(join(CONFIG_ROOT, 'runtime', 'entry', entryPoint + '.template'), 'utf8')

  setupTemplateEngine()

  const template = Handlebars.compile(contents)
  const entryPointFile = join(ctx.staging.entryPoint, entryPoint)

  await ensureDir(ctx.staging.entryPoint)
  await writeFile(entryPointFile, template(ctx.options))
  await chmod(entryPointFile, ENTRY_POINT_PERMISSIONS)
}

export const installGameControllerDb = async (ctx) => {
  if (!ctx.options.installGameControllerDb) {
    return
  }

  ctx.status.assets.update('installing gamecontrollerdb.txt')

  const file = await downloadFile(GAME_CONTROLLER_DB_URL, ctx.options)
  const csv = await readFile(file, 'utf8')
  const platformMap = {
    [Platform.macos]: 'platform:Mac OS X',
    [Platform.windows]: 'platform:Windows',
    [Platform.linux]: 'platform:Linux'
  }

  await ensureDir(ctx.staging.assets)
  await writeFile(
    join(ctx.staging.assets, basename(file)),
    // it's a big file. constrain entries to target platform
    csv.split(/\r?\n/)
      .filter(line => line.indexOf(platformMap[ctx.options.platform]) !== -1)
      .join(EOL))
}

export const installMeta = async (ctx) => {
  if (ctx.options.format === Format.hmod && ctx.options.platformType === PlatformType.nclassic) {
    ctx.status.entryPoint.update('installing uninstall script')
    await copyTo(join(CONFIG_ROOT, 'runtime', 'meta', 'uninstall-nclassic.sh'), ctx.staging.root, 'uninstall')
  }
}

export const compile = async (ctx) => {
  if (ctx.options.skipCompile) {
    return
  }

  ctx.status.compile.update('yarn run bundle')

  await exec('yarn', '--force')
  await exec('yarn', 'run', 'sync-core-addon-source')

  ctx.status.compile.update('yarn run rebuild')

  if (ctx.options.isCrossCompile) {
    await exec('cross', getCrossProfile(ctx.options), 'yarn', 'run', 'rebuild', '--jobs', 'max', '--release')
  } else {
    await exec('yarn', 'run', 'rebuild', '--jobs', 'max', '--release')
  }
}

export const installNode = async (ctx) => {
  const { nodeMinimal, platform, targetArch, nodeSrc, nodeCustomTag } = ctx.options
  const { version } = process
  const custom = nodeCustomTag ? `-${nodeCustomTag}` : ''
  const nodePlatform = NodePlatform[platform]
  let tag
  let files
  let ext

  ctx.status.node.update('installing')

  if (platform === Platform.windows) {
    tag = `node-${version}${custom}-${nodePlatform}-${targetArch}`
    ext = '.zip'
    files = [
      `${tag}/node.exe`,
      `${tag}/LICENSE`
    ]
  } else {
    tag = `node-${version}${custom}-${nodePlatform}-${targetArch}`
    ext = '.tar.gz'
    files = [
      `${tag}/bin/node`,
      `${tag}/LICENSE`
    ]
  }

  const url = new URL(`${version}/${tag}${ext}`, nodeSrc)
  const temp = await emptyTempDir('lse-node-scratch')

  // extract to a temporary directory
  await extract(url.href, temp, nodeMinimal ? { files, ...ctx.options } : ctx.options)

  // ensure the directory containing the node folder exists in staging
  await ensureDir(ctx.staging.node)

  // move from temp to the node folder in staging
  await renameOrCopy(join(temp, tag), ctx.staging.node)
}

// if rename fails due to cross partition error, fall back to copy. on some linux systems, /tmp is a separate partition
const renameOrCopy = async (from, to) => {
  try {
    await rename(from, to)
  } catch (e) {
    // linux: if /tmp is in its own partition, rename throws EXDEV (cross device link) -> try a copy instead
    // windows: renaming from temp -> build throws EPERM (permissions) -> try a copy instead
    if (e.code === 'EXDEV' || e.code === 'EPERM') {
      await ensureDir(to)
      await copy(from, to)
    } else {
      throw e
    }
  }
}

export const installSDL = async (ctx) => {
  const { platform, platformType, sdlRuntime, sdlMixerRuntime } = ctx.options

  if (!sdlRuntime && !sdlMixerRuntime) {
    return
  }

  ctx.status.lib.update('installing SDL')

  await ensureDir(ctx.staging.lib)

  if (platformType === PlatformType.pi) {
    if (sdlRuntime) {
      await installSDLForPi(ctx, 'SDL2', sdlRuntime)
    }
  } else if (platform === Platform.macos) {
    await parallel(
      sdlRuntime ? installSDLFramework(ctx, 'SDL2', sdlRuntime) : Promise.resolve()
    )
  } else if (platform === Platform.windows) {
    await parallel(
      sdlRuntime ? installSDLWindows(ctx, 'SDL2', sdlRuntime) : Promise.resolve()
    )
  }
}

const installSDLWindows = async (ctx, name, runtime) => {
  const temp = await emptyTempDir('lse-sdl-windows-zip')
  const lib = `${name}.dll`
  const tempLib = join(temp, lib)

  await extract(runtime, temp, ctx.options)
  await copy(tempLib, join(ctx.staging.lib, lib))
}

const installSDLFramework = async (ctx, name, runtime) => {
  const framework = `${name}.framework`

  if (isFramework(runtime)) {
    const frameworkPath = join(ctx.staging.lib, basename(runtime))

    await ensureDir(frameworkPath)
    await copy(runtime, frameworkPath)
  } else if (isDmg(runtime)) {
    const temp = await emptyTempDir('lse-sdl-macos-dmg')
    const tempFramework = join(temp, framework)

    await extract(runtime, temp, ctx.options)
    await emptyDir(join(tempFramework, 'Versions', 'A', 'Headers'))
    await rename(tempFramework, join(ctx.staging.lib, framework))
  } else {
    assert(false, `bad runtime value: ${runtime}`)
  }
}

const installSDLForPi = async (ctx, name, runtime) => {
  const temp = await emptyTempDir('lse-sdl-pi-tgz')
  const soName = 'libSDL2.so'
  const soPath = join(temp, basename(runtime, '.tgz'), soName)

  await extract(runtime, temp, ctx.options)
  await copy(soPath, join(ctx.staging.lib, soName))
}

export const installIntrinsicModules = async (ctx) => {
  for (const entry of ctx.manifest) {
    if (entry.src.endsWith('package.json')) {
      const contents = await readFile(entry.src, 'utf8')
      const pkg = JSON.parse(contents)
      const dest = join(entry.dest, pkg.name)

      await ensureDir(dest)
      await copy(entry.src, join(dest, basename(entry.src)))

      for (const file of Object.values(pkg.exports)) {
        await copy(join(dirname(entry.src), file), join(dest, file))
      }
    } else {
      await ensureDir(entry.dest)
      await copy(entry.src, join(entry.dest, basename(entry.src)))
    }
  }
}

export const stripBinary = async (ctx, status, binaryId) => {
  if (!ctx.options[`${binaryId}BinStrip`]) {
    return
  }

  status.update(`stripping ${binaryId} symbols`)

  await exec(getStrip(ctx.options), getBinary(ctx, binaryId))
}

export const compressBinary = async (ctx, status, binaryId) => {
  if (!ctx.options[`${binaryId}BinCompress`]) {
    return
  }

  status.update('compressing executable with upx')

  await exec('upx', '-q', getBinary(ctx, binaryId))
}

export const createPackage = async (ctx) => {
  const { id, format } = ctx.options
  const file = join(ctx.build, id + '.' + format)

  ctx.status.package.update('creating')

  switch (format) {
    case Format.tgz:
      await createTarGz(ctx.build, [id], file)
      break
    case Format.hmod:
      await createTarGz(ctx.staging.root, ['usr', 'uninstall'], file)
      break
    case Format.zip:
      await createZip(ctx.staging.root, id, file)
      break
    default:
      assert(false, `unsupported format: ${format}`)
  }

  if (!ctx.options.preserveStaging) {
    await remove(ctx.staging.root)
  }

  return file
}

export const createContext = async (args) => {
  const ctx = {
    cl: args,
    options: buildOptions(args, await loadRecipe(args.recipe)),
    status: createStatusReporter(),
    build: join(PROJECT_ROOT, 'build')
  }

  const root = join(ctx.build, ctx.options.id)

  if (ctx.options.format === Format.zip) {
    ctx.staging = {
      entryPoint: '',
      lib: 'lib',
      assets: 'assets',
      node: 'node',
      intrinsic: 'intrinsic',
      license: ''
    }
  } else {
    const prefix = (ctx.options.format === Format.hmod) ? 'usr' : ''

    ctx.staging = {
      entryPoint: join(prefix, 'bin'),
      lib: join(prefix, 'share', 'lib'),
      assets: join(prefix, 'share', 'assets'),
      node: join(prefix, 'share', 'node'),
      intrinsic: join(prefix, 'share', 'intrinsic'),
      license: join(prefix, 'share', 'license')
    }
  }

  Object.keys(ctx.staging).forEach(key => (ctx.staging[key] = join(root, ctx.staging[key])))

  await emptyDir(root)
  ctx.staging.root = root
  ctx.manifest = createManifest(ctx.staging)

  return ctx
}

const createManifest = (staging) => [
  { src: resolveLsePackagePath('core', 'runtime', 'package.json'), dest: staging.intrinsic },
  { src: resolveLsePackagePath('react', 'runtime', 'package.json'), dest: staging.intrinsic },
  { src: resolveLsePackagePath('style', 'runtime', 'package.json'), dest: staging.intrinsic },
  { src: resolveLsePackagePath('loader', 'runtime', 'package.json'), dest: staging.intrinsic },
  { src: resolveInternalPackagePath('react', 'runtime', 'package.json'), dest: staging.intrinsic },
  { src: resolveLsePackagePath('core', 'build', 'Release', 'lse-core.node'), dest: join(staging.intrinsic, '@lse', 'core', 'release') },
  { src: join(PROJECT_ROOT, 'LICENSE'), dest: staging.license }
]
