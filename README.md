# Image quantizer

## Basics
For this scholar project, various problem solving methods were used to develop an image compression algorithm. The objective is to write a function making it possible to pass from `n` to `k`, with `k < n`, the number of gray levels necessary for the encoding of an image while preserving as much as possible the starting image quality.

## Formalism

Let the starting image be represented by a matrix `I` of size `w * h` such that *I[i; j] <text>&isin;</text> {0; ... ; n-1}* is the level of the pixel `(i;j)`. Typically, *n = 255* represents white and a value of *n = 0* corresponds to black and the intermediate values correspond to shades of gray.

The compression consists in associating at each level *v <text>&isin;<\text> {0; ... ; n-1}* a new value among *k* values *{v<sub>1</sub>; v<sub>2</sub>;...; v<sub>k</sub>}* as defined by a compression function *g: {0; 1; ... ; n-1} <text>&rarr;</text> {v<sub>1</sub>; v<sub>2</sub>; ... ; v<sub>k</sub>}* . We will define the error *Err(g)* associated with the compression defined by the function *g* as the sum on all the image's pixels of the square of the difference between the value original and the compressed value:

<p align="center">
    <i>
    Err(g) = &sum;<sub>i&isin;[1;w]</sub>&sum;<sub>j&isin;[1;h]</sub> (I[i; j] - g(I[i; j]))<sup>2</sup>
    </i>
</p>
<p align="center">
    <i>
    Err(g) = &sum;<sub>i&isin;[0;n-1]</sub> h[i] (i - g(i))<sup>2</sup>
    </i>
</p>

where *h[i]*, for *i = 0; ... ; n - 1*, is the number of pixels in the image that take the value *i* (that is to say the histogram of the image).

The objective is to determine the function *g* and the values *v<sub>k</sub>* which minimizes this error.
## Example
Let the image given in figure 1

<p align="center">
  <img src="Images/camera.pgm"><br>
  Fig. 1: Image before compression.
</p>

This can be reduced to 2 shades of gray as shown in Figure 2

<p align="center">
  <img src="Images/compressed.pgm"><br>
  Fig. 2: Compression image.
</p>

## Implementation

### Reduction files
* `NaiveImageQuantizer.c`: solve the problem using a naive approach;
* `GreedyReduction.c`: solve the problem using a greedy approach
* `DPReduction.c`: solve the problem using a dynamic programming approach.

### General files
A small application implementing the compression routine includes the `main.c`, `NaiveImageQuantizer.c` files, as well as a PGM image manipulation library, `PortableGrayMap.c`.

## Usage
The quantizer program can be compiled by using the command

```
gcc main.c ChosenQuantizer.c PortableGrayMap.c -o quantizer
```
where `ChosenQuantizer.c` can be either `NaiveImageQuantizer.c`, `GreedyReduction.c` or `DPReduction.c`.

Once compiled, you can compress an image in PGM format into a number of levels and save it using the following command (with `k` the number of desired shade of grey after the compression)
```
./quantizer imageToCompress.pgm 4 compressed. pgm
```
where `imageToCompress.pgm`is a PGM files, 3 are provided in the Images folder, `camera.pgm`, `coins.pgm` and `lena.pgm`.
Note that to compile `main.c` with, namely `GreedyReduction.c` and `DPReduction.c`, you must add the `ImageQuantizer.c` file, ending with the following command
```
gcc main.c GreedyReduction.c PortableGrayMap.c ImageQuantizer.c -o quantizer
```
