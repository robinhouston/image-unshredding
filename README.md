# Image unshredding using a TSP solver

## Introduction

Yesterday I saw [a fun demo by Nayuki](https://www.nayuki.io/page/image-unshredder-by-annealing) using simulated annealing to reconstruct photographs whose columns have been shuffled.

For example, the photograph Blue Hour in Paris (CC licensed by [Falcon® Photography](https://www.flickr.com/photos/falcon_33/15178077733/)):

![Blue hour in Paris](https://robinhouston.github.io/image-unshredding/images/original/blue-hour-paris.png)

is shuffled to produce:

![Blue hour in Paris, shuffled](https://robinhouston.github.io/image-unshredding/images/shuffled/blue-hour-paris.png)

and the simulated annealing algorithm (starting temperature 4000, 1 billion iterations) reconstructs this:

![Blue hour in Paris, reconstructed using simulated annealing](https://robinhouston.github.io/image-unshredding/images/nayuki/blue-hour-paris.png)

Subsequently Sangaline [showed that the images can be reconstructed faster and more effectively](http://sangaline.com/blog/image_unshredder/) using a simple greedy algorithm to pick the most-similar column at each step. The greedy algorithm produces this:

![Blue hour in Paris, reconstructed using a greedy algorithm](https://robinhouston.github.io/image-unshredding/images/sangaline/blue-hour-paris.png)

which is quite close to the original, though you can see some misplaced columns in the sky at the right.

## This project

This project uses a [fast approximate solver](http://webhotel4.ruc.dk/~keld/research/LKH/) for the Travelling Salesman Problem to reconstruct the images quickly and perfectly.

![Blue hour in Paris, reconstructed using LKH](https://robinhouston.github.io/image-unshredding/images/reconstructed/blue-hour-paris.png)

You will notice that the image is flipped, but otherwise reconstructed perfectly. It is impossible in general to distinguish an image from its flipped version when the columns have been shuffled, and all the algorithms mentioned here produce flipped reconstructions half the time.

I believe this algorithm can reconstruct all the images in Nayuki’s demo perfectly.

## Running the code

* Clone the repository
* Run `make` to download and shuffle the images, and download and compile LKH.
* Now you can run `make reconstruct` to reconstruct the images from their shuffled versions using LKH.

You can also run `make nayuki` to reconstruct the images using Nayuki’s simulated annealing code.

## Prerequisites

You will need a working build environment (Make and a C compiler), and `curl` is used to download files from the web. You also need libpng >= 1.6 (which the Makefile assumes to be in `/usr/local`, but that is easy to change). The simpler bits of image manipulation are done using Python 2 and require the [Python Imaging Library](http://www.pythonware.com/products/pil/) or a compatible fork such as [Pillow](https://pillow.readthedocs.io/en/3.4.x/).
