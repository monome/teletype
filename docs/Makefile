.PHONY: build clean

build: teletype.pdf teletype.html cheatsheet/cheatsheet.pdf

clean:
	rm -f teletype.pdf && \
		rm -f teletype.html && \
		rm -f cheatsheet/include.tex && \
		cd cheatsheet && \
			latexmk -xelatex -c && \
			rm -f cheatsheet.pdf

teletype.pdf: $(wildcard *.md ops/*.md ops/*.toml) \
		../utils/docs.py ../CHANGELOG.md
	../utils/docs.py teletype.pdf

teletype.html: $(wildcard *.md ops/*.md ops/*.toml) \
		../utils/docs.py ../CHANGELOG.md $(wildcard ../utils/templates/*)
	../utils/docs.py teletype.html

cheatsheet/include.tex: $(wildcard ops/*.toml) ../utils/cheatsheet.py
	../utils/cheatsheet.py "cheatsheet/include.tex"

cheatsheet/cheatsheet.pdf: cheatsheet/cheatsheet.tex cheatsheet/include.tex
	cd cheatsheet && latexmk -xelatex cheatsheet.tex
