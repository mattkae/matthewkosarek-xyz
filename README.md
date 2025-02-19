# MatthewKosarek.xyz
This contains the entirety of https://matthewkosarek.xyz, which is my personal website. Feel free to look around and take any snippets that you like from it!

If you are on Github, this is a mirror of: https://git.matthewkosarek.xyz/matthew_kosarek_xyz/

## Running
```sh
./run_dev.sh
firefox localhost:8080
```

## Build Themes
1. Install emscripten: https://emscripten.org/docs/getting_started/downloads.html

2. Compile the shaders 
   ```sh
   cd themes/src
   node tools/shaders.js
   ```
3. Build the project
   ```sh
   cd ..
   ./compile-commands.sh
   make
   ```

## Update Posts
```sh
./publish.sh
```

## Publish New Version
```sh
./upload.sh
```
