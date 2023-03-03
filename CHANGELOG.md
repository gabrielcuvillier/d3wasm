*In compliance with the [GPL-3.0](https://opensource.org/licenses/GPL-3.0) license: I declare that this version of the program contains my modifications, which can be seen through the usual "git" mechanism.*  


2019-12  
Contributor(s):  
Gabriel Cuvillier  
>Disabled Exceptions  
- - - - - - - - - - - - - - - - - - - - - - - - - - - 


2019-08  
Contributor(s):  
gab  
>enabled -O3 + additional noinline + fix crash if loading gui not found  
>asyncify: prevent inline of some functions  
- - - - - - - - - - - - - - - - - - - - - - - - - - - 


2019-07  
Contributor(s):  
Gabriel  
gab  
>Allow "Insert" key to open the console (because of issues with Backquote/tilde keys on some systems)  
>Updated version  
>Fixed warning in snd_emitter.cpp  
>Asyncify  
- - - - - - - - - - - - - - - - - - - - - - - - - - - 


2019-06  
Contributor(s):  
gab  
>Temporarily disabled EMTERPRETER to test new LLVM backend  
- - - - - - - - - - - - - - - - - - - - - - - - - - - 


2019-02  
Contributor(s):  
Gabriel Cuvillier  
gab  
>Updated build number  
>Added an interesting video resolution for web viewing  
>Removed too high video modes  
>Merge pull request #3 from dhewm/masterMerge upstream  
>Enabled llvm-lto3 (link-time-optimization). This is making things a little bit smoother, but not that much... Exe is a lot bigger though (from 4.5MB to 5.8MB)  
>Disabled in-game fullscreen. Fullscreen mode is handled by the browser for now  
>Restored default resolution to a little bit lower  
>Bumped version. There have been many internal changes.  
>Use slightly bigger default resolution  
>Sort video modes by height first instead of by width  
>Alt+Enter only issue a vid_restart, and not a fullscreen change  
>Attempt to use "Home" instead of "ESC" key  
>Cleanups in sound system  
>Renamed texGen0S to clipPlane  
>Implemented Blend Lights + Refactored things a bit  
>Attempts to enhance things a bit with timer synchronization and screen updates  
>GL state optimization with TexCoord array  
>Rendersystem must be initialized after soundsystem, otherwise there is some issues  
>Tweaks  
>Use com_asyncSound 3  
>Fixed warnings with C++ 11 enabled  
>Used directly raw string literals to store GLSL shaders+ renamed draw_glsl to draw_gles2+ License additions  
>Fixed issue with speakers  
>Removed AVIDemo code  
>Removed the "ViewNotes" related commands (idSoftware internal stuff).. There is even the names of individuals id Members: John, MrElusive, Seneca, Tim, etc... :)  
>Fixed crash in "screenshot" and "takeviewnotes" commands  
>Network: Just issue an error instead of crashing  
>Disable buffer swapping on WebGL (not possible)  
>Empterpretification (sic!) of TimeDemo code paths  
>Reworked Vertex Attribute arrays state + more GL state opitmization  
>Use display mode 5 by default  
>CriticalSections are no more an issue in SingleThreaded mode, so disable #ifdef around them  
>Fix in thread.cpp  
>Added some usefull resolutions  
>Various additional comments  
>Restore SIMD testing code, but not yet enabled on Emscripten  
>Cancel the main loop callback in Sys_Quit()  
>Removed needless #defines  
- - - - - - - - - - - - - - - - - - - - - - - - - - - 


