var Module;
if (Module['preRun'] instanceof Array) {
  Module['preRun'].push(setupD3memfs);
} else {
  Module['preRun'] = [setupD3memfs];
}

(function(d, script) {
  script = d.createElement('script');
  script.type = 'text/javascript';
  script.async = true;
  script.onload = function(){
    // remote script has loaded
  };
  script.src = 'pak000.js';
  d.getElementsByTagName('head')[0].appendChild(script);
}(document));

(function(d, script) {
  script = d.createElement('script');
  script.type = 'text/javascript';
  script.async = true;
  script.onload = function(){
    // remote script has loaded
  };
  script.src = 'pak001.js';
  d.getElementsByTagName('head')[0].appendChild(script);
}(document));


(function(d, script) {
  script = d.createElement('script');
  script.type = 'text/javascript';
  script.async = true;
  script.onload = function(){
    // remote script has loaded
  };
  script.src = 'pak002.js';
  d.getElementsByTagName('head')[0].appendChild(script);
}(document));

(function(d, script) {
  script = d.createElement('script');
  script.type = 'text/javascript';
  script.async = true;
  script.onload = function(){
    // remote script has loaded
  };
  script.src = 'pak003.js';
  d.getElementsByTagName('head')[0].appendChild(script);
}(document));

(function(d, script) {
  script = d.createElement('script');
  script.type = 'text/javascript';
  script.async = true;
  script.onload = function(){
    // remote script has loaded
  };
  script.src = 'pak004.js';
  d.getElementsByTagName('head')[0].appendChild(script);
}(document));

(function(d, script) {
  script = d.createElement('script');
  script.type = 'text/javascript';
  script.async = true;
  script.onload = function(){
    // remote script has loaded
  };
  script.src = 'pak005.js';
  d.getElementsByTagName('head')[0].appendChild(script);
}(document));

(function(d, script) {
  script = d.createElement('script');
  script.type = 'text/javascript';
  script.async = true;
  script.onload = function(){
    // remote script has loaded
  };
  script.src = 'pak006.js';
  d.getElementsByTagName('head')[0].appendChild(script);
}(document));

(function(d, script) {
  script = d.createElement('script');
  script.type = 'text/javascript';
  script.async = true;
  script.onload = function(){
    // remote script has loaded
  };
  script.src = 'pak007.js';
  d.getElementsByTagName('head')[0].appendChild(script);
}(document));

(function(d, script) {
  script = d.createElement('script');
  script.type = 'text/javascript';
  script.async = true;
  script.onload = function(){
    // remote script has loaded
  };
  script.src = 'pak008.js';
  d.getElementsByTagName('head')[0].appendChild(script);
}(document));





function setupD3memfs() {
  console.info("Creating d3wasm data folder (/usr/local/share/d3wasm/base)");
  FS.createPath('/', 'usr', true, true);
  FS.createPath('/usr', 'local', true, true);
  FS.createPath('/usr/local', 'share', true, true);
  FS.createPath('/usr/local/share', 'd3wasm', true, true);
  FS.createPath('/usr/local/share/d3wasm', 'base', true, true);

  console.info("Creating user home folder (/home/web_user)");
  FS.createPath('/', 'home', true, true);
  FS.createPath('/home', 'web_user', true, true);

  console.info("Mounting user home to IDBFS");
  FS.mount(IDBFS, {}, '/home/web_user');

  FS.syncfs(true, function (err) {
    if (err) {
      console.error(err);
    }
    else {
      console.info("Mounting user home completed");
      console.info("Creating user home config and local folders if necessary (~/.config, ~/.local/d3wasm/base)");
      FS.createPath('/home/web_user', '.config', true, true);
      FS.createPath('/home/web_user/.config', 'd3wasm', true, true);
      FS.createPath('/home/web_user', '.local', true, true);
      FS.createPath('/home/web_user/.local', 'd3wasm', true, true);
      FS.createPath('/home/web_user/.local/d3wasm', 'base', true, true);
      Module['removeRunDependency']("setupD3memfs");
    }
  });

  Module['addRunDependency']("setupD3memfs");
}
