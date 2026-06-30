var Module = Module || {};

Module.preRun = Module.preRun || [];

Module.preRun.push(function () {
    console.log("[Module] Setting up Emscripten environment...");

    ENV.USER = ENV.USER || "web_user";
    ENV.HOME = ENV.HOME || `/home/${ENV.USER}`;
    ENV.OPFS_ROOT = "/opfs";

    const depname = "preload-opfs";
    addRunDependency(depname);
    preloadOPFS(depname)
        .then(function () {
            removeRunDependency(depname);
        })
        .catch(function (err) {
            console.error("[Module] Emscripten setup failed:", err);
            removeRunDependency(depname);
        });
});

async function mkdirp(root, path) {
    let dir = root;

    for (const part of path.split("/").filter(Boolean)) {
        dir = await dir.getDirectoryHandle(part, {create: true});
    }

    return dir;
}

async function fileExists(dir, filename) {
    try {
        await dir.getFileHandle(filename, {create: false});
        return true;
    } catch (e) {
        if (e.name === "NotFoundError")
            return false;

        throw e;
    }
}

async function downloadToOPFS(depname, url, dir, filename) {
    console.log(`[${depname}] Downloading ${url}`)
    const response = await fetch(url);

    if (!response.ok) {
        throw new Error(
            `Download failed: ${url} (${response.status})`
        );
    }

    const fileHandle = await dir.getFileHandle(filename, {create: true});
    const writable = await fileHandle.createWritable();

    try {
        await response.body.pipeTo(writable);
    } catch (e) {
        try {
            await writable.abort();
        } catch (_) {
        }

        throw e;
    }
    console.log(`[${depname}] Pak file successfully preloaded: ${dir.name}/${filename}`);
}

async function ensureFile(depname, url, dir, filename) {
    if (await fileExists(dir, filename)) {
        console.log(`[${depname}] Pak file already existing: ${dir.name}/${filename}`);
        return;
    }

    await downloadToOPFS(depname, url, dir, filename);
}

async function preloadOPFS(depname) {
    console.log(`[${depname}] Preloading pak files to OPFS...`);
    const root = await navigator.storage.getDirectory();

    console.log(`[${depname}] Setting up directories...`);
    const baseDir = await mkdirp(root, "/d3wasm/base");
    const demoDir = await mkdirp(root, "/d3wasm/demo");
    await mkdirp(root, `${ENV.HOME}/.local/d3wasm`);
    await mkdirp(root, `${ENV.HOME}/.config/d3wasm`);

    console.log(`[${depname}] Setting up base files...`);
    for (let i = 0; i <= 8; ++i) {
        const filename = `pak${String(i).padStart(3, "0")}.pk4`;

        await ensureFile(depname,
            `data/base/${filename}`,
            baseDir,
            filename
        );
    }

    //console.log(`[${depname}] Setting up demo files...`);
    //await ensureFile(depname, "data/demo/demo00.pk4", demoDir, "demo00.pk4");

    console.log(`[${depname}] Preloading complete. Wasm program can now be executed.`);
}