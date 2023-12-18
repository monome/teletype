.PHONY: build clean

build: teletype.pdf teletype.html cheatsheet/cheatsheet-core.pdf cheatsheet/cheatsheet-i2c.pdf

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

cheatsheet/cheatsheet-%.pdf: $(wildcard ops/*.toml) ../utils/cheatsheet.py cheatsheet/cheatsheet.tex
	../utils/cheatsheet.py $* "cheatsheet/include.tex"
	cd cheatsheet && latexmk -xelatex cheatsheet.tex -jobname=cheatsheet-$*

cheatsheet/%.tex: $(wildcard ops/*.toml) ../utils/cheatsheet.py
	../utils/cheatsheet.py $* "cheatsheet/$*.tex"

cheatsheet/%.pdf: cheatsheet/cheatsheet.tex cheatsheet/include.tex
	cd cheatsheet && latexmk -xelatex cheatsheet-common.tex -jobname=$*