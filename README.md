# Project overview
_**FoE: Remains of Remains**_ (abbreviated as _**FoE: RR**_ or simply _**foerr**_) (working title) is a fan project
aiming to rewrite and improve _Fallout Equestria: Remains_, a game [described](http://foe.ucoz.org/main_en.html) as
"a hardcore mix of platformer, 2D-shooter and roguelike with RPG elements".
The game is based on [_Fallout: Equestria_](https://en.wikipedia.org/wiki/Fallout:_Equestria) by Kkat, which itself is
based both on _Fallout_ and _My Little Pony: Friendship is Magic_.

The original project (_FoE: Remains_) was developed using Flash Player as an engine. A small, yet dedicated community
formed around the game, with its members continuing to play and discuss it long after the development was officially
completed. Several [mods](https://fallout-equestria-remains.fandom.com/wiki/User_Modifications) have also been created,
however making mods is not straightforward - there is no modding API, and mods are usually compiled straight into the
same swf files that the main game uses, which makes enabling multiple mods very difficult. The underlying technology
(Flash Player) is also limiting what can done in terms of improving performance. Game is definitely playable, but it's
not exactly the best experience framerate-wise, with occasional lag spikes. Another big issue is that Flash Player has
reached end of life in 2021, making it not 100% safe to use.

Because of this, there were several attempts to rewrite/port the game to some other technology/engine, however
(as of 2023) no such project has achieved significant, or even moderate success. _FoE: RR_ is yet another attempt.

The aim of this project is to reimplement the whole game from scratch, with improvements to performance,
customizability, extensibility, and new quality of life features (not affecting the overall vibe/style in a significant
way), using free, open and cross-platform technologies. Instead of developing it in a closed usergroup, the source code
is shared freely, contributions are welcome from anyone (as long as they fulfill project goals), and discussion about
the project's direction is encouraged.

# Project goals
1. Create a reimplementation of _FoE: Remains_, faithful in gameplay, style (visual, audio), and overall vibe.
2. Implement the entire campaign of _FoE: Remains_ (Chosen-25).
3. Make the game compatible with major PC platforms - Linux, Windows, and possibly OS X.
4. Design the game with performance and extensibility in mind.
5. Add meaningful quality of life features (e.g. search bar in inventory, scalable GUI, more configurable shortcut keys,
customizable cutie mark, etc.).
6. Make it easy to modify and add content (campaigns, story, levels, characters, items, translations, etc.).
7. Implement a modding system.
8. Fix known bugs, such as the double-frozen debuff, or certain terminals blocking certain projectiles in certain boss
rooms.
9. Incorporate some already existing mods (with their creator's approval), such as additional rooms or balance
improvements.
10. Upscale all textures (ideally to 4k) to make the game look better on high-resolution displays. Provide texture packs
in various other resolutions for improved performance on lower resolution displays.

Features currently **outside** project scope:
* Multiplayer/split screen
* New playable player races/classes
* Game console versions

# Build
Refer to [SFML documentation](https://www.sfml-dev.org/tutorials/2.6/compile-with-cmake.php#installing-dependencies)
for satisfying SFML dependencies. On most Debian-based systems the following should be enough:
```
sudo apt update
sudo apt install -y \
	libfreetype-dev \
	libx11-dev \
	libxrandr-dev \
	libudev-dev \
	libgl-dev \
	libflac-dev \
	libvorbis-dev \
	libopenal-dev \
	libxcursor-dev
```

Then:
```
cd $PROJECT_ROOT
cmake -DCMAKE_BUILD_TYPE=$TYPE -B build
cmake --build . -j`nproc` --config $TYPE
```
Where `TYPE` can be either `Debug` or `Release`.

Source code of dependencies is automatically downloaded on project configure. Libraries are built from source and linked
statically, with the exception of openal32.

# Run
```
cd $PROJECT_ROOT
build/bin/Release/foerr
```
Where `$PROJECT_ROOT` is a directory containing `res` and `fonts` directories.

On Windows, `openal32.dll` must be present in PATH or in current directory.

# Clean
```
cd build
cmake --build . --target clean
```

...or just delete the `build` dir.

# Install
```
cd build
cmake --install . --config Release --prefix $PATH_TO_INSTALL
```
