# dep-graphV - A useful tool to analize header dependendencies via graphs

Copyright (C) 2013 - 2015, by Francesco Guastella (guastella.francesco@gmail.com)
Report bugs and download new versions at https://github.com/romeoxbm/depgraphv

This is the distribution of dep-graphV, which is a useful development tool, written in C++ and powered by Qt and GraphViz, that creates a graph of your codebase, by parsing every #include statement found in the project's root folder.

It is helpful when you need to analize and visualize the dependecy tree, highlightining cycles and "useless paths", and It means, in short, that you could drastically reduce build times, especially when working on large-scale projects.

## Main Features
* CMake based build, so it's easy to switch between different platforms/IDEs;
* Build against Qt4 or Qt5 by simply changing DEPGRAPHV_USE_QT5 variable value in CMake;
* OpenGL rendering support;
* Optional recursive folder scan;
* Customizable file extension filters (for both header and source files);
* Graphs can be saved as images (.bmp, .eps, .gd, .gd2, .gif, .jpe, .jpeg, .jpg, .png, .ps, .svg or any other format made available by GraphViz plugins) or in GraphViz dot format;
* Localized user interface (currently only English and Italian are available);
* Persistent user settings;

## Contents
The sources distribution contains the following folders and files:

| Item           | Description                                |
|----------------|--------------------------------------------|
| cmake/         | CMake scripts                              |
| i18n/          | Translation files                          |
| res/           | Resources                                  |
| src/           | Header and source files                    |
| ui/            | Forms and dialogs in Qt Designer UI format |
| AUTHORS        |                                            |
| ChangeLog      |                                            |
| CMakeLists.txt |                                            |
| COPYING        |                                            | 
| README         | This file                                  |
| TODO           |                                            |
	
## Installing	
When installing dep-graphV on Windows, the installer will create the following folders structure:

| Item          | Description                                                                                |
|---------------|--------------------------------------------------------------------------------------------|
| bin/          | Program executable, required libraries and configuration files                             |
| docs/         |                                                                                            |
| AUTHORS       |                                                                                            |
| ChangeLog     |                                                                                            |
| COPYING       |                                                                                            |
| README        |                                                                                            |
| TODO          |                                                                                            |
| src/          | (optional) Headers, sources translation files (.ts), ui files and the resource file (.qrc) |
| translations/ | (optional) Translation files used by dep-graphV (.qm)                                      |
| Uninstall.exe | The uninstaller                                                                            |
