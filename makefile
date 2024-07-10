.PHONY: all clean 1 2 3 4 5 6 7 8 9 10

DIRS := 1 2 3 4 5 6 7 8 9 10

all: $(DIRS)

clean: 
	for dir in $(DIRS); do \
		$(MAKE) -C $$dir clean; \
	done

$(DIRS):
	cd $@ && $(MAKE) all
