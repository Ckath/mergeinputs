# mergeinputs
for some reason theres some issues using multiple keyboards simultaneously in linux, for some applications. this is a very small utility to merge/combine keyboards in linux into one `merged input` device.

## usage
```
mergeinputs inputeventpaths
example to merge all keyboards: mergeinputs /dev/input/by-id/*-kbd
```
depending on your distro you might either need the `input` group or run mergeinputs as root.

## but how does it work?
leveraging the uinput module, mergeinputs is able to sit before any userspace input drivers:

`/dev/input/eventx /dev/input/eventy -> mergeinputs grabs all key events -> "merged input" device -> input drivers -> userspace applications`

this means any application will only see one keyboard(`merged inputs`) pressing keys, eliminating any issues multiple devices might've caused.

## install
`make install`
