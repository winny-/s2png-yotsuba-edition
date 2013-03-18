s2png yotsuba edition
=====================

Why?
----
The [upstream sources](http://s2png.sourceforge.net/) have been untouched for seven years. Its `exists` function made me cross. It didn't handle Unix pipes. Heck, it didn't *even* compile. I saw that some people from [/g/](https://boards.4chan.org/g/) [got it working](https://github.com/dbohdan/s2png), but it feels rough.

Installation
------------
1. Dependencies:
  - [libgd](https://bitbucket.org/pierrejoye/gd-libgd)
2. Run `make`.
3. Install using `make install` or manually.

Invocation
----------
- `-h` is help.
- `-v` is verbose
- `-o` is the file output. Use `-` for stdout.
- `-w` sets the image width.
- `-e` is encode; `-d` is decode.
- `-b` sets the banner at the bottom of the image.
- `file` can be `-` for stdin.

Examples
--------

Usage:

	$ s2png -h
	s2png "something to png" version 0.02
	usage: s2png [-h] [-v] [-o filename] [-w width (600)] [-e (default) | -d] [-b banner] file

Create `s2png-yotsuba-edition.tar.xz.png`:

	$ s2png s2png-yotsuba-edition.tar.xz

Compress `greentexts.txt` using `xz` and create `greentexts.txt.xz.png` with banner ">tfw all my greentexts are not green":

	$ xz -c greentexts.txt | s2png -ve -b '>tfw all my greentexts are not green' -o greentexts.txt.xz.png -
	- -> greentexts.txt.xz.png
	Mode:   Encode
	Width:  200
	Height: 8
