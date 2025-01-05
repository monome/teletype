.PHONY: release format \
	clean clean-docs clean-module clean-simulator clean-tests clean-zip

release: teletype.zip

clean: clean-docs clean-module clean-simulator clean-tests

clean-docs:
	cd docs && make clean && cd ..

clean-module:
	cd module && make clean && cd ..

clean-simulator:
	cd simulator && make clean && cd ..

clean-tests:
	cd tests && make clean && cd ..

clean-zip:
	rm -f teletype.zip

teletype.zip: clean-zip
	cd module && make && cd .. && \
	cd docs && make && cd .. && \
	zip -j teletype.zip \
		module/teletype.hex \
		module/flash.sh \
		module/update_firmware.command \
		docs/teletype.pdf \
		docs/teletype.html \
		docs/cheatsheet/cheatsheet.pdf

format:
	git-clang-format -f --style=file main

format-all:
	find . -type f -name "*.c" -o -name "*.h" | xargs clang-format -style=file -i
