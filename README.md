# Cint - C Interactive (REPL)

A C REPL to run functions from any `.so` you want! (even your own ones)

## Building
### Requirements
- libncurses (.so) <br>
    The package for most distros is: `ncurses-devel`

### Cloning
```bash
git clone --recursive git@github.com:ShakedGold/cint.git
```

### Building
#### All
```bash
make target=debug
```
or
```bash
make target=debug all
```
#### Clean
```bash
make clean
```
#### Purge (cleans up the library compilation)
```bash
make purge
```