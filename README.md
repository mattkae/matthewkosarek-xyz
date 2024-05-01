# MatthewKosarek.xyz
This contains the entirety of https://matthewkosarek.xyz, which is my personal website. Feel free to look around and take any snippets that you like from it!

If you are on Github, this is a mirror of: https://git.matthewkosarek.xyz/matthew_kosarek_xyz/

## Running
```sh
./run_dev.sh
firefox localhost:8080
```

## Building

### Themes
```sh
# Compile the shaders
cd themes/src
node tools/shaders.js

# Build the project
cd ..
./compile-commands.sh
make
```

### Posts
```sh
./publish.sh
```

## Upload
```sh
./upload.sh
```
