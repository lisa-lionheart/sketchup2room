
.SUFFIXES: .md .html
TARGETS = $(addsuffix .html, $(basename $(wildcard *.md)))

all: $(TARGETS)

clean:
	rm *.html

%.html: %.md
	cat header.tmpl > $@
	marked -gfm $^ >> $@
