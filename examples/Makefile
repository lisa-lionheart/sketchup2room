

ODIR=build
IN=$(basename $(wildcard *.skp))


MK=../bin/sketchup2room.exe
FLAGS=--out build 

.SUFFIXES: .skp .html

TARGETS = $(addprefix $(ODIR)/, $(addsuffix .html, $(IN)))

all: $(TARGETS)
	echo Done

clean:
	rm -Rf $(ODIR) 

$(ODIR)/%.html: %.skp %.skp.config
	$(MK) $(FLAGS) $(shell cat $<.config) --html $(@F) $<

$(ODIR)/%.html: %.skp
	$(MK) $(FLAGS) --html $(@F) $^