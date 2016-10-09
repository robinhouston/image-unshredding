#!/usr/bin/python
# -*- encoding: utf-8 -*-
from __future__ import division

"""
Take a TSPLIB tour file and use it to reconstruct an image from the column-shuffled version.
"""

import optparse
import sys

from PIL import Image

parser = optparse.OptionParser(usage="%prog [--rows | --cols] tour.tsp [input.png] > reconstructed.png")
parser.add_option("-r", "--rows",
                  action="store_true",
                  help="reconstruct rows")
parser.add_option("-c", "--cols",
                  action="store_true",
                  help="reconstruct columns (default)")
(options, args) = parser.parse_args()

if options.rows and options.cols:
	parser.error("Cannot specify both --rows and --cols")
reconstruct_rows = options.rows

tour_filename = args[0]
image_filename = args[1] if len(args) > 1 else None

# Read the tour
tour = []
with open(tour_filename, 'r') as f:
	in_tour_section = False
	for line in f:
		line = line.strip()
		if line == "TOUR_SECTION":
			in_tour_section = True
		elif in_tour_section:
			node = int(line)
			if node == -1:
				break
			tour.append(node - 2)

i = tour.index(-1)
tour = tour[i+1:] + tour[:i]

image = Image.open(image_filename or sys.stdin)
w, h = image.size
data = image.getdata()

output_data = [None] * (w*h)

if reconstruct_rows:
	def copyRow(src, dst):
		for col in range(w):
			output_data[dst * w + col] = data[src * w + col]

	for row in range(h):
		copyRow(tour[row], row)

else:
	def copyColumn(src, dst):
		for row in range(h):
			output_data[row * w + dst] = data[row * w + src]

	for col in range(w):
		copyColumn(tour[col], col)

output_image = Image.new("RGB", (w, h))
output_image.putdata(output_data)
output_image.save(sys.stdout, format="png")
