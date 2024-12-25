Images with "X" in the name are blank images (e.g., "disk3X.img" is
the blank image that exists before the files are added which yields
"disk3.img").

The original versions of all files stored into the disk images can be
found in the "originals/ subdirectory". You can use "diff" to compare
the original file with what is obtained using "cat360fs" from some disk
image (e.g., compare "originals/alphabet_short.txt" with the results
of "cat360fs --image disk3.img --file alphabet_short.txt").
