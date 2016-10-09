#!/usr/bin/python
# -*- encoding: utf-8 -*-
from __future__ import division

"""
Shuffle the columns of an image.
"""

import optparse
import random
import sys

from PIL import Image

parser = optparse.OptionParser(usage="%prog [--rows | --cols] [input.png] > shuffled.png")
parser.add_option("-r", "--rows",
                  action="store_true",
                  help="shuffle rows")
parser.add_option("-c", "--cols",
                  action="store_true",
                  help="shuffle columns (default)")
(options, args) = parser.parse_args()

if options.rows and options.cols:
	parser.error("Cannot specify both --rows and --cols")
shuffle_rows = options.rows

image_filename = args[0] if len(args) > 0 else None
image = Image.open(image_filename or sys.stdin)
w, h = image.size
data = image.getdata()

output_data = [None] * (w*h)

if shuffle_rows:
	def copyRow(src, dst):
		for col in range(w):
			output_data[dst * w + col] = data[src * w + col]

	shuffled_rows = range(h)
	random.shuffle(shuffled_rows)

	for row in range(h):
		copyRow(row, shuffled_rows[row])

else:
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