2019-01  
Contributor(s):  
Gabriel Cuvillier  
gab  
>Added RB_ComputeMVP function  
>Restored some test functions in SIMD code, for later use when SIMD would be available  
>Better comments in the main loop handling code, and terminal handling code  
>Unused include  
>Don't try to set to fullscreen by default. It is the browser that decides!  
>Removed duplicate code  
>Code formatting  
>Restored a cvar mistakenly deleted  
>Removed code related to SkipRenderContext, never implemented in original D3 code  
>Restored some code related to com_asyncSound  
>Additional removal of SDL 1.2 code  
>Restored some code related to com_asyncSound0 (default): mix sound inline, in the main thread1: on Linux: mix sound inline, in a separate thread   on Emscripten: similar to 02: <not supported>: no sound at all3: on Linux: mix sound using UpdateAsyncWrite, in a separate thread   on Emscripten: mix sound using UpdateAsyncWrite, in the main thread,  
>Disabled code to set the Gamma on WebGL. Not supported on the Web  
>Removed SDLIcon stuff on EmscriptenAlso reenabled call to GLimp_SwapInterval method. It is the method internal that will decide if we can swap (Linux) or not (Emscripten)  
>Restored r_wayland compatibility mode, but only for Linux build (not emscripten)  
>Make in_tty readonly on Emscripten (terminal mode not supported)  
>Removed SDL 1.2 code  
>When networking is not supported, nevertheless init num_interfaces to 0  
>Cleaned up uneeded __asm__ code  
>Cleaned up VertexCache, with proper R_CreateIndexCache method  
>Minor optimsDisabled AF_TIMINGS and call to AF::DebugDraw, looks like it take 1% in profiler for nothing  
>Use only one uniform for the fogMatrixI am not 100% sure, but looks like it works ;)  
>Use only one uniform for the lightProjectionMatrixI am not 100% sure, but looks like it works ;)  
>Removed global state in the renderer  
>Cache activeTexture and active VBO, to prevent too much GL calls  
>Don't check GL errors, unless specified  
>additional changes to use embedded libjpeg on Emscripten, and system wide one on Linux  
>Build and use the embedded libjpeg now+ disabled EFX usage too in linux build, for a fair comparison between the two builds  
>Fixed warnings  
>Disabled \*Test_f commands, to reduce code size a bit  
>Fixed a warning  
>Disable inclusion of SDL_Thread/Timer stuff + have our own Sys_Milliseconds implementation  
>Fixed issues with chunk-based loading of game dataNeed to initialize the pak the first time, even data is coming from cache!  
>Updated name of the project to "d3wasm"  
>Code formating  
>Fix sound issue  
>Comment  
>Don't need to set the color mask again  
>Allow to use unsigned short for vertex buffer cache indices (save CPU/GPU bandwidth)Not enabled now, need to be tested that this indices are not too big  
>Disable GL_FRONT buffer stuff + clear alpha channel only at SwapBuffer time  
>Make notarget command really work  
>Correctly enable the vertex attributes arrays  
>Finally got the VertexCache working correctly with VertexProgramTurboShadows  
>Continue on VertexCaches: merged back shadowIndex + indexes, now stuff are working againIn fact, the issue was with VertexProgramTurboShadows...  
>Continue working on VertexCaches: separated index with shadows one and normal ones  
>Added some debug info  
>Attempt to cleanup VertexCache usage to fix some issues  
>Removed unused stuff (MINGW checks, __unix__ checks  
>Removal of global state  
>Fixed hard-to-find-bug with some translucent surfaces not getting drawn correctlyMorality: global state is evil  
>Disabled display of missing sound updates on console. This is annoying and frequent in non multithreading mode  
>Moved some methods again, to match original sources  
>Refactored/moved shaders to separate .c file (for easy regeneration later on) + moved some methods  
>Implemented diffuseCube shader + shader refactor  
>Attempt at Reflection Map shader  
>Added missing utility method in idMat4  
>Use phong shader by default  
>Implemented the WobbleSky texgen  
>Removed R_SkyboxTexGen as it is done by GLSL shaders now  
>Things are getting complex with TexGen, so I make a temporary commit to be able to rollback  
>Disabled postprocess light scale, as it really concerns only old video cards w/out shaders  
>Separated interaction shader with BlinnPhong style, and Phong style. Use "r_usePhong" to switch at runtime, "r_specularExponent" to change the exponent  
>Attempt to fix TextureMatrix issues (row-ordering vs column-ordering). Not clear for now  
>Removed unused shader uniforms  
>Correctly use texturematrix in zfill shader (in case alphatested surfaces) + state optim  
>Disabled debug stencil vizu (optimize the sencil shadow shader by removing color attribute)  
>Removed R_SpecularTexGen (not needed with GLSL backend)  
>Removed image_lodbias. The associated extension is not present on GLES2 and WebGL  
>Replaced USEREGAL with WEBGL  
>Comments on IndexBuffers  
>Integrated both versions of DepthFill shader: one with clip planes, and one without + many GL state optims  
>Merge comit  
>Some silly optimiations in idPluecker coordinates code, after some profiling insights  
>Disabled r_showOverdraw. Not usefull  
>Updated Shadow Shader and Light code to offload a little bit shadow computation from CPU to GPU  
>Restored Vertex Program specific versions of CreateShadowCache, and CreateTurboShadowVolumeThis will be used later, with shadow shader update  
>Updated again r_vertexBufferMegs to more reasonable value (48MB)  
>Reset the vertex cache counter after a vid_restart (fixed issue)  
>Be sure to cleanup vertex/index caches before overriding with new ones  
>Always create temporary index caches for temporary vertex caches  
>Prevent calls to qglDrawElements while there is no index buffer bound  
>removed r_useIndexBuffer cvar (always on)  
>Increased VBOMegs limits, as now we cache EVERYTHING  
>Fixed debuggings counters for VertexCache.cpp  
>Index cache cleanups/fixes & commentsStill have some issues with some interactive surfaces  
>Make VertexCache Alloc buffer type argument mandatory (Vertices OR Indexes)  
>Fixed IndexCache. This is working now, D3 is fully WebGL compliant!!  
>Attempt to use the IndexCache (not yet functional)  
>Separated Index VBOs from Vertex VBOs, as required by GLES 2 spec  
>Added temporary index caches (needed to cache temp indices from GUI & effects)  
>Disable lightingCache, no more needed with GLSL shaders  
>Restored IndexCache code. Will be enhanced later for WebGL  
>text fixes  
>Removed calls to glEnable/Disable(GL_TEXTURE_\*), correctly get number of texture units, cleanups to default GL state  
>SDL2 init ES2 context  
>Multiply modelView and projection in program, not in shaders  
>Restored anisotropy extension  
>Move to GLES 2.0 exclusively + removed dependency on Regal (WIP)  
>Disabled calls to legacy OpenGL 1.x  
>Shadow Cleanup  
>Shadow Shader working  
>Removed unecessary stuff: roq compiler, stub_gl  
>Removed unecessary stuff: renderbump command  
>Removed unecessary stuff: dmap compiler  
>Removed unecessary stuff: glPolygonMode, AlphaTest  
>Refactored backend renderer  
>Removed R_FindARBProgram  
>Ambient shader surface works!  
>Integrate ambient surfaces shader  
>Handling of clipplanes in Depth Shader  
>Comments  
>Zfill shader render path  
>Removed precompressed/DDS image stuff  
>Removed Render Debug Tools  
>Removed SelectInternalFormat, no more needed  
>Fixed stencil shadows no more working  
>Restored call to push/pop matrix. Not yet ok  
>Make calls to glTexImage conformant with ES2/WebGL spec (format must match internalFormat)This allows to remove a layer from regal (Xfer)  
>Disable useless extensions with GLSLusage  
>zfill Shader rework  
>Completed Fog Shader render path  
>FogShader cleanup + first render path code  
>State optimiation in GLSL render path  
>Cancelled again the copyFramebuffer at each frame. Does not seem necessary  
>Only use RGBA8 now  
>Cleanups  
>Reenabled the Fog  
>Properly alloc the size of qglReadPixels dest buffer  
>Do the modelviewmatrix computation differently when there is no Regal (to fix the Zfighting issue).. well this is not solved  
>Integrate interaction shader in the code directly (and not in external GLSL file)  
>Always use RGBA8 internal format, even on non Regal builds  
>Do not run Async timer too much + fixed division issue  
>Fixed memory trash with qglReadPixels (need to read RGB, because dest buffer is RGB and not RGBA)  
>Cleanups  
>Allow to compile again on Linux, using NOMT option (disabled multithreading)  
>Definitively, I am unable to fix the Fog issue. I suspect this is something in RegalI tried use a GL texture combiner, use the code from D3 BFG edition, tweaking the float compilation parameters, etc... Nothing works !?The fogEnterImage texture looks ok, it is more an issue of TexGens I thing. Looks like in the end the Alpha value is always a value of 1 or near to 1I give up :  
>Always copy the Framebuffer at each frame, like it was done with ARB2 shadersIt probably have been done on purpose, for shaders usage  
>Removed useless qglTexCoord2f calls  
>Removed last references to BE_ARB2. For Glasswarp, still enable the drawing even if no shader is bound (partial results)  
>Attempt to disable AsyncServer  
>Disabled unneeded code change  
>Removed CURL, and cleaned up CMakeFile  
>Removed Win32,OSX,AROS, ID_DEDICATED, ID_TOOLS, 3DNow, AltiVec specific code  
>Exit the program properly  
>At end of level, stop immediately the RunGameTic loop after buffering the next map command (otherwise map will be loaded several times)  
>Display FPS by default  
>Added a new "playmap" command, triggered by end of a level. This allows to not have to put RunGameTic() method in EMTERPRETERThe command will be executed at next Frame()  
>Do not popup the fetch window if data already have been fetched from server  
>better async timer update + Added comments about EMTERPRETIFY functions  
>Removed code related to ID_ENFORCE_KEY  
>Do not try to capture images for now (screenshots, demos, debug tools), as this and this will consume too much memory for no real use  
>Fix to make work with full data again  
>Support for 3 modes of loading data: chunked demo, full demo, full game  
>Restored DepthFill pass using classic ARB pathThe shader one had issues + it looks like D3 code: only interactions are done using shaders  
>!IMPORTANT! Fixed the z-buffer fighting issue between GLSL passes and Fixed Pipeline passes (Regal)Let's do the modelviewprojection matrix computation in the shader, like it is done by RegalOtherwise, GLSL shaders and Regal shaders are computing gl_position a little bit differently, leading in Z-fighting  
>Big Refactoring of GLSL code path, better integrated with classic fixed function pipeline  
>Revert "Added default shader (for ambient surfaces)"This reverts commit 759000ce  
>Restored idCommonLocal::Frame function, due to weird crash found sometime. TO BE INVESTIGATED  
>Fixed an issue with TextureMatrixes (the new GLSL path have to use specific code  
>Removed most 3rd party tools (MayaExporter,Radiant,Editors,TypeInfo) + AROS stuff (I don't care)  
>Added default shader (for ambient surfaces)We loose some effects, but gain perfo  
>Disabled BindStageTexture/FinishStageTexture as they were never used  
>Load the default shader  
>Copy the framebuffer at each frame (? not sure if needed for now ?)  
>Load the stencilShadowShader  
>Slight potential optim  
>Removed unused extensions EXT_Depth_Bounds, 3D Textures, and SharedTexturePalette  
>Removed legacy ARB render path  
>Disable trying to render shader passes that are using legacy ARB2 programs  
>GLSL Shader Zbuffer integration  
>Loading the zfill shader  
>Removed unused parrts: ARB2 VertexProgram/FragmentProgram code paths, indexBuffers (never worked), non-VBO code paths and Immediate Mode rendering, twosidedstencil (not used)  
>Disabled some legacy code only used with old gfx cards  
>Disabled RB_BindStageTexture and FinishStageTexture, as they are not used anywhere  
>Revert "Discard Immediate Rendering (no need with VBOs)"This reverts commit 751d4fe3  
>Discard Immediate Rendering (no need with VBOs)  
>Started to duplicate RB_CreateSingleDrawInteractions for GLSL, as it will need specific modifications  
>Disabled backEndRendererHasVertexPrograms. This will be too difficult to maintain both ARB2 shaders in the end, I'll soon remove them  
>Removed unecessary GLSL shaders for nowDepthFill and Stencil Shadows are still done with the fixed function pipeline  
>Disable the IndexCache (does not work properly for now)  
>Reworked GL extensions handling  
>Fixed issue with retreiving glLinkProgram function pointer  
>Disabled calls to FindARBPrograms (method is no more present in Emscripten)  
>Clean GLSL program state in SetDefaultGLState  
>Added GLSL qgl methods  
>Disabled not used GL extensions/methods  
>First attempt to integrate GLSL path (WIP)  
>Merge remote-tracking branch 'remotes/myremote/upstream'# Conflicts:#	neo/CMakeLists.txt#	neo/framework/Common.cpp#	neo/framework/FileSystem.cpp#	neo/framework/Session.cpp#	neo/framework/Session_local.h#	neo/game/Weapon.cpp#	neo/ui/ChoiceWindow.cpp  
>Dont' proceed to main menu until chunck_00 have been loaded  
>Don't load level until the chunck01 is fully loaded  
>Reintegrated call to RunGameTic outside of an emterpreted function (otherwise cause some weird crash, needs to be investigated)  
>Be cooler with emscripten_sleep_with_yield. Just try to match 60hz instead of reentering ASAP  
>Hack to update the Loading Screen more often (and not hangs the browser)  
>Fixed log string  
>Exposed PrintLoadingMessage (will need it later on)  
>Syncing IDBFS after successfull WriteConfiguration()  
>Merge pull request #2 from dhewm/masterMerge upstream  
>Disabled call to SDL_GL_SetSwapInterval => not necessary  
>Reworked async timer & main loop algorithm- removed all references to async_timer- splitted Common::Frame and Session::Frame in subfunctions (pre, mid, last) for parts that do no needs to run in EMTERPRETER- Disabled SIMD test at each frame- Better handling of timer updates to trigger it when necessary- Better handling of emscripten_sleep_with_yield to trigger it when necessary- added emmain() function that is not run in EMTERPRETER  
>Fixed warning [-Wvarargs]  
>Make project compile again on native platforms  
>Added PrintLoadingMessage/Main/Init\* methods to EMPTERPRETIFY  
>Sync FS changes to IDBFS after saving, or after quiting the application  
>Disabled changes for filesystem restart before loading new game (maps will be included in the base .pk4 package)  
>Better handling of the AsyncTimer with emscripten_sleep_with_yield calls  
>fixed glReadPixels  
>Ony use ARB path for now  
>#ifdef EMSCRIPTEN specific code  
>Disabled SwapBuffers (not needed on WebGL)  
>First attempt to use EMTERPRETIFY, to allow synchronous operations that while having possibiloty to yield to the browserThis helps to have the level loading screen being shown and progressively updated  
>make emloopcb a standard C function  
>Reenable CaptureRenderToImage & disable glReadBuffer calls  
>Restart Filesystem before new game, so that we can find newly loaded data  
>Fixes for single binary (NoDLL)  
>Merge pull request #1 from dhewm/masterTest  
>Whitespaces  
>Updated GL extension checks with latest Regal changes  
>More discard of ARB2 path  
>Better demo version support  
>Fix issues with terminal/stdin usage on Emscripten (unwanted prompt)  
>Discard ARB2 calls  
>Additional EMSCRIPTEN #ifdefs  
>Fix compilation error  
- - - - - - - - - - - - - - - - - - - - - - - - - - - 


2018-12  
Contributor(s):  
gab  
>Allow Demo version  
>Disable Fog Falloff Image (does not work)  
>Correct check for Anisotropy extension  
>Restored CheckPrecompressedImage and SelectInternalFormat  
>EMSCRIPTEN cleanup / Formating  
>Misc cleanup  
>Threads cleanup  
>Restored image_useCompression  
>Net update  
>Thread update  
>Clear Alpha at end of each frame, to prevent framebuffer being blended with canvas color  
>Better GL extension support  
>Always use RGBA8 internal format  
>Added new com_machinespec tailored for emscripten  
>In Work  
>Properly check for GL Extensions based on Regal features  
>Restored default value for image_usePrecompressed  
>Better handling of background download thread start/stop  
>Halves the system requirements for Emscripten  
>Properly check for GL Extensions based on Regal features  
>Disabled GL_CLAMP_TO_BORDER  
>Disabled image capture (not working for now)  
>Do not use compressed textures + disabled call to GL_TEXTURE_BORDER_COLOR  
>Disabled calls to glPolygonMode  
>Simulated Multithreading
>Completely disabled threads  
>Restored number of critical sections, even if threads are not used  
>Fixed gl buffer method names (removed "ARB")  
>Implemented main loop for emscripten  
>Use Regal  
>Merged wasm in master  
>Restored ARB render path (no shaders)  
>Enable static linking of game (no DLL)  
>First Emscripten fixes  
- - - - - - - - - - - - - - - - - - - - - - - - - - - 

