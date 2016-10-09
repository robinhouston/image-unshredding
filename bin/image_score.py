#!/usr/bin/python
# -*- encoding: utf-8 -*-
from __future__ import division

"""
Read a PNG image from standard input or the file named on the command line,
and write the column similarity scores to standard output in TSPLIB format.
"""

import sys

from PIL import Image

filename = sys.argv[1] if len(sys.argv) > 1 else None
image = Image.open(filename or sys.stdin)
w, h = image.size
data = image.getdata()

def comparePixels(p1, p2):
	"""Compute a similarity score between two pixels.
	"""
	r1, g1, b1 = p1[:3]
	r2, g2, b2 = p2[:3]
	# return (r1-r2)*(r1-r2) + (g1-g2)*(g1-g2) + (b1-b2)*(b1-b2)
	return abs(r1-r2) + abs(g1-g2) + abs(b1-b2)

def compare(col1, col2):
	"""Compute a similarity score between two columns of an image.
	"""
	score = 0
	for row in range(h):
		score += comparePixels(data[row * w + col1], data[row * w + col2])
	return score

score = 0
for col in range(w - 1):
	score += compare(col, col + 1)

print score

