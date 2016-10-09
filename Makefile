IMAGES = abstract-light-painting.png alaska-railroad.png blue-hour-paris.png \
	lower-kananaskis-lake.png marlet2-radio-board.png nikos-cat.png \
	pizza-food-wallpaper.png the-enchanted-garden.png tokyo-skytree-aerial.png

build: LKH-2.0.7/LKH bin/compute_scores shuffled_images build_message

reconstruct: $(foreach img,$(IMAGES),$(patsubst %,images/reconstructed/%,$(img))) reconstruct_message

nayuki: $(foreach img,$(IMAGES),$(patsubst %,images/nayuki/%,$(img)))

clean:
	rm -rf images/*/* tsp/*/* LKH-2.0.7 nayuki/*.class bin/compute_scores

original_images: $(foreach img,$(IMAGES),$(patsubst %,images/original/%,$(img)))

shuffled_images: $(foreach img,$(IMAGES),$(patsubst %,images/shuffled/%,$(img)))

double_shuffled_images: $(foreach img,$(IMAGES),$(patsubst %,images/double_shuffled/%,$(img)))

build_message:
	@echo
	@echo "Now run 'make reconstruct' to reconstruct the images"

reconstruct_message:
	@echo
	@echo "The reconstructed images are in images/reconstructed"

.PHONY: build reconstruct nayuki clean original_images shuffled_images build_message reconstruct_message
.SECONDARY: # Retain all intermediate files


LKH-2.0.7/Makefile:
	curl -L http://webhotel4.ruc.dk/~keld/research/LKH/LKH-2.0.7.tgz | tar zxf -

LKH-2.0.7/LKH: LKH-2.0.7/Makefile
	$(MAKE) -C LKH-2.0.7

tsp/instances/double_shuffled/1/%.tsp: images/double_shuffled/%.png bin/compute_scores
	bin/compute_scores --rows "$<" > "$@"

tsp/instances/double_shuffled/2/%.tsp: images/semi_reconstructed/%.png bin/compute_scores
	bin/compute_scores --cols "$<" > "$@"

tsp/instances/%.tsp: images/shuffled/%.png bin/compute_scores
	bin/compute_scores "$<" > "$@"

tsp/tours/%.tour: tsp/instances/%.tsp LKH-2.0.7/LKH bin/lkh.sh
	bin/lkh.sh "$<" "$@"

images/original/%.png:
	curl -L -o "$@" https://www.nayuki.io/res/image-unshredder-by-annealing/"$*".png

images/shuffled/%.png: images/original/%.png bin/shuffle_image.py
	bin/shuffle_image.py "$<" > "$@"

images/double_shuffled/%.png: images/shuffled/%.png bin/shuffle_image.py
	bin/shuffle_image.py --rows "$<" > "$@"

images/reconstructed/%.png: tsp/tours/%.tour bin/reconstruct_image.py images/shuffled/%.png
	bin/reconstruct_image.py "$<" images/shuffled/"$*".png > "$@"

images/semi_reconstructed/%.png: tsp/tours/double_shuffled/1/%.tour bin/reconstruct_image.py images/double_shuffled/%.png
	bin/reconstruct_image.py --rows "$<" images/double_shuffled/"$*".png > "$@"

images/double_reconstructed/%.png: tsp/tours/double_shuffled/2/%.tour bin/reconstruct_image.py images/semi_reconstructed/%.png
	bin/reconstruct_image.py --cols "$<" images/semi_reconstructed/"$*".png > "$@"

bin/compute_scores: src/compute_scores.c
	gcc -O3 -Wall --std=c99 -I/usr/local/include -L/usr/local/lib "$<" -lpng -o "$@"


nayuki/%.java:
	curl -L -o "$@" https://www.nayuki.io/res/image-unshredder-by-annealing/"$*".java

nayuki/%.class: nayuki/%.java
	javac "$<"

images/nayuki/%.png: images/shuffled/%.png nayuki/AnnealImageColumns.class
	java -cp nayuki AnnealImageColumns "$<" "$@" 1000000000 4000
