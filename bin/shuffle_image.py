#!/usr/bin/python
# -*- encoding: utf-8 -*-
from __future__ import division

"""
Shuffle the columns of an image.
"""

import random
import sys

from PIL import Image

image_filename = sys.argv[1] if len(sys.argv) > 1 else None
image = Image.open(image_filename or sys.stdin)
w, h = image.size
data = image.getdata()

output_data = [None] * (w*h)

def copyColumn(src, dst):
	for row in range(h):
		output_data[row * w + dst] = data[row * w + src]

shuffled_cols = range(w)
random.shuffle(shuffled_cols)

for col in range(w):
	copyColumn(col, shuffled_cols[col])

output_image = Image.new("RGB", (w, h))
output_image.putdata(output_data)
output_image.save(sys.stdout, format="png")
