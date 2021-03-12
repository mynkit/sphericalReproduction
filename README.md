# Setup

## openFrameworks

1. download openFrameworks0.11.0 from [website](https://openframeworks.cc/download/) according to your environment.

2. unzip openFrameworks0.11.0 and move it to your folder.

3. enter the following commands.

```sh
cd openFrameworksFolder/apps/myApps/
git clone git@github.com:mynkit/crazySoundRoom.git
cd crazySoundRoom
```

4. add addon ofxCsv from https://github.com/paulvollmer/ofxCsv#installation--usage

5. add addon ofxJSON from https://github.com/jeffcrouse/ofxJSON#to-use


### Python

1. install python3.8 and pipenv

2. enter the following commands.

```sh
export ARCHFLAGS="-arch x86_64" # macOS only
pipenv sync --dev
```

# Run

`sphericalReproduction.xcodeproj`を開いてRunすれば動きます
