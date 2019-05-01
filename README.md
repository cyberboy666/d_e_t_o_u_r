# Raspberry Pi Frame Sampler

Proof-of-concept video sampler made over a few beers with @langolierz 🍻

* Records pixel buffers from CSI Camera into RAM memory (`vector<ofPixel>`)
* Playback can be reversed, sped up, slowed down
* Max ~30 seconds with NTSE (640 x 480) in RGB24, when GPU split set to 128MB
* Crashes / runs out of memory at < 200MB

**Controls**

* Spacebar to open CSI Camera
* R key to toggle recording frames
* C to clear recorded frames
* < Left and Right > keys to set forwards or backwards
* 1 - 9 keys to set speed
* Up and Down keys to change capture resolution

**Installation**

Raspberry Pi 3 with Raspbian Stretch, installed OF 0.10.1 dependencies

[Cross-compiling guide Armv6l](https://github.com/autr/pi/blob/master/of-cross-compile-guide.md)