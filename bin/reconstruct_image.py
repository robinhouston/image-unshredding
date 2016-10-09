#!/usr/bin/python
# -*- encoding: utf-8 -*-
from __future__ import division

"""
Take a TSPLIB tour file and use it to reconstruct an image from the column-shuffled version.
"""

import sys

from PIL import Image

tour_filename = sys.argv[1]
image_filename = sys.argv[2] if len(sys.argv) > 2 else None

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

def copyColumn(src, dst):
	for row in range(h):
		output_data[row * w + dst] = data[row * w + src]

for col in range(w):
	copyColumn(tour[col], col)

output_image = Image.new("RGB", (w, h))
output_image.putdata(output_data)
output_image.save(sys.stdout, format="png")
