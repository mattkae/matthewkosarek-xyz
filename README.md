# MatthewKosarek.xyz
This contains the entirety of https://matthewkosarek.xyz, which is my personal website. Feel free to look around and take any snippets that you like from it!

If you are on Github, this is a mirror of: https://git.matthewkosarek.xyz/matthew_kosarek_xyz/

## Running
With python installed, run `./run_dev.sh` and you should be good to go. Then navigate to `localhost:8080` in your browser.

## Building Themes

### Build the shaders
```
cd themes/src
node tools/shaders.js
```

### Compile
```
cd themes
./compile-commands.sh
make
```
